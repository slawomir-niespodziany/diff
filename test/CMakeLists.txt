# topology_loader_test
add_executable(topology_loader_test "TopologyLoaderTest.cpp")
    
target_link_libraries(
  topology_loader_test
  diff
  GTest::gtest_main
)

gtest_discover_tests(topology_loader_test)
add_test(NAME topology_loader_test COMMAND topology_loader_test)


# cast_checker_test
add_executable(cast_checker_test "CastCheckerTest.cpp")
target_link_libraries(cast_checker_test diff GTest::gtest_main)

gtest_discover_tests(cast_checker_test)
add_test(NAME cast_checker_test COMMAND cast_checker_test)