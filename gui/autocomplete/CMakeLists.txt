#生成动态库名称、类型、资源文件
add_library(autocomplete STATIC autointerface.cpp autocomplete_config.cpp autocomplete_format.cpp autocomplete_utils.cpp)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/.nyuv.config DESTINATION ${EXECUTABLE_OUTPUT_PATH})