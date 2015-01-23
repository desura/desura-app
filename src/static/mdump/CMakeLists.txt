include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${BREAKPAD_EXCEPTION_HANDLER_INCLUDE_DIR}
  ${BREAKPAD_EXCEPTION_HANDLER_INCLUDE_DIR_S}
)

if(WIN32)
  file(GLOB Sources code/MiniDumpGenerator_win.cpp)
  set(WINDOWS_LIBS Psapi)
else()
  file(GLOB Sources code/MiniDumpGenerator_lin.cpp)
endif()

add_library(mdump STATIC ${Sources})
target_link_libraries(mdump
    ${BREAKPAD_EXCEPTION_HANDLER_LIBRARIES}
	${WINDOWS_LIBS}
)

add_dependencies(mdump breakpad)

if(WIN32)
  SetSharedRuntime(mdump)
endif()

SET_PROPERTY(TARGET mdump                PROPERTY FOLDER "Static")

if (WIN32)
	add_library(mdump_s STATIC ${Sources})
	target_link_libraries(mdump_s
		${BREAKPAD_EXCEPTION_HANDLER_LIBRARIES_S}
		${WINDOWS_LIBS}
	)

	SET_PROPERTY(TARGET mdump_s                PROPERTY FOLDER "Static")
	add_dependencies(mdump_s breakpad_s)
	SetStaticRuntime(mdump_s)
endif()

if(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Weffc++")
endif()
