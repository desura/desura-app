cmake_minimum_required(VERSION 2.8)
project(wxWidget_3_0_test)

set(CMAKE_CXX_FLAGS "-std=c++11")
find_package(wxWidgets REQUIRED)

include_directories(
  ${wxWidgets_INCLUDE_DIRS}
)
file(GLOB Sources *.cpp)
# wxWidgets config
add_definitions(-D__WXGTK__)
add_executable(wxWidget_3_0_test ${Sources})
target_link_libraries(
  wxWidget_3_0_test
  ${wxWidgets_LIBRARIES}	
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Weffc++")
