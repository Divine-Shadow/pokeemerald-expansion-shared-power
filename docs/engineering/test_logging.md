# Test/Build Log Capture

Use `dev_scripts/run_logged.sh` to stream output to the console and a log file under `build_artifacts/logs/`, which is git-ignored.

Examples:
- `LOG_LABEL=make_check dev_scripts/run_logged.sh make check NO_MULTIBOOT=1`
- `LOG_LABEL=shared_power dev_scripts/run_logged.sh make check TESTS="Shared Power" NO_MULTIBOOT=1`

This keeps routine compiler/test output out of git while preserving logs for review.
