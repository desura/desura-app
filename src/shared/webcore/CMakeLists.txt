include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/code
  ${BZIP2_INCLUDE_DIR}
  ${SQLITE3X_INCLUDE_DIRS}
)

file(GLOB Sources
  code/ImageCache.cpp
  code/ImageCache.h
  code/Log.cpp
  code/Log.h
  code/WebCore.cpp
  code/WebCore_External.cpp
  code/WebCore.h
  code/WebCore_Internal.cpp
  code/WebCoreMain.cpp
)

if(WIN32)
  if(NOT MINGW)
    set(WINDOWS_LIBRARIES ${WINDOWS_LIBRARIES} crypt32 dnsapi rpcrt4 wininet winmm ws2_32)
  else()
    set(WINDOWS_LIBRARIES ${WINDOWS_LIBRARIES} msi wintrust winhttp)
  endif()
endif()

setup_precompiled_header(Common.h ${COMMON_INCLUDE_DIRS}/Common.cpp Sources)
add_library(webcore SHARED ${Sources})
target_link_libraries(webcore
  threads
  util
  util_fs
  util_web
  ${BZIP2_LIBRARIES}
  ${CURL_LIBRARIES}
  ${TINYXML_LIBRARIES}
  ${SQLITE3X_LIBRARIES}
  ${WINDOWS_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
)

add_dependencies(webcore curl tinyxml2 bzip2 sqlite sqlite3x)

if(WIN32)
  SetSharedRuntime(webcore)
endif()

install_library(webcore)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Weffc++")
