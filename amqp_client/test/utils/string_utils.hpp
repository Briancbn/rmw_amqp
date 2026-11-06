// Copyright 2026 Chen Bainian
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

#ifndef UTILS__STRING_UTILS_HPP_
#define UTILS__STRING_UTILS_HPP_

#include <random>
#include <string>

namespace amqp_client
{
namespace test_utils
{

inline std::string generate_random_string(size_t length)
{
  // available characters to choose from
  const std::string characters =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";

  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_int_distribution<size_t> distribution(0, characters.length() - 1);

  std::string random_string;
  random_string.reserve(length);

  for (size_t i = 0; i < length; ++i) {
    random_string += characters[distribution(generator)];
  }

  return random_string;
}

}  // namespace test_utils
}  // namespace amqp_client

#endif  // UTILS__STRING_UTILS_HPP_
