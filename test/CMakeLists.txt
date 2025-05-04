include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
                              DOWNLOAD_EXTRACT_TIMESTAMP ON)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP ON)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

# test_topology_loader
add_executable(test_topology_loader TestTopologyLoader.cpp)

set_property(TARGET test_topology_loader PROPERTY CXX_STANDARD 17)
set_property(TARGET test_topology_loader PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(test_topology_loader diff::diff nlohmann_json::nlohmann_json
                      GTest::gtest_main)

gtest_discover_tests(test_topology_loader)

# test_cast_checker
add_executable(test_cast_checker TestCastChecker.cpp)

set_property(TARGET test_cast_checker PROPERTY CXX_STANDARD 17)
set_property(TARGET test_cast_checker PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(test_cast_checker diff::diff nlohmann_json::nlohmann_json GTest::gtest_main)

gtest_discover_tests(test_cast_checker)
