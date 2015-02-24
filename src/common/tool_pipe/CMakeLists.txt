include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${IPC_PIPE_INCLUDE_DIRS}
)

file(GLOB Sources *.cpp)

set(TOOL_PIPE_SPOURCES ${Sources} PARENT_SCOPE)

if(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Weffc++")
endif()
