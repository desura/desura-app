# from https://bitbucket.org/sumwars/sumwars-code/src/b2ba13b0d7406f9130df9f59a904c5fef69e29a3/CMakeModules/FindTinyXML.cmake?at=default

################################################################################
# Custom cmake module for CEGUI to find tinyxml
################################################################################
include(FindPackageHandleStandardArgs)
include(CheckCXXSourceCompiles)

find_path(TINYXML2_H_PATH NAMES tinyxml2.h)
find_library(TINYXML2_LIB NAMES tinyxml2 libtinyxml2)
mark_as_advanced(TINYXML2_H_PATH TINYXML2_LIB)

find_package_handle_standard_args(TINYXML DEFAULT_MSG TINYXML2_LIB TINYXML2_H_PATH)

if (TINYXML_FOUND)
    # what API version do we have here?
    set(CMAKE_REQUIRED_INCLUDES ${TINYXML2_H_PATH})
    set(CMAKE_REQUIRED_LIBRARIES ${TINYXML2_LIB})
    check_cxx_source_compiles("
    #include <tinyxml2.h>
    int main() {
        tinyxml2::XMLError m_eXMLLoadError = tinyxml2::XML_ERROR_EMPTY_DOCUMENT;
        return 0;
    }"
    
    TINYXML_API_TEST)
endif()

if (TINYXML_API_TEST)
    set (TINYXML_INCLUDE_DIR ${TINYXML2_H_PATH})
    set (TINYXML_LIBRARIES ${TINYXML_LIB})
else()
	if (TINYXML_FOUND)
		message("-- System TinyXml2 found but is too old, using local tinyxml2")
	else()
		message("-- System TinyXml2 not found, using local tinyxml2")
	endif()

	set(TINYXML_FOUND)
    set(TINYXML_INCLUDE_DIR)
    set(TINYXML_LIBRARIES)
endif()

