#生成动态库名称、类型、资源文件
add_library(mywidgets STATIC myFileTree.cpp myInfoBar.cpp myQImageviewer.cpp mainwindow.cpp update.cpp myPrefer.cpp)
target_link_libraries(mywidgets Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network ${QT_QTNETWORK_LIBRARY} algo utils cvtinterface autocomplete)