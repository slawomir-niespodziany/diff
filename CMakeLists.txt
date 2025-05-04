cmake_minimum_required(VERSION 3.15)

project(
    diff
    VERSION 1.0.0
    LANGUAGES CXX)

add_library(diff INTERFACE)
add_library(diff::diff ALIAS diff)

target_include_directories(diff INTERFACE $<INSTALL_INTERFACE:include>
                                          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

add_subdirectory(test)

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/diff)

install(TARGETS diff EXPORT diff-targets)
install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(
    EXPORT diff-targets
    DESTINATION ${INSTALL_CONFIGDIR}
    NAMESPACE diff::
    FILE diff.cmake)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/diffConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY ExactVersion)
configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/Config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/diffConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/diffConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/diffConfigVersion.cmake DESTINATION ${INSTALL_CONFIGDIR})

export(
    EXPORT diff-targets
    NAMESPACE diff::
    FILE ${CMAKE_CURRENT_BINARY_DIR}/diffTargets.cmake)

export(PACKAGE diff)
