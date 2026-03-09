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

#ifndef AMQP_CLIENT__ENVELOPE_HPP_
#define AMQP_CLIENT__ENVELOPE_HPP_

#include <string>
#include "amqp_client/message.hpp"

namespace amqp_client
{

struct Envelope {
  int channel;
  std::string consumer_tag;
  uint64_t delivery_tag;
  bool redelivered;
  std::string exchange;
  std::string routing_key;
  Message message;
};

}  // namespace amqp_client

#endif  // AMQP_CLIENT__ENVELOPE_HPP_
