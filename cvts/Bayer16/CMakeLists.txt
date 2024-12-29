#生成动态库名称、类型、资源文件
add_library(com.cvt.Bayer16 SHARED Bayer16.cpp interface.cpp)
target_link_libraries(com.cvt.Bayer16 utils)