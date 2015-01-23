include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${Boost_INCLUDE_DIR}
)

file(GLOB Sources code/UtilFs.cpp)

if(WIN32)
  file(GLOB PlattFormSources code/UtilFs_win.cpp)
  add_linker_flags(/NOD:MSVCRTD)
else()
  file(GLOB PlattFormSources code/UtilFs_nix.cpp)
endif()

add_library(util_fs STATIC ${Sources} ${PlattFormSources})

target_link_libraries(util_fs
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
)

add_dependencies(util_fs boost)

if(WIN32)
  SetSharedRuntime(util_fs)
endif()

SET_PROPERTY(TARGET util_fs                PROPERTY FOLDER "Static")

if(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Weffc++")
endif()
