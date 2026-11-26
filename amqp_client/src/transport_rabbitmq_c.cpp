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

#include <memory>

#include "amqp_client/transport_rabbitmq_c.hpp"
#include "amqp_client/connection_rabbitmq_c.hpp"
#include "amqp_client/connection_info.hpp"
#include "amqp_client/amqp_error.hpp"
#include "amqp_client/log.hpp"

namespace amqp_client
{

namespace rabbitmq_c
{

Transport::Transport(std::shared_ptr<RabbitmqCInterface> rabbitmq_c_interface)
: rabbitmq_c_interface_(rabbitmq_c_interface)
{
}

std::shared_ptr<amqp_client::Connection> Transport::create_connection(
  const ConnectionInfo::ConstPtr & info,
  AMQPError * error
)
{
  amqp_connection_state_t conn = rabbitmq_c_interface_->new_connection();
  if (conn == nullptr) {
    AMQPError::from_message("Failed to allocate new AMQP connection", error);
    return nullptr;
  }

  amqp_socket_t * socket = rabbitmq_c_interface_->tcp_socket_new(conn);
  if (socket == nullptr) {
    AMQPError::from_message("Failed to create TCP socket", error);
    amqp_destroy_connection(conn);
    return nullptr;
  }

  int rc = rabbitmq_c_interface_->socket_open(
    socket, info->host().c_str(), info->port());
  if (rc != AMQP_STATUS_OK) {
    AMQPError::from_message("Failed to open TCP socket", error);
    amqp_destroy_connection(conn);
    return nullptr;
  }

  amqp_rpc_reply_t reply = rabbitmq_c_interface_->login(
    conn,
    info->vhost().c_str(),
    info->channel_max(),
    info->frame_max(),
    info->heartbeat(),
    &info->sasl_config()
  );

  AMQPError local;
  AMQPError * target = error ? error : &local;
  if (AMQPError::from_amqp_rpc_reply(&reply, target)) {
    amqp_destroy_connection(conn);
    return nullptr;
  }

  return std::make_shared<rabbitmq_c::Connection>(conn, rabbitmq_c_interface_);
}

}  // namespace rabbitmq_c
}  // namespace amqp_client
