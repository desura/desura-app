include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/code
  ${Boost_INCLUDE_DIR}
)

file(GLOB Sources code/main.cpp)

add_definitions(-D_WINDOWS
  -DDONTUSE_PCH
  -DNO_UTIL_FS
)

add_executable(java_launcher ${Sources})

target_link_libraries(java_launcher
  util_s
  ${Boost_FILESYSTEM_LIBRARY_S}
)

SetStaticRuntime(java_launcher)
install_tool(java_launcher)
