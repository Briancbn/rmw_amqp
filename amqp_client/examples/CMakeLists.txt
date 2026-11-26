add_executable(publisher_example publisher_example.cpp)
target_link_libraries(publisher_example ${PROJECT_NAME})

add_executable(subscriber_example subscriber_example.cpp)
target_link_libraries(subscriber_example ${PROJECT_NAME})

install(TARGETS publisher_example subscriber_example
  DESTINATION lib/${PROJECT_NAME}
)
