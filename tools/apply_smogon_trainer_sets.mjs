#!/usr/bin/env node

import fs from "node:fs";

const TRAINERS_PATH = "src/data/trainers.party";
const DOC_PATH = "docs/mob_trainer_sets_showdown.md";
const SETS_BASE_URL = "https://data.pkmn.cc/sets";

const args = new Set(process.argv.slice(2));
const shouldApply = args.has("--apply");
const shouldWriteDoc = args.has("--write-doc") || shouldApply;
const includeTera = args.has("--include-tera");

const bossClasses = new Set([
  "Aqua Admin",
  "Aqua Leader",
  "Arena Tycoon",
  "Champion",
  "Dome Ace",
  "Elite Four",
  "Factory Head",
  "Leader",
  "Magma Admin",
  "Magma Leader",
  "Palace Maven",
  "Pike Queen",
  "Pkmn Trainer 1",
  "Pyramid King",
  "Rival",
  "RS Protag",
  "Salon Maiden",
]);

const excludedTrainerIds = new Set([
  "TRAINER_NONE",
  "TRAINER_STEVEN",
  "TRAINER_BRENDAN_PLACEHOLDER",
  "TRAINER_MAY_PLACEHOLDER",
]);

const formatPreference = [
  "ou",
  "ubers",
  "uu",
  "ru",
  "nu",
  "pu",
  "zu",
  "nfe",
  "lc",
  "nationaldex",
  "nationaldexuu",
  "nationaldexru",
  "nationaldexnu",
  "nationaldexpu",
  "nationaldexzu",
  "nationaldexmonotype",
  "monotype",
  "anythinggoes",
];

const statOrder = [
  ["hp", "HP"],
  ["atk", "Atk"],
  ["def", "Def"],
  ["spa", "SpA"],
  ["spd", "SpD"],
  ["spe", "Spe"],
];

function normalizeName(name) {
  return name
    .replace(/^SPECIES_/, "")
    .replace(/♀/g, "F")
    .replace(/♂/g, "M")
    .toLowerCase()
    .replace(/[^a-z0-9]/g, "");
}

function isBossTrainer(trainer) {
  return bossClasses.has(trainer.className) || excludedTrainerIds.has(trainer.id);
}

function formatRank(format) {
  if (
    format.includes("doubles")
    || format.includes("vgc")
    || format.includes("battle")
    || format.includes("1v1")
    || format.includes("bdsp")
  ) {
    return 1000;
  }

  const preferred = formatPreference.indexOf(format);
  if (preferred >= 0)
    return preferred;
  if (format.startsWith("nationaldex"))
    return 50;

  return 100;
}

async function fetchJson(url) {
  const response = await fetch(url, {
    headers: {
      "user-agent": "pokeemerald-expansion-shared-power trainer set updater",
    },
  });

  if (!response.ok)
    throw new Error(`Failed to fetch ${url}: ${response.status} ${response.statusText}`);

  return response.json();
}

async function loadSmogonData() {
  const byGeneration = [];
  for (let generation = 9; generation >= 1; generation--) {
    const data = await fetchJson(`${SETS_BASE_URL}/gen${generation}.json`);
    const speciesIndex = new Map();
    for (const species of Object.keys(data))
      speciesIndex.set(normalizeName(species), species);
    byGeneration.push({ generation, data, speciesIndex });
  }
  return byGeneration;
}

function chooseFirst(value) {
  if (Array.isArray(value))
    return chooseFirst(value[0]);
  return value;
}

function isGimmickItem(item) {
  if (!item)
    return false;

  if (item.endsWith(" Z"))
    return true;

  if ((item.endsWith("ite") || item.endsWith("ite X") || item.endsWith("ite Y")) && item !== "Eviolite")
    return true;

  return item === "Red Orb" || item === "Blue Orb";
}

function pickSet(smogonData, speciesName) {
  const normalized = normalizeName(speciesName);

  for (const generationData of smogonData) {
    const smogonSpecies = generationData.speciesIndex.get(normalized);
    if (!smogonSpecies)
      continue;

    const formats = generationData.data[smogonSpecies];
    const rankedFormats = Object.entries(formats)
      .filter(([format]) => formatRank(format) < 1000)
      .sort((a, b) => formatRank(a[0]) - formatRank(b[0]));

    if (rankedFormats.length === 0)
      continue;

    const [format, sets] = rankedFormats[0];
    const [setName, set] = Object.entries(sets)[0];
    return {
      generation: generationData.generation,
      format,
      setName,
      smogonSpecies,
      set,
    };
  }

  return null;
}

function parseTrainers(text) {
  const firstTrainerIndex = text.search(/^=== /m);
  const prefix = text.slice(0, firstTrainerIndex);
  const trainerText = text.slice(firstTrainerIndex);
  const rawBlocks = trainerText.split(/(?=^=== )/m).filter(Boolean);

  const trainers = rawBlocks.map((raw) => {
    const id = raw.match(/^===\s+([^=]+?)\s+===/m)?.[1].trim();
    const className = raw.match(/^Class:\s*(.*)$/m)?.[1].trim() ?? "";
    const bodyStart = raw.indexOf("\n\n");
    const header = bodyStart >= 0 ? raw.slice(0, bodyStart) : raw.trimEnd();
    const partyText = bodyStart >= 0 ? raw.slice(bodyStart + 2).trimEnd() : "";
    const monBlocks = partyText.length > 0 ? partyText.split(/\n{2,}/) : [];
    return { raw, id, className, header, monBlocks };
  });

  return { prefix, trainers };
}

function parseMonHeader(line) {
  const withoutItem = line.replace(/\s+@\s+.*$/, "");
  const parentheticalSpecies = withoutItem.match(/\(([^)]+)\)(?:\s+\([MF]\))?$/);
  if (parentheticalSpecies)
    return parentheticalSpecies[1].trim();

  return withoutItem.replace(/\s+\([MF]\)$/, "").trim();
}

function parseLevel(monBlock) {
  const match = monBlock.match(/^Level:\s*(\d+)$/m);
  return match ? Number(match[1]) : null;
}

function formatStats(stats) {
  if (!stats)
    return null;

  const chosen = Array.isArray(stats) ? stats[0] : stats;
  return statOrder
    .filter(([key]) => chosen[key] !== undefined)
    .map(([key, label]) => `${chosen[key]} ${label}`)
    .join(" / ");
}

function normalizeMove(move) {
  const selected = chooseFirst(move);
  if (typeof selected === "string" && selected.startsWith("Hidden Power "))
    return "Hidden Power";
  return selected;
}

function formatMonBlock(originalBlock, picked) {
  const lines = originalBlock.split("\n");
  const originalHeader = lines[0];
  const level = parseLevel(originalBlock);
  const set = picked.set;
  const item = chooseFirst(set.item);
  const heldItem = isGimmickItem(item) ? null : item;
  const ability = chooseFirst(set.ability);
  const nature = chooseFirst(set.nature);
  const evs = formatStats(set.evs);
  const ivs = formatStats(set.ivs);
  const teraType = chooseFirst(set.teratypes);
  const moves = (set.moves ?? []).map(normalizeMove).filter(Boolean).slice(0, 4);

  const block = [];
  const baseHeader = originalHeader.replace(/\s+@\s+.*$/, "");
  block.push(heldItem ? `${baseHeader} @ ${heldItem}` : baseHeader);

  if (ability)
    block.push(`Ability: ${ability}`);
  if (level !== null)
    block.push(`Level: ${level}`);
  if (evs)
    block.push(`EVs: ${evs}`);
  if (nature)
    block.push(`Nature: ${nature}`);
  if (ivs)
    block.push(`IVs: ${ivs}`);
  if (includeTera && teraType)
    block.push(`Tera Type: ${teraType}`);

  for (const move of moves)
    block.push(`- ${move}`);

  return block.join("\n");
}

function summarizeSource(picked) {
  return `Gen ${picked.generation} ${picked.format.toUpperCase()} ${picked.setName}`;
}

function applySets(parsed, smogonData) {
  const changes = [];
  const misses = new Map();

  const outputBlocks = parsed.trainers.map((trainer) => {
    if (isBossTrainer(trainer) || trainer.monBlocks.length === 0)
      return trainer.raw.trimEnd();

    let trainerChanged = false;
    const newMonBlocks = trainer.monBlocks.map((monBlock) => {
      const headerLine = monBlock.split("\n")[0];
      const species = parseMonHeader(headerLine);
      const picked = pickSet(smogonData, species);

      if (!picked) {
        misses.set(species, (misses.get(species) ?? 0) + 1);
        return monBlock;
      }

      trainerChanged = true;
      const updatedBlock = formatMonBlock(monBlock, picked);
      changes.push({
        trainerId: trainer.id,
        trainerClass: trainer.className,
        species,
        level: parseLevel(monBlock),
        source: summarizeSource(picked),
        block: updatedBlock,
      });
      return updatedBlock;
    });

    if (!trainerChanged)
      return trainer.raw.trimEnd();

    return `${trainer.header}\n\n${newMonBlocks.join("\n\n")}`;
  });

  return {
    text: `${parsed.prefix}${outputBlocks.join("\n\n")}\n`,
    changes,
    misses,
  };
}

function buildDoc(changes, misses) {
  const byTrainer = new Map();
  for (const change of changes) {
    if (!byTrainer.has(change.trainerId))
      byTrainer.set(change.trainerId, []);
    byTrainer.get(change.trainerId).push(change);
  }

  const out = [];
  out.push("# Mob Trainer Sets");
  out.push("");
  out.push("This file contains exact non-boss trainer team spoilers in Showdown format.");
  out.push("");
  out.push("These sets are mechanically applied to trainers outside the boss classes handled in separate logs. Trainer levels are preserved. Tera Type suggestions, Z-Crystals, and Mega Stones from Smogon are intentionally omitted for mob trainers so broad trainer cleanup does not turn every ordinary trainer into a gimmick battle.");
  out.push("");
  out.push(`Updated Pokemon: ${changes.length}`);
  out.push(`Updated trainers: ${byTrainer.size}`);
  out.push("");
  out.push("## Sets");
  out.push("");
  out.push("```text");

  for (const [trainerId, trainerChanges] of byTrainer.entries()) {
    out.push(`=== ${trainerId} ===`);
    out.push("");
    for (const change of trainerChanges) {
      out.push(change.block);
      out.push("");
    }
    out.push("");
  }

  out.push("```");
  out.push("");
  out.push("## Source Formats");
  out.push("");

  const sourceCounts = new Map();
  for (const change of changes) {
    const key = `${change.species}: ${change.source}`;
    sourceCounts.set(key, (sourceCounts.get(key) ?? 0) + 1);
  }
  for (const [source, count] of [...sourceCounts.entries()].sort((a, b) => a[0].localeCompare(b[0])))
    out.push(`- ${source} (${count})`);

  if (misses.size > 0) {
    out.push("");
    out.push("## No Smogon Set Found");
    out.push("");
    for (const [species, count] of [...misses.entries()].sort((a, b) => a[0].localeCompare(b[0])))
      out.push(`- ${species} (${count})`);
  }

  return `${out.join("\n")}\n`;
}

const originalText = fs.readFileSync(TRAINERS_PATH, "utf8");
const parsed = parseTrainers(originalText);
const smogonData = await loadSmogonData();
const result = applySets(parsed, smogonData);

console.log(`Eligible Pokemon updated: ${result.changes.length}`);
console.log(`Species without Smogon sets: ${result.misses.size}`);

if (shouldApply)
  fs.writeFileSync(TRAINERS_PATH, result.text);

if (shouldWriteDoc)
  fs.writeFileSync(DOC_PATH, buildDoc(result.changes, result.misses));

if (!shouldApply)
  console.log("Dry run only. Pass --apply to update trainer data and docs.");
