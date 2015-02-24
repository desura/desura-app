include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
)

file(GLOB Sources code/*.cpp)

add_library(threads STATIC ${Sources})

if(WIN32)
  SetSharedRuntime(threads)
endif()

SET_PROPERTY(TARGET threads PROPERTY FOLDER "Static")

if (WIN32)
	add_library(threads_s STATIC ${Sources})

	SET_PROPERTY(TARGET threads_s PROPERTY FOLDER "Static")
	SetStaticRuntime(threads_s)
endif()

if(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Weffc++")
endif()
