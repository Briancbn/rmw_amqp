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

// different include for version 0.12.0 onwards
#if AMQP_VERSION >= 0x000C0001
#include "rabbitmq-c/amqp_tcp_socket.h"
#else
#include "amqp_tcp_socket.h"  // NOLINT(build/include_subdir)
#endif

#include "amqp_client/rabbitmq_c_api.hpp"

namespace amqp_client
{

RabbitmqCApi::RabbitmqCApi()
{
}

RabbitmqCApi::~RabbitmqCApi()
{
}

amqp_connection_state_t RabbitmqCApi::new_connection()
{
  return amqp_new_connection();
}


amqp_socket_t * RabbitmqCApi::tcp_socket_new(amqp_connection_state_t state)
{
  return amqp_tcp_socket_new(state);
}

int RabbitmqCApi::socket_open(amqp_socket_t * self, const char * host, int port)
{
  return amqp_socket_open(self, host, port);
}


amqp_rpc_reply_t RabbitmqCApi::login(
  amqp_connection_state_t state,
  const char * vhost,
  int channel_max,
  int frame_max,
  int heartbeat,
  const SASLConfig * sasl_config
) {
  // TODO(anyone): potential segfault when sasl_config is nullptr
  if (sasl_config->external) {
    return amqp_login(
      state,
      vhost,
      channel_max,
      frame_max,
      heartbeat,
      AMQP_SASL_METHOD_EXTERNAL,
      sasl_config->identity.c_str()
    );
  } else {
    return amqp_login(
      state,
      vhost,
      channel_max,
      frame_max,
      heartbeat,
      AMQP_SASL_METHOD_PLAIN,
      sasl_config->username.c_str(),
      sasl_config->password.c_str()
    );
  }
}

}  // namespace amqp
