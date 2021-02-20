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
    Key.c
    USB.c
    "../../include/debug.c"
)

######################################################
## USER part end
######################################################

include("../../toolchain/common-ch55-options.cmake")

# Project name
project(${ProjName} LANGUAGES C)
include_directories(${IncludeDir})
add_executable(${ProjName} ${SourceList})

include("../../toolchain/common-ch55-postbuild.cmake")