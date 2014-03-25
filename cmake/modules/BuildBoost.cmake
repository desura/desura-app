if(MINGW)
  set(CONFIGURE_COMMAND "./bootstrap.sh")
  set(BJAM_BINARY "./bjam.exe")
  set(BOOST_EXTRA_BUILD_OPTS "--with-toolset=mingw")
  set(BOOST_PATCH_COMMAND "")
else()
  set(CONFIGURE_COMMAND "bootstrap.bat")
  set(BJAM_BINARY "b2.exe")
  set(BOOST_EXTRA_BUILD_OPTS "")
endif()

set(BOOST_BJAM_LIBS_STATIC --with-filesystem)
set(BOOST_BJAM_LIBS ${BOOST_BJAM_LIBS_STATIC})

if (WIN32 AND NOT MINGW)
  if (MSVC10)
    set(TOOLSET_MSVC_VER --toolset=msvc-10.0)
    set(BOOST_EXTRA_BUILD_OPTS vc10)
  elseif (MSVC11)
    set(TOOLSET_MSVC_VER --toolset=msvc-11.0)
    set(BOOST_EXTRA_BUILD_OPTS vc11)
  elseif (MSVC12)
    set(TOOLSET_MSVC_VER --toolset=msvc-12.0)
    set(BOOST_EXTRA_BUILD_OPTS vc12)
  endif()
endif()

if(DEBUG) 
  ExternalProject_Add(
    boost
    URL "${BOOST_URL}"
    URL_MD5 ${BOOST_MD5}
    UPDATE_COMMAND ""
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ${CONFIGURE_COMMAND} ${BOOST_EXTRA_BUILD_OPTS}
    BUILD_COMMAND ${BJAM_BINARY} ${BOOST_BJAM_LIBS} --layout=tagged variant=debug link=static
                    threading=multi runtime-link=shared ${TOOLSET_MSVC_VER}
    INSTALL_COMMAND ""
  )
  set(BOOST_LIB_ADD_STRING "mt-gd")
else()
  ExternalProject_Add(
    boost
    URL "${BOOST_URL}"
    URL_MD5 ${BOOST_MD5}
    UPDATE_COMMAND ""
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ${CONFIGURE_COMMAND} ${BOOST_EXTRA_BUILD_OPTS}
    BUILD_COMMAND ${BJAM_BINARY} ${BOOST_BJAM_LIBS} --layout=tagged variant=release link=static
                    threading=multi runtime-link=shared ${TOOLSET_MSVC_VER}
    INSTALL_COMMAND ""
  )
  set(BOOST_LIB_ADD_STRING "mt")
endif()

ExternalProject_Get_Property(
  boost
  source_dir
)

set(Boost_DIR ${source_dir})
set(Boost_INCLUDE_DIR ${Boost_DIR})
set(Boost_LIBRARY_DIR ${Boost_DIR}/stage/lib)

if(MINGW)
  set(BOOST_SUFFIX a)
else()
  set(BOOST_SUFFIX lib)
endif()

set(Boost_FILESYSTEM_LIBRARY "${Boost_LIBRARY_DIR}/libboost_filesystem-${BOOST_LIB_ADD_STRING}.${BOOST_SUFFIX}")
set(Boost_SYSTEM_LIBRARY "${Boost_LIBRARY_DIR}/libboost_system-${BOOST_LIB_ADD_STRING}.${BOOST_SUFFIX}")

set_property(TARGET boost PROPERTY FOLDER "ThirdParty")

if (WIN32)
  if(DEBUG)
    ExternalProject_Add(
      boost_s
      UPDATE_COMMAND ""
      DOWNLOAD_COMMAND ""
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ${BJAM_BINARY} ${BOOST_BJAM_LIBS_STATIC} --layout=tagged
                      variant=debug link=static threading=multi runtime-link=static ${TOOLSET_MSVC_VER}
      INSTALL_COMMAND ""
      SOURCE_DIR "${source_dir}"
    )
    set(BOOST_LIB_ADD_STRING "mt-sgd")
  else()
    ExternalProject_Add(
      boost_s
      UPDATE_COMMAND ""
      DOWNLOAD_COMMAND ""
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ${BJAM_BINARY} ${BOOST_BJAM_LIBS_STATIC} --layout=tagged
                      variant=release link=static threading=multi runtime-link=static ${TOOLSET_MSVC_VER}
      INSTALL_COMMAND ""
      SOURCE_DIR "${source_dir}"
    )
    set(BOOST_LIB_ADD_STRING "mt-s")
  endif()
  
  add_dependencies(boost_s boost)

  set(Boost_DIR_S ${source_dir})
  set(Boost_LIBRARY_DIR_S ${Boost_DIR_S}/stage/lib)

  set_property(TARGET boost_s PROPERTY FOLDER "ThirdParty")
  
  set(Boost_FILESYSTEM_LIBRARY_S "${Boost_LIBRARY_DIR_S}/libboost_filesystem-${BOOST_LIB_ADD_STRING}.${BOOST_SUFFIX}")
  set(Boost_SYSTEM_LIBRARY_S "${Boost_LIBRARY_DIR_S}/libboost_system-${BOOST_LIB_ADD_STRING}.${BOOST_SUFFIX}")
endif()
