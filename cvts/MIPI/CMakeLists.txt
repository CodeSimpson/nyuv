#生成动态库名称、类型、资源文件
add_library(com.cvt.MIPI SHARED MIPI.cpp interface.cpp)
target_link_libraries(com.cvt.MIPI utils)