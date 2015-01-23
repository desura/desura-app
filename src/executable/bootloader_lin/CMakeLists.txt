include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/code
  ${TINYXML_INCLUDEDIR}
  ${UTIL_BOOTLOADER_INCLUDE_DIRS}
)

file(GLOB Sources code/*.cpp )

add_executable(desura ${Sources})
target_link_libraries(desura
  dl
  mdump
  rt
  sqlite3x
  util
  ${TINYXML_LIBRARIES}
  ${BZIP2_LIBRARIES}
  ${CURL_LIBRARIES}
  ${BREAKPAD_EXCEPTION_HANDLER_LIBRARIES}
  ${SQLITE_LIBRARIES}
  ${wxWidgets_LIBRARIES}
  ${LIBNOTIFY_LIBRARIES}
  umcf
  Xpm
  X11
  util_web
  pthread
)

install_executable(desura)
add_gtest_test(desura --unittests)

if(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Weffc++")
endif()
