if(UNIX)
  set(MY_BUILD_TOOL $(MAKE))
else()
  set(MY_BUILD_TOOL ${CMAKE_BUILD_TOOL})
endif()

if(BUILD_CEF OR BUILD_ONLY_CEF)
  if(WIN32)
    if(NOT SUBVERSION_FOUND)
      find_package(Subversion REQUIRED)
    endif()
    find_package(DirectX REQUIRED)
    if(NOT DirectX_D3DX9_FOUND)
      message(FATAL_ERROR "DirectX9 SDK not found")
    else()
      message("-- DirectX9 found: ${DirectX_D3DX9_LIBRARY} ${DirectX_D3DX9_INCLUDE_DIR}")
    endif()
  endif()

  set(DEPOT_TOOLS_INSTALL_DIR ${CMAKE_EXTERNAL_BINARY_DIR}/depot_tools)
  set(DEPOT_TOOLS_BIN_DIR ${DEPOT_TOOLS_INSTALL_DIR}/src/depot_tools)

  ExternalProject_Add(
    depot_tools
    URL ${DEPOT_TOOLS_URL}
    URL_MD5 ${DEPOT_TOOLS_MD5}
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    PREFIX ${DEPOT_TOOLS_INSTALL_DIR}
  )

  if(PYTHON_VERSION_MAJOR EQUAL 3)
    ExternalProject_Add_Step(
    depot_tools
    fix_python_scripts
    COMMAND ${CMAKE_SCRIPT_PATH}/fix_python_depot_tools.sh
    DEPENDEES build
    WORKING_DIRECTORY ${DEPOT_TOOLS_BIN_DIR}
    )
  endif()
  
  if(NOT WIN32 OR MINGW)
    set(chromium_INSTALL_COMMAND "${CMAKE_SCRIPT_PATH}/fix_chromium_path.sh")
  else()
    set(chromium_INSTALL_COMMAND "${CMAKE_SCRIPT_PATH}/fix_chromium_path.bat")
  endif()
  
  ExternalProject_Add(
    chromium
    URL ${CHROMIUM_URL}
    URL_MD5 ${CHROMIUM_MD5}
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND ""
  )
  
  ExternalProject_Get_Property(
    chromium
    source_dir
  )
  set(CHROMIUM_SOURCE_DIR ${source_dir})
  
  ExternalProject_Add_Step(
    chromium
    chromium_move
    COMMAND "${chromium_INSTALL_COMMAND}"
    DEPENDERS install
    DEPENDEES download
    WORKING_DIRECTORY ${CHROMIUM_SOURCE_DIR}/..
  )
  
  ExternalProject_Get_Property(
    fetch_cef
    source_dir
  )
  set(FETCH_CEF_SOURCE_DIR ${source_dir})

  ExternalProject_Add(
    cef
    DOWNLOAD_COMMAND ""
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND "" 
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND ""
  )

  ExternalProject_Get_Property(
    fetch_cef
    source_dir
  )
  set(CEF_SOURCE_DIR ${source_dir})
  
  # set some environment dependent variables
  if(WIN32 AND NOT MINGW)
    set(WORKING_DIR ${CHROMIUM_SOURCE_DIR}/src)
    set(CEF_COPY_CMD ${CMAKE_SCRIPT_PATH}/xcopy.bat ${CEF_SOURCE_DIR} cef)
    set(CEF_CONFIG_CMD ${CMAKE_SCRIPT_PATH}/depot_tools_wrapper.bat ${DEPOT_TOOLS_BIN_DIR} ${PYTHON_EXECUTABLE} tools/gclient_hook.py)
    if(DEBUG_CEF)
      set(CEF_BUILD_CMD msbuild ${WORKING_DIR}/cef/cef.sln /target:cef_desura /consoleloggerparameters:verbosity=minimal /m /p:Platform=Win32 /p:Configuration=Debug)
      set(CEF_LIB_DIR ${WORKING_DIR}/cef/Debug/)
    else()
      set(CEF_BUILD_CMD msbuild ${WORKING_DIR}/cef/cef.sln /target:cef_desura /consoleloggerparameters:verbosity=minimal /m /p:Platform=Win32 /p:Configuration=Release)
      set(CEF_LIB_DIR ${WORKING_DIR}/cef/Release/)
    endif()
    set(CEF_LIBRARIES "${CEF_LIB_DIR}/cef_desura.dll" "${CEF_LIB_DIR}/icudt.dll")
    set(CEF_FFMPEG_LIB "${CEF_LIB_DIR}/avcodec-53.dll" "${CEF_LIB_DIR}/avformat-53.dll" "${CEF_LIB_DIR}/avutil-51.dll")
    set(CEF_MEDIA_DIR "${LIB_INSTALL_DIR}")
  else()
    set(WORKING_DIR ${CHROMIUM_SOURCE_DIR}/src)
    set(CEF_COPY_CMD cp -r ${CEF_SOURCE_DIR} ./cef)
    set(CEF_CONFIG_CMD ${CMAKE_SCRIPT_PATH}/depot_tools_wrapper.sh ${DEPOT_TOOLS_BIN_DIR} ./cef_create_projects.sh)
    set(CEF_BUILD_CMD ${CMAKE_SCRIPT_PATH}/depot_tools_wrapper.sh ${DEPOT_TOOLS_BIN_DIR} ${MY_BUILD_TOOL} cef_desura V=1 $ENV{MAKEOPTS} CC.host=${CMAKE_C_COMPILER} CXX.host=${CMAKE_CXX_COMPILER} LINK.host=${CMAKE_CXX_COMPILER} AR.host=${CMAKE_AR} BUILDTYPE=Release)

    set(CEF_LIB_DIR ${CHROMIUM_SOURCE_DIR}/src/out/Release/lib.target)
    set(CEF_FFMPEG_LIB_DIR ${CHROMIUM_SOURCE_DIR}/src/out/Release)
    set(CEF_LIBRARIES "${CEF_LIB_DIR}/libcef_desura.so")
    set(CEF_FFMPEG_LIB "${CEF_FFMPEG_LIB_DIR}/libffmpegsumo.so")
    set(CEF_MEDIA_DIR "${LIB_INSTALL_DIR}/cefmedia")

    add_dependencies(cef ${CEF_LIB_DIR})
    add_dependencies(cef ${CEF_FFMPEG_LIB_DIR})
    add_dependencies(cef ${CEF_LIBRARIES})
    add_dependencies(cef ${CEF_FFMPEG_LIB})
    add_dependencies(cef ${CEF_MEDIA_DIR})
    
    set(ENV{CFLAGS.host} "$ENV{CFLAGS}")
    set(ENV{CXXFLAGS.host} "$ENV{CXXFLAGS}")
    set(ENV{LDFLAGS.host} "$ENV{LDFLAGS}")
  endif()
  
  set(CEF_INCLUDE_DIRS "${CEF_SOURCE_DIR}")
  
  ExternalProject_Add_Step(
    cef
    copy_files
    COMMAND ${CEF_COPY_CMD}
    DEPENDERS download
    WORKING_DIRECTORY ${WORKING_DIR}
  )
  
  ExternalProject_Add_Step(
    cef
    config_cef
    COMMAND ${CEF_CONFIG_CMD}
    DEPENDEES download
    DEPENDERS configure
    WORKING_DIRECTORY ${WORKING_DIR}/cef
  )
  
  ExternalProject_Add_Step(
    cef
    build_cef
    COMMAND ${CEF_BUILD_CMD}
    DEPENDEES configure
    DEPENDERS build
    WORKING_DIRECTORY ${WORKING_DIR}
  )
  
  if(WIN32 AND NOT MINGW)
    # we need to download some external libs
    set(DEFAULT_SVN_URL https://src.chromium.org/chrome/trunk)
    macro(download_cef_dep name svn_url path)
      ExternalProject_Add_Step(
        cef
        ${name}-svn-fetch
        COMMAND ${Subversion_SVN_EXECUTABLE} co ${svn_url} ${WORKING_DIR}/${path} --force
        DEPENDERS config_cef
        WORKING_DIRECTORY ${WORKING_DIR}
      )
    endmacro()
    download_cef_dep(nss ${DEFAULT_SVN_URL}/deps/third_party/nss@94921 third_party/nss)
    download_cef_dep(yasm ${DEFAULT_SVN_URL}/deps/third_party/yasm/binaries@74228 third_party/yasm/binaries)
    download_cef_dep(rlz https://rlz.googlecode.com/svn/trunk@47 rlz)
    download_cef_dep(nacl http://src.chromium.org/native_client/trunk/src/native_client/tests@6668 native_client/tests)
    download_cef_dep(cygwin ${DEFAULT_SVN_URL}/deps/third_party/cygwin@66844 third_party/cygwin)
    download_cef_dep(ffmpeg_bin ${DEFAULT_SVN_URL}/deps/third_party/ffmpeg/binaries/win@99115 third_party/ffmpeg/binaries/chromium/win/ia32)
  endif()
  
  add_dependencies(cef depot_tools)
  add_dependencies(cef chromium)
  add_dependencies(cef fetch_cef)

  install_external_library(cef ${CEF_LIBRARIES} ${CEF_FFMPEG_LIB})
		  
  SET_PROPERTY(TARGET cef                PROPERTY FOLDER "ThirdParty")
elseif(CEF_EXTERNAL_PATH AND WIN32)
  set(CEF_SOURCE_DIR ${CEF_EXTERNAL_PATH})
  set(CEF_INCLUDE_DIRS "${CEF_EXTERNAL_PATH}") 
  
  ExternalProject_Add(
    fetch_cef_external
	SOURCE_DIR "${CEF_EXTERNAL_PATH}"
    UPDATE_COMMAND ""
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )  
  
  install_external_library(fetch_cef_external 
    ${CEF_EXTERNAL_PATH}/Debug/avcodec-53.dll
    ${CEF_EXTERNAL_PATH}/Debug/avformat-53.dll
    ${CEF_EXTERNAL_PATH}/Debug/avutil-51.dll
  )
	  
  add_copy_target_dir_step(fetch_cef_external ${CEF_EXTERNAL_PATH}/Debug/resources)
	  
  ExternalProject_Add(
    fetch_cef_bin
    URL "${CEF_BIN_URL}"
    URL_MD5 ${CEF_BIN_MD5}
    UPDATE_COMMAND ""
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )

  ExternalProject_Get_Property(
    fetch_cef_bin
    source_dir
  )
    
  install_external_library(fetch_cef_bin 
    ${source_dir}/plugin.vch
    ${source_dir}/zlibwapi.dll
  )	  
	  
else(BUILD_CEF)
  ExternalProject_Add(
    fetch_cef
    URL ${CEF_URL}
    URL_MD5 ${CEF_MD5}
    UPDATE_COMMAND ""
    #PATCH_COMMAND ${PATCH_SCRIPT_PATH} ${CMAKE_SOURCE_DIR}/cmake/patches/cef.patch
    CONFIGURE_COMMAND ""
    BUILD_COMMAND "" 
    INSTALL_COMMAND ""
  )
  
  ExternalProject_Get_Property(
    fetch_cef
    source_dir
  )
  set(CEF_SOURCE_DIR ${source_dir})
  set(CEF_INCLUDE_DIRS "${CEF_SOURCE_DIR}")
  
  SET_PROPERTY(TARGET fetch_cef PROPERTY FOLDER "ThirdParty")
  
  if(WIN32)
    ExternalProject_Add(
      fetch_cef_bin
      URL "${CEF_BIN_URL}"
      URL_MD5 ${CEF_BIN_MD5}
      UPDATE_COMMAND ""
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
    )

    ExternalProject_Get_Property(
      fetch_cef_bin
      source_dir
    )

    install_external_library(fetch_cef_bin 
      ${source_dir}/avcodec-53.dll
      ${source_dir}/avformat-53.dll
      ${source_dir}/avutil-51.dll
      ${source_dir}/cef.pak
      ${source_dir}/cef_100_percent.pak
      ${source_dir}/cef_200_percent.pak
      ${source_dir}/cef_desura.dll
      ${source_dir}/cef_sandbox.lib
	  ${source_dir}/d3dcompiler_43.dll
	  ${source_dir}/d3dcompiler_47.dll
	  ${source_dir}/devtools_resources.pak
	  ${source_dir}/ffmpegsumo.dll
	  ${source_dir}/icudtl.dat
	  ${source_dir}/libcef.dll
	  ${source_dir}/libcef.lib
	  ${source_dir}/libEGL.dll
	  ${source_dir}/libGLESv2.dll
	  ${source_dir}/pdf.dll
      ${source_dir}/plugin.vch
	  ${source_dir}/wow_helper.exe
      ${source_dir}/zlibwapi.dll)

    install_external_locales(fetch_cef_bin 
	  ${source_dir}/locales/am.pak
	  ${source_dir}/locales/ar.pak
	  ${source_dir}/locales/bg.pak
	  ${source_dir}/locales/bn.pak
	  ${source_dir}/locales/ca.pak
	  ${source_dir}/locales/cs.pak
	  ${source_dir}/locales/da.pak
	  ${source_dir}/locales/de.pak
	  ${source_dir}/locales/el.pak
	  ${source_dir}/locales/en-GB.pak
	  ${source_dir}/locales/en-US.pak
	  ${source_dir}/locales/es.pak
	  ${source_dir}/locales/es-419.pak
	  ${source_dir}/locales/et.pak
	  ${source_dir}/locales/fa.pak
	  ${source_dir}/locales/fi.pak
	  ${source_dir}/locales/fil.pak
	  ${source_dir}/locales/fr.pak
	  ${source_dir}/locales/gu.pak
	  ${source_dir}/locales/he.pak
	  ${source_dir}/locales/hi.pak
	  ${source_dir}/locales/hr.pak
	  ${source_dir}/locales/hu.pak
	  ${source_dir}/locales/id.pak
	  ${source_dir}/locales/it.pak
	  ${source_dir}/locales/ja.pak
	  ${source_dir}/locales/kn.pak
	  ${source_dir}/locales/ko.pak
	  ${source_dir}/locales/lt.pak
	  ${source_dir}/locales/lv.pak
	  ${source_dir}/locales/ml.pak
	  ${source_dir}/locales/mr.pak
	  ${source_dir}/locales/ms.pak
	  ${source_dir}/locales/nb.pak
	  ${source_dir}/locales/nl.pak
	  ${source_dir}/locales/pl.pak
	  ${source_dir}/locales/pt-BR.pak
	  ${source_dir}/locales/pt-PT.pak
	  ${source_dir}/locales/ro.pak
	  ${source_dir}/locales/ru.pak
	  ${source_dir}/locales/sk.pak
	  ${source_dir}/locales/sl.pak
	  ${source_dir}/locales/sr.pak
	  ${source_dir}/locales/sv.pak
	  ${source_dir}/locales/sw.pak
	  ${source_dir}/locales/ta.pak
	  ${source_dir}/locales/te.pak
	  ${source_dir}/locales/th.pak
	  ${source_dir}/locales/tr.pak
	  ${source_dir}/locales/uk.pak
	  ${source_dir}/locales/vi.pak
	  ${source_dir}/locales/zh-CN.pak
	  ${source_dir}/locales/zh-TW.pak)

  endif()	
endif()

