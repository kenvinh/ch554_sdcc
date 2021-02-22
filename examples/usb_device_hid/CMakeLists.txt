cmake_minimum_required(VERSION 3.12)

######################################################
## USER part start
######################################################

# Set project name
set(ProjName usb_device_hid)
# [Set executable name, default ProjName]
set(ExecName ${ProjName})

# Set include dir for compilation
set(IncludeDir
    "../../include"
)

# Set compile source list
set(SourceList 
    main.c
    # Key.c
    # USB.c
    "../../toolchain/sdcc/4.0.0/lib/src/rand.c"
    "../../include/debug.c"
)

# Adjust the XRAM location and size to leave space for the USB DMA buffers
# Buffer layout in XRAM:
# 0x0000 Ep0Buffer[8]
# 0x0040 Ep1Buffer[8]
# 0x0080 EP2Buffer[2*64]
#
# This takes a total of 256bytes, so there are 768 bytes left.
set(XRAM_SIZE, 0x0300)
set(XRAM_LOC, 0x0100)

######################################################
## USER part end
######################################################

include("../../toolchain/common-ch55-options.cmake")

# Project name
project(${ProjName} LANGUAGES C)
include_directories(${IncludeDir})
add_executable(${ProjName} ${SourceList})

include("../../toolchain/common-ch55-postbuild.cmake")