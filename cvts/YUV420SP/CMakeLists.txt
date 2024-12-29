#生成动态库名称、类型、资源文件
add_library(com.cvt.YUV420SP SHARED YUV420SP.cpp interface.cpp)
target_link_libraries(com.cvt.YUV420SP utils)
