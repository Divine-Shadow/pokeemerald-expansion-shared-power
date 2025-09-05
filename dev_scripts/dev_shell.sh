#!/usr/bin/env bash
set -euo pipefail

container="${1:-pokeemerald-dev}"
exec docker exec -it "$container" bash

