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

#include "amqp_client/connection_rabbitmq_c.hpp"
// #include "amqp_client/channel_rabbitmq_c.hpp"
#include "amqp_client/rabbitmq_c_api.hpp"

namespace amqp_client
{

namespace rabbitmq_c
{

Connection::Connection(
  const amqp_connection_state_t connection,
  const std::shared_ptr<RabbitmqCInterface> & rabbitmq_c_interface
)
: amqp_client::Connection(),
  connection_(connection),
  rabbitmq_c_interface_(rabbitmq_c_interface),
  last_channel_id_(0)
{
}

Connection::~Connection()
{
  if (connection_ != nullptr) {
    amqp_destroy_connection(connection_);
  }
}

std::shared_ptr<amqp_client::Channel> Connection::channel(
  const uint16_t * channel_number,
  AMQPError * error
)
{
  if (error == nullptr) {
  if (connection_ == nullptr) {
    if (error != nullptr) {
      AMQPError::from_message(
        "Error creating channel: connection is null",
        error
      );
    }
    return nullptr;
  }

  amqp_channel_t channel_id;
  if (channel_number == nullptr) {
    channel_id = _next_channel_id();
  } else {
    channel_id = *channel_number;
  }
  amqp_channel_open(connection_, channel_id);

  if(!rabbitmq_c_interface_->process_amqp_rpc_reply(
      amqp_get_rpc_reply(connection_),
      "Error logging in",
      error
    )) {
    return nullptr;
  }

  channel_ids_.emplace(channel_id);
  if (channel_number == nullptr) {
    last_channel_id_ = channel_id;
  }
  return std::make_shared<amqp::rabbitmq_c::Channel>(connection_, channel_id, rabbitmq_c_interface_);
}

bool Connection::close(
  AMQPError * error
) {
  if (connection_ == nullptr) {
    if (error != nullptr) {
      error->message = "Error closing connection: connection is null";
    }
    return false;
  }

  amqp_rpc_reply_t reply = amqp_connection_close(connection_, AMQP_REPLY_SUCCESS);

  if(!rabbitmq_c_interface_->process_amqp_rpc_reply(reply, "Error closing connection", error)) {
    return false;
  }

  return true;
}

amqp_channel_t Connection::_next_channel_id(
) {
  amqp_channel_t id = last_channel_id_ + 1;

  // increment until the channel id is not used
  while(channel_ids_.find(id) != channel_ids_.end()) {
    id += 1;
  }
  return id;
}

}  // namespace rabbitmq_c
}  // namespace amqp
}  // namespace rmf2_scheduler
