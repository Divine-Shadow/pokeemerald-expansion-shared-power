#!/usr/bin/env bash
set -euo pipefail

# Starts the persistent dev container with bind-mount.
# Requires Docker Compose V2 (`docker compose`). Falls back to `docker-compose` if needed.

if command -v docker >/dev/null && docker compose version >/dev/null 2>&1; then
  docker compose up -d --build dev
else
  docker-compose up -d --build dev
fi

echo "Dev container is up (pokeemerald-dev)."
echo "- Open a shell:   docker exec -it pokeemerald-dev bash"
echo "- Build:          docker exec pokeemerald-dev make -j$(nproc) NO_MULTIBOOT=1"
echo "- Run tests:      docker exec pokeemerald-dev make check"
echo "- Extract:        bash dev_scripts/extract_artifacts.sh"
