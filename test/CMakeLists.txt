#test interface
ADD_EXECUTABLE(testintreface testintreface.cpp)
target_link_libraries(testintreface cvtinterface)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${EXECUTABLE_OUTPUT_PATH})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/testinterface.sh DESTINATION ${EXECUTABLE_OUTPUT_PATH})