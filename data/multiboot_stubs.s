.section .rodata
.align 2

@ Stub symbols for multiboot payloads when NO_MULTIBOOT=1
@ These provide zero-length ranges so code can link and feature paths can no-op.

gMultiBootProgram_EReader_Start::
gMultiBootProgram_EReader_End::

gMultiBootProgram_BerryGlitchFix_Start::
gMultiBootProgram_BerryGlitchFix_End::

gMultiBootProgram_PokemonColosseum_Start::
gMultiBootProgram_PokemonColosseum_End::

