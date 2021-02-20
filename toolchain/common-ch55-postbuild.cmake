# Post build
add_custom_command(
    TARGET ${ExecName}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "[Postbuild] Converting ihx to hex file"
    COMMAND packihx ${PROJECT_BINARY_DIR}/${ExecName}.ihx > ${PROJECT_BINARY_DIR}/${ExecName}.hex
    COMMAND ${CMAKE_COMMAND} -E echo "[Postbuild] Converting hex file to binary file"
    COMMAND sdobjcopy -I ihex -O binary ${PROJECT_BINARY_DIR}/${ExecName}.ihx ${PROJECT_BINARY_DIR}/${ExecName}.bin
    VERBATIM
)