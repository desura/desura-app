if(WIN32 AND NOT MINGW)
  ExternalProject_Add(
    wxWidgets
    URL ${WXWIDGET_URL}
    URL_MD5 ${WXWIDGET_MD5}
    UPDATE_COMMAND ""
    PATCH_COMMAND ${PATCH_SCRIPT_PATH} ${CMAKE_SOURCE_DIR}/cmake/patches/wxWidgets.patch
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )

  if(DEBUG) 
    ExternalProject_Add_Step(
      wxWidgets
      custom_build
      DEPENDEES configure
      DEPENDERS build
      COMMAND ${NMAKE_EXECUTABLE} /nologo -f makefile.vc BUILD=debug MONOLITHIC=1 VENDOR=desura DEBUG_INFO=1 SHARED=1 RUNTIME_LIBS=dynamic
      WORKING_DIRECTORY <SOURCE_DIR>/build/msw
    )
  else()
    ExternalProject_Add_Step(
      wxWidgets
      custom_build
      DEPENDEES configure
      DEPENDERS build
      COMMAND ${NMAKE_EXECUTABLE} /nologo -f makefile.vc BUILD=release MONOLITHIC=1 VENDOR=desura DEBUG_INFO=1 SHARED=1 RUNTIME_LIBS=dynamic
      WORKING_DIRECTORY <SOURCE_DIR>/build/msw
    )
  endif()
  
  ExternalProject_Get_Property(
    wxWidgets
    source_dir
  )
  set(wxWidgets_INSTALL_DIR ${source_dir})
  set(wxWidgets_BIN_DIR ${wxWidgets_INSTALL_DIR}/bin)
  set(wxWidgets_LIBRARY_DIRS ${wxWidgets_INSTALL_DIR}/lib/vc_dll)
  
  if(DEBUG)
    set(wxWidgets_INCLUDE_DIRS ${wxWidgets_INSTALL_DIR}/include ${wxWidgets_INSTALL_DIR}/include/msvc)
    set(wxWidgets_LIBRARIES ${wxWidgets_LIBRARY_DIRS}/wxmsw30ud.lib)
    install_external_library(wxWidgets "${wxWidgets_LIBRARY_DIRS}/wxmsw30ud_vc_desura.dll")
  else()
    set(wxWidgets_INCLUDE_DIRS ${wxWidgets_INSTALL_DIR}/include ${wxWidgets_INSTALL_DIR}/include/msvc)
    set(wxWidgets_LIBRARIES ${wxWidgets_LIBRARY_DIRS}/wxmsw30u.lib)
    install_external_library(wxWidgets "${wxWidgets_LIBRARY_DIRS}/wxmsw30u_vc_desura.dll")
  endif()
  
else()
  if(MINGW)
    set(WX_SETUP_INCLUDE_SUB "msw-unicode-3.0-desura")
    set(WX_SETUP_INCLUDE_SUB_DEBUG "msw-unicode-debug-3.0-desura")
    set(WX_LIB_NAME "libwx_mswu_desura-3.0.dll.a")
    set(WX_LIB_NAME_DEBUG "libwx_mswu_desura-3.0.dll.a")
  else()
    set(WX_SETUP_INCLUDE_SUB "gtk2-unicode-3.0-desura")
    set(WX_SETUP_INCLUDE_SUB_DEBUG ${WX_SETUP_INCLUDE_SUB})
    set(WX_LIB_NAME "libwx_gtk2u_desura-3.0.so")
    set(WX_LIB_NAME_DEBUG ${WX_LIB_NAME})
  endif()

  set(wxWidgets_INSTALL_DIR ${CMAKE_EXTERNAL_BINARY_DIR}/wxWidgets)
  
  if(MINGW)
    set(WX_PATCH_COMMAND "${PATCH_SCRIPT_PATH}" "${CMAKE_SOURCE_DIR}/cmake/patches/wxWidgets.patch")
  endif()

  ExternalProject_Add(
    wxWidgets
    URL ${WXWIDGET_URL}
    URL_MD5 ${WXWIDGET_MD5}
    UPDATE_COMMAND ""
    ${WX_PATCH_COMMAND}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ./configure
        --enable-shared --enable-unicode ${CONFIGURE_DEBUG}
        --enable-monolithic --with-flavour=desura --enable-threads --with-opengl=no --disable-palette2
		--disable-joystick --disable-mediactrl --prefix=${wxWidgets_INSTALL_DIR} --enable-permissive
  )
  
  set(wxWidgets_LIBRARY_DIRS ${wxWidgets_INSTALL_DIR}/lib)
  if(DEBUG_EXTERNAL)
    set(wxWidgets_INCLUDE_DIRS  ${wxWidgets_INSTALL_DIR}/include/wx-3.0-desura ${wxWidgets_LIBRARY_DIRS}/wx/include/${WX_SETUP_INCLUDE_SUB_DEBUG})
    set(wxWidgets_LIBRARIES "${wxWidgets_LIBRARY_DIRS}/${WX_LIB_NAME_DEBUG}")
    install_external_library(wxWidgets ${wxWidgets_LIBRARY_DIRS}/${WX_LIB_NAME})
  else()
    set(wxWidgets_INCLUDE_DIRS  ${wxWidgets_INSTALL_DIR}/include/wx-3.0-desura ${wxWidgets_LIBRARY_DIRS}/wx/include/${WX_SETUP_INCLUDE_SUB})
    set(wxWidgets_LIBRARIES "${wxWidgets_LIBRARY_DIRS}/${WX_LIB_NAME}")
    install_external_library(wxWidgets ${wxWidgets_LIBRARY_DIRS}/${WX_LIB_NAME})
  endif()
  set(wxWidgets_BIN_DIR ${wxWidgets_INSTALL_DIR}/bin)
  set(wxWidgets_CONFIG_EXECUTABLE ${wxWidgets_BIN_DIR}/wx-config)
  set_property(TARGET wxWidgets PROPERTY FOLDER "ThirdParty")
endif()

