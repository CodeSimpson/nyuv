# set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)               # 启动自动元对象编译器MOC
set(CMAKE_AUTOUIC ON)               # 启动自动用户界面编译器UIC
set(CMAKE_AUTORCC ON)               # 启动自动RCC
set(CMAKE_INCLUDE_CURRENT_DIR ON)   # 将当前构建目录和源代码路径加入头文件搜索路径

find_package(Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)  # 查找并验证项目所需的Qt5库及其组件‌

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/autocomplete)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/mywidgets)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/mywidgets/algo)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/mywidgets)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/autocomplete)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/mywidgets/algo)

file(GLOB_RECURSE QRC_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc)   # 递归搜索当前目录下的.qrc文件，并保存它们的路径到 QRC_SOURCE_FILES 中
set(QRC_SOURCE_FILES icon.qrc)                                          # 将变量 QRC_SOURCE_FILES 的值覆盖为 icon.qrc
qt5_add_resources(QRC_FILES ${QRC_SOURCE_FILES})                        # 调用rcc工具生成c++代码，并将生成的文件路径保存到 QRC_FILES 

add_executable(nyuv main.cpp ${QRC_FILES})
target_link_libraries(nyuv Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network ${QT_QTNETWORK_LIBRARY} mywidgets utils cvtinterface autocomplete)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/statics/nyuv.png DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})