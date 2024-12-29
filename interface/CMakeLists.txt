add_library(cvtinterface STATIC cvtinterface.cpp schedule.cpp)
TARGET_LINK_LIBRARIES(cvtinterface dl utils)