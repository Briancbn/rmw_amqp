cmake_minimum_required(VERSION 3.5)
project(amqp_client)

# Default to C++17 for std::optional
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  add_compile_options(/W4)
endif()

find_package(ament_cmake REQUIRED)
find_package(spdlog REQUIRED)
find_package(spdlog_vendor REQUIRED)

# Find rabbitmq_c
include(./cmake/find_rabbitmq_c.cmake)
find_rabbitmq_c()

# Lib
add_library(${PROJECT_NAME} SHARED
  src/amqp_error.cpp
  src/log.cpp
  src/default_log_handler.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  spdlog::spdlog_header_only
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  ${Rabbitmqc_LIBRARY}
)

target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
  "$<INSTALL_INTERFACE:include>"
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  add_subdirectory(test)
endif()

install(
  DIRECTORY include/
  DESTINATION include
)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

ament_export_include_directories(include)
ament_export_targets(${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_export_libraries(${PROJECT_NAME})
ament_package()
