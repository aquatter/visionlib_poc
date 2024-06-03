cmake_minimum_required(VERSION 3.28.0)
project(welib3d)

function(find_sources _PATH all_sources pattern)
    file(GLOB_RECURSE sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${_PATH}/${pattern})
    set(${all_sources} ${${all_sources}} ${sources} PARENT_SCOPE)
endfunction()

option(BUILD_TEST_APP "Build test app" OFF)

if(NOT ${BUILD_TEST_APP})
    list(APPEND _3RD_PARTY_LIST "Sensor3d.dll" "tbb12.dll" "lz4.dll")
    list(APPEND _FILE_LIST "welib3d.dll" "welib3d.lib" "welib3dd.dll" "welib3dd.lib" "welib3dd.pdb")
    set(_BASE_URL "https://github.com/aquatter/visionlib_poc/releases/download/v0.0.2/")

    function(get_binaries path file_list)
        foreach(file_name ${file_list})
            set(SAVE_PATH ${CMAKE_CURRENT_LIST_DIR}/${path}/${file_name})

            message(STATUS "Downloading " ${file_name} "...")

            if(NOT EXISTS ${SAVE_PATH})
                file(DOWNLOAD
                    ${_BASE_URL}${file_name}
                    ${SAVE_PATH}
                    SHOW_PROGRESS
                    STATUS DOWNLOAD_RESULT)

                list(GET DOWNLOAD_RESULT 0 DOWNLOAD_RESULT_CODE)

                if(NOT DOWNLOAD_RESULT_CODE EQUAL 0)
                    message(FATAL_ERROR "Failed downloading ${file_name}! Error: ${DOWNLOAD_RESULT}")
                endif()

                message(STATUS "Saved to: " ${SAVE_PATH})
            else()
                message(STATUS ${file_name} " already downloaded")
            endif()
        endforeach()
    endfunction()

    get_binaries(bin "${_FILE_LIST}")
    get_binaries(3rd_parties "${_3RD_PARTY_LIST}")
endif()

find_sources(include _HEADERS "*.h")
find_sources(cmake _CMAKE "*.cmake")
find_sources(bin _LIB "*.lib")
find_sources(bin _SO "*.dll")
find_sources(bin _PDB "*.pdb")
find_sources(3rd_parties _OTHER_SO "*.dll")

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${_HEADERS}")

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)

install(FILES ${_CMAKE} DESTINATION cmake)
install(FILES ${_SO} DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${_PDB} DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${_LIB} DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${_OTHER_SO} DESTINATION ${CMAKE_INSTALL_BINDIR})

if(${BUILD_TEST_APP})
    add_subdirectory(test_app)
endif()
