set(CMAKE_CXX_STANDARD 20)
set(PROJECT_NAME test_app)
find_package(welib3d REQUIRED)

cmake_policy(SET CMP0069 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} welib3d::welib3d)
include(GNUInstallDirs)
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)

if(ipo_supported)
    message(STATUS "IPO supported")
    set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO not supported")
endif()

install(FILES $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
