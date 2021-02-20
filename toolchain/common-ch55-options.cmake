# Common variables
set(FREQ_SYS 24000000)
set(XRAM_SIZE 0x0400)
set(XRAM_LOC 0x0000)
set(CODE_SIZE 0x3800)

set(CFLAGS )
set(CFLAGS ${CFLAGS} -V -mmcs51 --model-small)
set(CFLAGS ${CFLAGS} --xram-size ${XRAM_SIZE} --xram-loc ${XRAM_LOC} --code-size ${CODE_SIZE})

# For SDCC, linker option same as compile option
set(LDFLAGS ${CFLAGS})

# Common compile options
add_compile_options(${CFLAGS})

# Common linker options
add_link_options(${LDFLAGS})

# Common macro definition
add_compile_definitions(FREQ_SYS=${FREQ_SYS})