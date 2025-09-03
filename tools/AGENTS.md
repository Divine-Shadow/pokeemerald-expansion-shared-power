# Tools Contribution Guidelines

## Build & Style
- Build new tools through the existing makefile: run `make tools` and follow patterns in `make_tools.mk`.
- Tool code must follow the repository's C style: tabs for indentation, `snake_case` for functions and variables, and `UPPER_SNAKE_CASE` for macros and constants.

## Program Structure
- Each tool should define an `int main(int argc, char **argv)` entry point.
- Parse command line options using standard approaches (e.g. `getopt`), supporting `--help` for usage details.
- Keep argument handling consistent with other tools; prefer long-form flags and clear error messages for invalid inputs.

## Cross-Platform Considerations
- Tools must compile and run on Linux, macOS, and Windows (via MinGW or compatible environments).
- Avoid platform-specific APIs when possible; use standard C library functions or existing portability helpers.
- For file I/O, handle paths and line endings portably (use binary mode where appropriate).

## Documentation Requirements
- Provide a brief `README.md` in each tool's directory describing its purpose, usage, and examples.
- Whenever a new tool is added, update `UNRELEASED.md` with a summary entry referencing the commit.
- Use `make tools` to verify successful compilation before submitting changes.
