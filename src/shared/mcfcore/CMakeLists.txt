include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/code
  ${BZIP2_INCLUDE_DIR}
  ${COURGETTE_INCLUDE_DIRS}
)

file(GLOB Sources code/*.cpp
                  code/mcf/*.cpp
                  code/thread/*.cpp
)

add_definitions(-DNEED_STRDUP -DNEED_MEMCCPY)

if(WIN32)
  if(NOT MINGW)
    set(PLATFORM_LIBRARIES dnsapi rpcrt4 wininet winmm ws2_32)
  else()
    set(PLATFORM_LIBRARIES ${WINDOWS_LIBRARIES} msi winhttp wintrust)
  endif()
#  add_linker_flags(/NOD:LIBCMT)
  use_unicode_here()
else()
  set(PLATFORM_LIBRARIES dl)
endif()

setup_precompiled_header(Common.h ${COMMON_INCLUDE_DIRS}/Common.cpp Sources)
add_library(mcfcore SHARED ${Sources})
target_link_libraries(mcfcore
  umcf
  util
  util_fs
  util_web
  threads
  ${BZIP2_LIBRARIES}
  ${CURL_LIBRARIES}
  ${SQLITE_LIBRARIES}
  ${SQLITE3X_LIBRARIES}
  ${TINYXML_LIBRARIES}
  ${PLATFORM_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
)

add_dependencies(mcfcore curl tinyxml2 bzip2 sqlite3x sqlite)
#if (NOT Boost_FOUND)
  add_dependencies(mcfcore boost)
#endif()

if(WIN32)
  SetSharedRuntime(mcfcore)
endif()

link_with_gtest(mcfcore)
install_library(mcfcore)

if(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Weffc++")
endif()
