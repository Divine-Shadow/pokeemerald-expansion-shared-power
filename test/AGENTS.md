# Test Directory Guidelines

- Tests live under `test/` and mirror the engine's module structure.
  - Example: files in `test/battle/*` correspond to battle engine modules.
- Name test source files with the pattern `*_test.c`.
- Define test cases using the `TEST("Suite/Case")` macro from the in-repo test framework.
- Run the full test suite with `make check`.
  - Keep all test cases deterministic and isolated.
