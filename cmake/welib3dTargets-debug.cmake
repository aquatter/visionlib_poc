#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "welib3d::welib3d" for configuration "Debug"
set_property(TARGET welib3d::welib3d APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(welib3d::welib3d PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/welib3dd.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBUG "TBB::tbb;LZ4::lz4_shared"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/welib3dd.dll"
  )

list(APPEND _cmake_import_check_targets welib3d::welib3d )
list(APPEND _cmake_import_check_files_for_welib3d::welib3d "${_IMPORT_PREFIX}/lib/welib3dd.lib" "${_IMPORT_PREFIX}/bin/welib3dd.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
