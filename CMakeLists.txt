cmake_minimum_required(VERSION 3.22)

add_library(diff INTERFACE)

target_include_directories(diff 
                           INTERFACE "include")

target_link_libraries(diff
                      INTERFACE nlohmann_json)

add_subdirectory(test)