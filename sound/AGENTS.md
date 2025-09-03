# Sound Guidelines

## Sample formats

- **direct_sound_samples/**: 8-bit signed PCM, mono, 13,379 Hz. Convert external audio with a tool such as:
  `ffmpeg -i input.wav -c:a pcm_s8 -ac 1 -ar 13379 sound/direct_sound_samples/<name>.aif`.
  The build system (via `audio_rules.mk`) converts `.aif` files to `.bin` during `make`.
- **programmable_wave_samples/**: raw 4-bit PCM, exactly 32 samples (16 bytes). Update
  `sound/programmable_wave_data.inc` when adding new `.pcm` files.

## Songs and voicegroups

- **Songs** live under `sound/songs/`.
  1. Place the `.mid` file in `sound/songs/midi/`.
  2. Add an entry to `sound/songs/midi/midi.cfg` with the desired `mid2agb` options and voicegroup.
  3. Run `make` (which includes `audio_rules.mk`) to convert the MIDI to assembly and rebuild the objects.
  4. Register the song in `sound/song_table.inc` using the `song` macro.
- **Voicegroups** live under `sound/voicegroups/`.
  1. Create or edit the `.inc` file describing the instruments.
  2. Include it in `sound/voice_groups.inc`.
  3. Run `make` so headers/binaries such as `sound/voice_groups.inc` and `sound/music_player_table.inc` are regenerated.

`audio_rules.mk` provides the Make rules that convert AIFF, MIDI and other audio assets. Ensure tools built by `make tools` or `./build_tools.sh` are available before running `make`.
