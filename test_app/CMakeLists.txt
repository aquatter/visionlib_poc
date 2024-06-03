set(CMAKE_CXX_STANDARD 20)
set(PROJECT_NAME test_app)
find_package(welib3d REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} welib3d::welib3d)
include(GNUInstallDirs)

install(FILES $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
