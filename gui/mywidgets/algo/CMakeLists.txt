add_library(algo STATIC npea.cpp stats.cpp alight.cpp)
target_link_libraries(algo Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network ${QT_QTNETWORK_LIBRARY} utils)