include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/code
)

file(GLOB Sources code/*.cpp)

if(WIN32)
  if(MINGW)
    set(PLATFORM_LIBRARIES winhttp wintrust msi)
  endif()
#  add_linker_flags(/NODEFAULTLIB:LIBCMT)
else()
	set(PLATFORM_LIBRARIES ${GTK2_LIBRARIES})
endif()

add_library(crashuploader SHARED ${Sources})

target_link_libraries(crashuploader
    util
    util_fs
    util_web
    ${CURL_LIBRARIES}
    ${TINYXML_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${PLATFORM_LIBRARIES}
)

add_dependencies(crashuploader curl tinyxml2)

if(WIN32)
  SetSharedRuntime(crashuploader)
endif()

install_library(crashuploader)