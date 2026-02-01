# Test/Build Log Capture

Use `dev_scripts/run_logged.sh` to capture output under `build_artifacts/logs/`, which is git-ignored. By default it streams to the console too; set `LOG_SILENT=1` to suppress console output.

Examples:
- `LOG_LABEL=make_check dev_scripts/run_logged.sh make check NO_MULTIBOOT=1`
- `LOG_LABEL=shared_power dev_scripts/run_logged.sh make check TESTS="Shared Power" NO_MULTIBOOT=1`
- `LOG_LABEL=shared_power dev_scripts/run_logged.sh docker run --rm -u 0 -e HOST_UID="$(id -u)" -e HOST_GID="$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder bash -lc "git config --global --add safe.directory /workspace; apt-get update && apt-get install -y libelf1 && make check TESTS=\\"Shared Power\\" NO_MULTIBOOT=1; status=$?; chown -R \\"$HOST_UID:$HOST_GID\\" /workspace/build /workspace/build_artifacts /workspace/pokeemerald-test* 2>/dev/null || true; exit $status"`
- `LOG_LABEL=shared_power LOG_SILENT=1 dev_scripts/run_logged.sh make check TESTS="Shared Power" NO_MULTIBOOT=1`
- `dev_scripts/run_shared_power_tests.sh` (wrapper for the docker command above, defaults to quiet logging)

This keeps routine compiler/test output out of git while preserving logs for review.
