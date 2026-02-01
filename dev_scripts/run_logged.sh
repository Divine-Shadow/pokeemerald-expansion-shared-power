#!/usr/bin/env bash
set -euo pipefail

label="${LOG_LABEL:-run}"
timestamp="$(date +%Y%m%d_%H%M%S)"
log_dir="build_artifacts/logs"
log_file="${log_dir}/${timestamp}_${label}.log"

mkdir -p "${log_dir}"

{
  echo "[run_logged] $(date -Iseconds)"
  echo "[cwd] $(pwd)"
  echo "[cmd] $*"
  echo ""
} >> "${log_file}"

# Stream to console and log file unless LOG_SILENT=1.
if [[ "${LOG_SILENT:-0}" == "1" ]]; then
  "$@" >> "${log_file}" 2>&1
else
  "$@" 2>&1 | tee -a "${log_file}"
fi

status=${PIPESTATUS[0]}
echo "" >> "${log_file}"
echo "[exit] ${status}" >> "${log_file}"

exit "${status}"
