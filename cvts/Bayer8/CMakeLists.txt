#生成动态库名称、类型、资源文件
add_definitions(-DBAYER_PROCESS_V1)
add_library(com.cvt.Bayer8 SHARED Bayer8.cpp interface.cpp)

# find_package(OpenMP)
# target_link_libraries(com.cvt.Bayer8 PUBLIC OpenMP::OpenMP_CXX)