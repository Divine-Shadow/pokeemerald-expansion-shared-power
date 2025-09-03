# Graphics Asset Guidelines

- Source art lives here as indexed PNGs (usually 4bpp/16 colors). Store palettes separately in matching `.pal` files.
- Edit only the `.png` and `.pal` sources. Generated files (`.4bpp`, `.gbapal`, `.lz`, etc.) are build artifacts and must not be committed.
- After modifying graphics, run `make` to regenerate the converted assets. The build will invoke `tools/gfx` and produce raw and compressed files automatically.
- Use lowercase `snake_case` for file and directory names. Palette files share the base name of their image (e.g., `sprite.png` and `sprite.pal`). Variants may use suffixes like `_shiny` or `_overworld`.
- Converted `.4bpp`/`.gbapal` files are written next to the sources, and the compressed outputs end up in the `build/` tree during ROM creation.
