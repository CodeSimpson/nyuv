cmake_minimum_required(VERSION 3.5)
project(nyuv)

# set可以定义普通变量、缓存变量和环境变量
set(_VERSION_MAJOR 0)
set(_VERSION_MINOR 9)
set(_VERSION_PATCH 8)
set(CPACK_PACKAGE_VERSION_MAJOR "${_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${_VERSION_PATCH}")

# 定义一个用户可配置的布尔开关选项，使用方式为 cmake -DINSTALL=ON ..
option(INSTALL "BUILD INSTALL" OFF)
if (${INSTALL})
    add_definitions(-DINSTALL)          # 向编译器传递预处理宏定义INSTALL
    set(LIB_DIR /usr/lib/nyuv/)
    link_directories(${LIB_DIR})        # 向链接器添加算法库文件的搜索路径
else()
    set(LIB_DIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
    link_directories(${LIB_DIR})
endif()

option(DEBUG "BUILD DEBUG" OFF)
if (${DEBUG})
    SET(CMAKE_BUILD_TYPE "Debug")
    SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")     # 设置编译选项，-O0：关闭优化，-Wall：启用所有警告，-g2 -ggdb：生成GDB专用调试信息，级别2
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/debug/lib)
    SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/debug/bin)         # 旧版可执行那个文件输出路径，RUNTIME_OUTPUT_DIRECTORY 未定义时使用
else()
    SET(CMAKE_BUILD_TYPE "Release")
    SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/release/lib)
    SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/release/bin)
endif()
message("CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")           # 追加两条编译选项，-std=c++11：强制编译器使用c++11编译代码，-pthread：启用POSIX线程，确保多线程代码能正确编译和链接

# 顶层CMakeLists.txt设置全局包含路径，所有子目录都可以访问以下目录中的头文件， 但这不符合现代CMake强调的以Target为中心的依赖管理思想，
# 即每个目标（库或可执行文件）应显式声明自己的依赖项（如头文件路径、链接库等），非最佳实践。
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/common/base)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/common/utils)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/common/utils/tinyxml2)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/interface)

ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/cvts)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/interface)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/common)

option(GUI "BUILD GUI" ON)
if (${GUI})
    ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/gui)
endif()
message("BUILD_GUI=${GUI}")

option(TEST "BUILD TEST" OFF)
if (${TEST})
    #test
    ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/test)
endif()
message("BUILD_TEST=${TEST}")

################### 配置CPack工具，生成适用于Debian/Ubuntu系统的deb安装包 ###################
set(CPACK_GENERATOR "DEB")              
set(CPACK_PACKAGE_NAME "nyuv")                  # 设置通用包名，作为默认名
set(CPACK_PACKAGE_VERSION "0.9.8")
set(CPACK_DEBIAN_PACKAGE_NAME "nyuv")           # 显示指定Debian系统的安装包名
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")  # 指定包的目标架构为amd64，即64位x86系统，最终执行cpack命令，会生成一个名为nyuv_0.9.8_amd64.deb的安装包

# 进一步配置生成的Debian包
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5core5a, libqt5gui5")    # 运行时依赖，需要系统环境中安装这些依赖
set(CPACK_DEBIAN_PACKAGE_SECTION "app")         # 将包分类为app，应用程序类，表示这是一个用户级应用
set(CPACK_DEBIAN_PACKAGE_PRIORITY "Optional")   # 标记包的优先级为Optianal，表示该包对系统运行不是必须的，其它优先级为：Required（系统必需）、Important（关键功能）、Standard（默认安装）。
set(CPACK_PACKAGE_DESCRIPTION "-")              # 定义包的简单描述

set(CPACK_PACKAGE_CONTACT "caibingcheng@xiaomi.com")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "members of caibingcheng@xiaomi.com")

# set(LIBS_DIR ${CMAKE_SOURCE_DIR}/lib)
# set(DOCS_DIR ${CMAKE_SOURCE_DIR}/doc)
# set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
#  "${LIBS_DIR}/postinst;${LIBS_DIR}/postrm;${LIBS_DIR}/prerm") # 配置生成的Debian包在安装或卸载时执行的自定义脚本

################### 自定义安装规则 ###################
file(COPY ${PROJECT_SOURCE_DIR}/cvts/liblist.xml DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
# 设置安装目录的默认权限
set(CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
    OWNER_READ
    OWNER_WRITE
    OWNER_EXECUTE
    GROUP_READ
)
set(CMAKE_INSTALL_PREFIX /usr)                          # 将默认安装根目录设为 /usr（而非默认的 /usr/local）
# GLOB用于匹配规则在指定的目录内找到所需的文件，并将这些文件的路径存储在变量中
file(GLOB GLOB_BIN ${EXECUTABLE_OUTPUT_PATH}/nyuv)      # 收集构建生成的可执行文件nyuv，路径由EXECUTABLE_OUTPUT_PATH指定
file(GLOB GLOB_LIB ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/*) # 收集构建生成的库文件，路径由 CMAKE_LIBRARY_OUTPUT_DIRECTORY 指定）
INSTALL(FILES ${GLOB_BIN} DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)    # 安装可执行文件到目录 /usr/bin
INSTALL(FILES ${GLOB_LIB} DESTINATION lib/nyuv)                         # 安装库文件到 /usr/lib/nyuv
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/nyuv.desktop DESTINATION share/applications/) # 安装桌面启动文件
INSTALL(FILES ${EXECUTABLE_OUTPUT_PATH}/.nyuv.config DESTINATION etc/nyuv/ PERMISSIONS OWNER_WRITE OWNER_READ) # 安装配置文件，使用绝对路径 /etc/nyuv/ 会报错

include(CPack)