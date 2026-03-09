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

#ifndef AMQP_CLIENT__RABBITMQ_C_API_HPP_
#define AMQP_CLIENT__RABBITMQ_C_API_HPP_

// different include for version 0.12.0 onwards
#if AMQP_VERSION >= 0x000C0001
#include "rabbitmq-c/amqp.h"
#else
#include "amqp.h"  // NOLINT(build/include_subdir)
#endif

#include "amqp_client/amqp_error.hpp"
#include "amqp_client/sasl_config.hpp"
#include "amqp_client/macros.hpp"

namespace amqp_client
{

class RabbitmqCInterface
{
public:
  RabbitmqCInterface() = default;
  virtual ~RabbitmqCInterface() {};

  virtual amqp_connection_state_t new_connection() = 0;

  virtual amqp_socket_t * tcp_socket_new(amqp_connection_state_t state) = 0;

  virtual int socket_open(amqp_socket_t * self, const char * host, int port) = 0;

  virtual amqp_rpc_reply_t login(
    amqp_connection_state_t state,
    const char * vhost,
    int channel_max,
    int frame_max,
    int heartbeat,
    const SASLConfig * sasl_config
  ) = 0;

private:
  AMQP_CLIENT_DISABLE_COPY(RabbitmqCInterface)
};

class RabbitmqCApi : public RabbitmqCInterface
{
public:
  RabbitmqCApi();
  virtual ~RabbitmqCApi();

  amqp_connection_state_t new_connection() override;

  amqp_socket_t * tcp_socket_new(amqp_connection_state_t state) override;

  int socket_open(amqp_socket_t * self, const char * host, int port) override;

  amqp_rpc_reply_t login(
    amqp_connection_state_t state,
    const char * vhost,
    int channel_max,
    int frame_max,
    int heartbeat,
    const SASLConfig * sasl_config
  ) override;

private:
  AMQP_CLIENT_DISABLE_COPY(RabbitmqCApi)
};

}  // namespace amqp_client

#endif  // AMQP_CLIENT__RABBITMQ_C_API_HPP_
