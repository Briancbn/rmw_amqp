// Copyright 2025 Chen Bainian
// Copyright 2014 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Original file taken from:
// https://github.com/ros2/rclcpp/blob/master/rclcpp/include/rclcpp/macro.hpp

#ifndef AMQP_CLIENT__MACROS_HPP_
#define AMQP_CLIENT__MACROS_HPP_

/**
 * Disables the copy constructor and operator= for the given class.
 *
 * Use in the private section of the class.
 */
#define AMQP_CLIENT_DISABLE_COPY(...) \
  __VA_ARGS__(const __VA_ARGS__ &) = delete; \
  __VA_ARGS__ & operator=(const __VA_ARGS__ &) = delete;

#endif  // AMQP_CLIENT__MACROS_HPP_
