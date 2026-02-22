find_package(ament_cmake_gtest REQUIRED)
find_package(ament_cmake_gmock REQUIRED)

add_definitions(-DTEST_RESOURCES_DIRECTORY="${TEST_RESOURCES_DIRECTORY}")

# LOG
ament_add_gmock(test_log
  test_log.cpp
)
target_link_libraries(test_log ${PROJECT_NAME})

ament_add_gtest(test_default_log_handler
  test_default_log_handler.cpp
)
target_link_libraries(test_default_log_handler ${PROJECT_NAME})
