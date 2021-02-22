# Common variables

if(NOT DEFINED FREQ_SYS)
  message(STATUS "Not defined FREQ_SYS")
  set(FREQ_SYS 24000000)
endif()

if(NOT DEFINED XRAM_SIZE)
  message(STATUS "Not defined XRAM_SIZE")
  set(XRAM_SIZE 0x0400)
endif()

if(NOT DEFINED XRAM_LOC)
  message(STATUS "Not defined XRAM_LOC")
  set(XRAM_LOC 0x0000)
endif()

if(NOT DEFINED CODE_SIZE)
  message(STATUS "Not defined CODE_SIZE")
  set(CODE_SIZE 0x3800)
endif()

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