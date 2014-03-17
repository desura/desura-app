include_directories(  
  ${CMAKE_CURRENT_SOURCE_DIR}/code
  ${Boost_INCLUDE_DIR}
  ${IPC_PIPE_INCLUDE_DIRS}
)

file(GLOB Sources code/util/gcBuff_test.cpp
                  code/util/MD5_test.cpp
                  code/util/util_misc.cpp
                  code/util_fs/util_fs_copyFile.cpp
                  code/util_fs/util_fs_copyFolder.cpp
                  code/util_fs/util_fs_getAllFiles.cpp
                  code/util_fs/util_fs_getAllFolders.cpp
                  code/util_string/util_string_sanitizeFilePath.cpp
				  code/util_string/UtilString.cpp
				  code/util/util_event.cpp
				  code/IPCTest.cpp
)


#code/util/UtilLinux_test.cpp

if(WIN32)
	file(GLOB WIN_Sources code/util/UtilWindowsTest.cpp
	)

endif()

add_library(unittest SHARED ${Sources} ${WIN_Sources})

target_link_libraries(unittest
  util
  util_fs
  ipc_pipe
)

link_with_gtest(unittest)
install_library(unittest "Shared")
