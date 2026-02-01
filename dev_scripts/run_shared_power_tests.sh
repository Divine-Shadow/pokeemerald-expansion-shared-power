#!/usr/bin/env bash
set -euo pipefail

label="${LOG_LABEL:-shared_power}"
log_silent="${LOG_SILENT:-1}"

LOG_LABEL="${label}" LOG_SILENT="${log_silent}" dev_scripts/run_logged.sh \
  docker run --rm -u 0 \
    -e HOST_UID="$(id -u)" -e HOST_GID="$(id -g)" \
    -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder \
    bash -lc "git config --global --add safe.directory /workspace; \
      apt-get update && apt-get install -y libelf1 && \
      make check TESTS=\"Shared Power\" NO_MULTIBOOT=1; \
      status=\$?; \
      chown -R \"$HOST_UID:$HOST_GID\" /workspace/build /workspace/build_artifacts /workspace/pokeemerald-test* 2>/dev/null || true; \
      exit \$status"
