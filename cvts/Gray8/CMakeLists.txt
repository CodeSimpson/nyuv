#生成动态库名称、类型、资源文件
add_library(com.cvt.Gray8 SHARED Gray8.cpp interface.cpp)

# find_package(OpenMP)
# target_link_libraries(com.cvt.Gray8 PUBLIC OpenMP::OpenMP_CXX)