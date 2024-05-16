set(welib3d_VERSION 0.0.0)


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was welib3dConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

####################################################################################



if(NOT TARGET welib3d::welib3d)
  include("${CMAKE_CURRENT_LIST_DIR}/welib3dTargets.cmake")
endif()

# Compatibility
get_property(welib3d_welib3d_INCLUDE_DIR TARGET welib3d::welib3d PROPERTY INTERFACE_INCLUDE_DIRECTORIES)

set(welib3d_LIBRARIES welib3d::welib3d)
set(welib3d_INCLUDE_DIRS "${welib3d_welib3d_INCLUDE_DIR}")
list(REMOVE_DUPLICATES welib3d_INCLUDE_DIRS)


