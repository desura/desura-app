include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/code
  ${CMAKE_CURRENT_SOURCE_DIR}/RES
)

configure_file(RES/service_exe.rc ${RC_DIR}/service_exe.rc @ONLY)
file(GLOB
  Sources code/*.cpp
  ${RC_DIR}/service_exe.rc
  ${COMMON_INCLUDE_DIRS}/Tracer.cpp
)

add_executable(desura_service ${Sources} ${COMMON_SOURCE})
target_link_libraries(desura_service
  mdump_s
  util_s
  wintrust
)

add_definitions(
  -DTRACER_SHARED_MEM_NAME=L"DESURA_SERVER_TRACER_OUTPUT"
)

if(WIN32)
  SetStaticRuntime(desura_service)
endif()

install_executable(desura_service)

add_dependencies(desura_service servicecore)
