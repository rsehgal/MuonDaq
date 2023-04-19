# FindTinyXml.cmake

# Search for tinyxml2 headers and libraries

find_path(TINYXML2_INCLUDE_DIR tinyxml2.h
    HINTS ${CMAKE_PREFIX_PATH}/include
)

find_library(TINYXML2_LIBRARY NAMES tinyxml2 libtinyxml2
    HINTS ${CMAKE_PREFIX_PATH}/lib
)

# If both the header file and library are found, set the TINYXML2_FOUND variable
if (TINYXML2_INCLUDE_DIR AND TINYXML2_LIBRARY)
    set(TINYXML2_FOUND TRUE)
endif()

# If tinyxml2 is found, set the TINYXML2_LIBRARIES variable
if (TINYXML2_FOUND)
    set(TINYXML2_LIBRARIES ${TINYXML2_LIBRARY})
endif()

# Export the tinyxml2 variables
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TinyXml2 DEFAULT_MSG TINYXML2_INCLUDE_DIR TINYXML2_LIBRARIES)
mark_as_advanced(TINYXML2_INCLUDE_DIR TINYXML2_LIBRARIES)

