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
  set(GMOCK_LIBRARIES "${source_dir}/${CMAKE_BUILD_TYPE}/gmock.lib")
  set(GMOCK_INSTALL_LIBS "${source_dir}/${CMAKE_BUILD_TYPE}/gmock.dll")
else()
  set(GMOCK_LIBRARIES "${source_dir}/libgmock.so")
  set(GMOCK_INSTALL_LIBS "${source_dir}/libgmock.so")
endif()

install_external_library(gmock ${GMOCK_INSTALL_LIBS})
