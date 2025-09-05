#!/usr/bin/env bash
set -euo pipefail

# Extracts build artifacts from a running dev container to host.
# If artifacts already exist on host (bind-mount), copies them locally.
#
# Usage:
#   dev_scripts/extract_artifacts.sh [container_name]
# Default container name: pokeemerald-dev

container="${1:-pokeemerald-dev}"
out_dir="build_artifacts"
mkdir -p "$out_dir"

have_host_artifacts=0
if [[ -f pokeemerald.gba ]]; then
  have_host_artifacts=1
fi

copy_from_host() {
  echo "Detected host artifacts (bind-mount). Copying..."
  cp -f pokeemerald.gba "$out_dir/" 2>/dev/null || true
  cp -f pokeemerald.map "$out_dir/" 2>/dev/null || true
  cp -f pokeemerald.sym "$out_dir/" 2>/dev/null || true
  # Package modern builds if present
  if compgen -G "build/modern*" >/dev/null; then
    tar czf "$out_dir/build_modern.tar.gz" build/modern* 2>/dev/null || true
  fi
  echo "Artifacts written to: $out_dir"
}

copy_from_container() {
  echo "Copying artifacts from container: $container"
  if docker ps -a --format '{{.Names}}' | grep -qx "$container"; then
    docker cp "$container:/workspace/pokeemerald.gba" "$out_dir/" 2>/dev/null || true
    docker cp "$container:/workspace/pokeemerald.map" "$out_dir/" 2>/dev/null || true
    docker cp "$container:/workspace/pokeemerald.sym" "$out_dir/" 2>/dev/null || true
    # Package modern builds
    # Create a temp tar inside the container to avoid copying many small files
    docker exec "$container" bash -lc "set -e; shopt -s nullglob; mods=(build/modern*); if ((${#mods[@]})); then tar czf /tmp/build_modern.tar.gz \"${mods[@]}\"; fi" 2>/dev/null || true
    if docker exec "$container" test -f /tmp/build_modern.tar.gz; then
      docker cp "$container:/tmp/build_modern.tar.gz" "$out_dir/build_modern.tar.gz"
      docker exec "$container" rm -f /tmp/build_modern.tar.gz >/dev/null 2>&1 || true
    fi
    echo "Artifacts written to: $out_dir"
  else
    echo "Container '$container' not found. Start it first or pass the correct name." >&2
    exit 1
  fi
}

if [[ $have_host_artifacts -eq 1 ]]; then
  copy_from_host
else
  copy_from_container
fi

