# Documentation Guidelines

## Markdown Style
- Use a single `#` heading at the top for the page title.
- Nest sections with `##`, `###`, etc. without skipping levels.
- Prefer relative links using the `.md` extension: [Install guide](INSTALL.md).
- Use fenced code blocks with a language tag (e.g., `sh`, `c`):

  ```sh
  mdbook build docs
  ```
- Inline code uses single backticks.

## Building Docs Locally
- The docs use [mdBook](https://rust-lang.github.io/mdBook/).
- Preview changes live:
  ```sh
  mdbook serve docs
  ```
  This hosts the site at `http://127.0.0.1:3000`.
- Create a static build:
  ```sh
  mdbook build docs
  ```
- Platform-specific setup guides live in `docs/local_mdbook/`.

## Maintenance Tips
- Cross-link related pages with relative Markdown links.
- When docs change, update relevant changelog sections (`UNRELEASED.md`, `docs/CHANGELOG.md`).
