if(MSVC11)
  set(ADD_CXX_FLAGS "/D_VARIADIC_MAX=10")
endif()

ExternalProject_Add(
  gmock
  URL "${GMOCK_URL}"
  URL_MD5 ${GMOCK_MD5}
  CMAKE_ARGS -DBUILD_SHARED_LIBS=ON -DCMAKE_CXX_FLAGS=${ADD_CXX_FLAGS} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -Dgmock_force_shared_crt:BOOL=ON -Dgtest_force_shared_crt=ON
  BUILD_IN_SOURCE 1
  INSTALL_COMMAND ""
)

ExternalProject_Get_Property(
  gmock
  source_dir
)

set(GMOCK_INCLUDE_DIRS "${source_dir}/include")
set(GTEST_INCLUDE_DIRS "${source_dir}/gtest/include")

if(WIN32)
  if(CMAKE_BUILD_TYPE EQUAL DEBUG)
    set(GMOCK_DIR "Debug")
  else(CMAKE_BUILD_TYPE EQUAL "RelWithDebInfo")
    set(GMOCK_DIR "RelWithDebInfo")
  else()
    set(GMOCK_DIR ".")
  endif()
  
  set(GMOCK_LIBRARIES "${source_dir}/${GMOCK_DIR}/gmock.lib")
  # currently we don't need them, but we should keep it here
  # set(GMOCK_MAIN_LIBRARIES "${source_dir}/${GTEST_DIR}/gtest_main.lib")
  set(GMOCK_INSTALL_LIBS "${source_dir}/${GMOCK_DIR}/gmock.dll")
else()
  set(GMOCK_LIBRARIES "${source_dir}/libgmock.so")
  # currently we don't need them, but we should keep it here
  # set(GMOCK_MAIN_LIBRARIES "${source_dir}/libgtest_main.so")
  set(GMOCK_INSTALL_LIBS "${source_dir}/libgmock.so")
endif()

install_external_library(gmock ${GMOCK_INSTALL_LIBS})
