#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "welib3d::welib3d" for configuration "Release"
set_property(TARGET welib3d::welib3d APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(welib3d::welib3d PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/welib3d.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "TBB::tbb;LZ4::lz4_shared"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/welib3d.dll"
  )

list(APPEND _cmake_import_check_targets welib3d::welib3d )
list(APPEND _cmake_import_check_files_for_welib3d::welib3d "${_IMPORT_PREFIX}/lib/welib3d.lib" "${_IMPORT_PREFIX}/bin/welib3d.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
