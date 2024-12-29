#生成动态库名称、类型、资源文件
add_library(com.cvt.RGB16 SHARED RGB16.cpp interface.cpp)
target_link_libraries(com.cvt.RGB16 utils)
# set(THREADS_PREFER_PTHREAD_FLAG ON)
# find_package(Threads REQUIRED)
# target_link_libraries(com.cvt.RGB16 PUBLIC Threads::Threads)