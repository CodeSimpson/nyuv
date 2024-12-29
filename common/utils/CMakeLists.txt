#生成动态库名称、类型、资源文件
add_library(utils SHARED ./tinyxml2/tinyxml2.cpp table.cpp extern.cpp threadpool.cpp)