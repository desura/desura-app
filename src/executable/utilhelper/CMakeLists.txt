include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/RES
  ${UTIL_BOOTLOADER_INCLUDE_DIRS}
)

configure_file(RES/repair.rc ${RC_DIR}/repair.rc @ONLY)
file(GLOB Sources
  code/*.cpp
  ${RC_DIR}/repair.rc
)

add_definitions(
  -Dgamecore_LOG
  -DDONTUSE_PCH
  -DDESURA_EXE
  -DDESURA_CLIENT
)

#Desura Uninstaller has this projects name hard coded. Do not change
add_executable(utility WIN32 ${Sources})
target_link_libraries(utility
  mdump_s
  util_s
  util_bootloader_s
  Comctl32
)

install_executable(utility)
SetStaticRuntime(utility)
