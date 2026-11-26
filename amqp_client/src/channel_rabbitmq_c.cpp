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
#include <string>

#include "amqp_client/channel_rabbitmq_c.hpp"
#include "amqp_client/connection_rabbitmq_c.hpp"
#include "amqp_client/envelope.hpp"
#include "amqp_client/log.hpp"

namespace amqp_client
{

namespace rabbitmq_c
{

// ---------------------------------------------------------------------------
// Helper: check an amqp_rpc_reply_t, populating error if non-null.
// Returns true on success, false on error.
// ---------------------------------------------------------------------------
static bool check_reply(
  const amqp_rpc_reply_t & reply,
  AMQPError * error)
{
  AMQPError local;
  AMQPError * target = error ? error : &local;
  return !AMQPError::from_amqp_rpc_reply(&reply, target);
}

// ===========================================================================
// Connection
// ===========================================================================

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
  if (connection_ == nullptr) {
    AMQPError::from_message("Error creating channel: connection is null", error);
    return nullptr;
  }

  amqp_channel_t channel_id;
  if (channel_number == nullptr) {
    channel_id = _next_channel_id();
  } else {
    channel_id = static_cast<amqp_channel_t>(*channel_number);
  }

  amqp_channel_open(connection_, channel_id);
  amqp_rpc_reply_t reply = amqp_get_rpc_reply(connection_);
  if (!check_reply(reply, error)) {
    return nullptr;
  }

  channel_ids_.emplace(channel_id);
  if (channel_number == nullptr) {
    last_channel_id_ = channel_id;
  }

  return std::make_shared<rabbitmq_c::Channel>(
    connection_, channel_id, rabbitmq_c_interface_);
}

bool Connection::close(
  AMQPError * error
)
{
  if (connection_ == nullptr) {
    AMQPError::from_message("Error closing connection: connection is null", error);
    return false;
  }

  amqp_rpc_reply_t reply = amqp_connection_close(connection_, AMQP_REPLY_SUCCESS);
  return check_reply(reply, error);
}

amqp_channel_t Connection::_next_channel_id()
{
  amqp_channel_t id = last_channel_id_ + 1;
  while (channel_ids_.find(id) != channel_ids_.end()) {
    id += 1;
  }
  return id;
}

// ===========================================================================
// Channel
// ===========================================================================

Channel::Channel(
  amqp_connection_state_t connection,
  amqp_channel_t channel_id,
  const std::shared_ptr<RabbitmqCInterface> & rabbitmq_c_interface
)
: connection_(connection),
  channel_id_(channel_id),
  rabbitmq_c_interface_(rabbitmq_c_interface)
{
}

Channel::~Channel()
{
}

bool Channel::exchange_declare(
  const std::string & exchange,
  const std::string & exchange_type,
  bool passive,
  bool durable,
  bool auto_delete,
  bool internal,
  AMQPError * error
)
{
  amqp_exchange_declare(
    connection_, channel_id_,
    amqp_cstring_bytes(exchange.c_str()),
    amqp_cstring_bytes(exchange_type.c_str()),
    passive, durable, auto_delete, internal,
    amqp_empty_table);
  amqp_rpc_reply_t reply = amqp_get_rpc_reply(connection_);
  return check_reply(reply, error);
}

std::string Channel::queue_declare(
  const std::string & queue,
  bool passive,
  bool durable,
  bool exclusive,
  bool auto_delete,
  AMQPError * error
)
{
  amqp_queue_declare_ok_t * result = amqp_queue_declare(
    connection_, channel_id_,
    amqp_cstring_bytes(queue.c_str()),
    passive, durable, exclusive, auto_delete,
    amqp_empty_table);

  amqp_rpc_reply_t reply = amqp_get_rpc_reply(connection_);
  if (!check_reply(reply, error) || result == nullptr) {
    return "";
  }

  return std::string(
    reinterpret_cast<const char *>(result->queue.bytes),
    result->queue.len);
}

bool Channel::queue_bind(
  const std::string & queue,
  const std::string & exchange,
  const std::string & routing_key,
  AMQPError * error
)
{
  amqp_queue_bind(
    connection_, channel_id_,
    amqp_cstring_bytes(queue.c_str()),
    amqp_cstring_bytes(exchange.c_str()),
    amqp_cstring_bytes(routing_key.c_str()),
    amqp_empty_table);
  amqp_rpc_reply_t reply = amqp_get_rpc_reply(connection_);
  return check_reply(reply, error);
}

bool Channel::basic_publish(
  const std::string & exchange,
  const std::string & routing_key,
  const std::string & body,
  bool mandatory,
  bool immediate,
  AMQPError * error
)
{
  int rc = amqp_basic_publish(
    connection_, channel_id_,
    amqp_cstring_bytes(exchange.c_str()),
    amqp_cstring_bytes(routing_key.c_str()),
    mandatory, immediate,
    nullptr,
    amqp_cstring_bytes(body.c_str()));

  if (rc != AMQP_STATUS_OK) {
    AMQPError::from_message(amqp_error_string2(rc), error);
    return false;
  }
  return true;
}

std::string Channel::basic_consume(
  const std::string & queue,
  MessageCallback on_message_callback,
  bool no_local,
  bool auto_ack,
  bool exclusive,
  const std::string & consumer_tag,
  AMQPError * error
)
{
  on_message_callback_ = on_message_callback;

  amqp_basic_consume_ok_t * result = amqp_basic_consume(
    connection_, channel_id_,
    amqp_cstring_bytes(queue.c_str()),
    amqp_cstring_bytes(consumer_tag.c_str()),
    no_local, auto_ack, exclusive,
    amqp_empty_table);

  amqp_rpc_reply_t reply = amqp_get_rpc_reply(connection_);
  if (!check_reply(reply, error) || result == nullptr) {
    return "";
  }

  return std::string(
    reinterpret_cast<const char *>(result->consumer_tag.bytes),
    result->consumer_tag.len);
}

void Channel::start_consuming(double timeout_sec)
{
  while (true) {
    amqp_maybe_release_buffers(connection_);

    amqp_envelope_t envelope;
    struct timeval tv;
    struct timeval * tv_ptr = nullptr;
    if (timeout_sec >= 0.0) {
      tv.tv_sec = static_cast<long>(timeout_sec);
      tv.tv_usec = static_cast<long>((timeout_sec - tv.tv_sec) * 1e6);
      tv_ptr = &tv;
    }

    amqp_rpc_reply_t reply = amqp_consume_message(connection_, &envelope, tv_ptr, 0);

    if (reply.reply_type == AMQP_RESPONSE_LIBRARY_EXCEPTION) {
      if (reply.library_error == AMQP_STATUS_TIMEOUT) {
        break;
      }
      AMQP_CLIENT_LOG_ERROR_STREAM(
        "start_consuming: library error: " << amqp_error_string2(reply.library_error));
      break;
    }

    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
      AMQP_CLIENT_LOG_ERROR_STREAM(
        "start_consuming: unexpected reply type: " << reply.reply_type);
      break;
    }

    if (on_message_callback_) {
      Envelope env;
      env.channel = envelope.channel;
      env.consumer_tag = std::string(
        reinterpret_cast<const char *>(envelope.consumer_tag.bytes),
        envelope.consumer_tag.len);
      env.delivery_tag = envelope.delivery_tag;
      env.redelivered = envelope.redelivered;
      env.exchange = std::string(
        reinterpret_cast<const char *>(envelope.exchange.bytes),
        envelope.exchange.len);
      env.routing_key = std::string(
        reinterpret_cast<const char *>(envelope.routing_key.bytes),
        envelope.routing_key.len);
      env.message.body = std::string(
        reinterpret_cast<const char *>(envelope.message.body.bytes),
        envelope.message.body.len);

      amqp_destroy_envelope(&envelope);
      on_message_callback_(env);
    } else {
      amqp_destroy_envelope(&envelope);
    }
  }
}

bool Channel::close(
  AMQPError * error
)
{
  amqp_channel_close(connection_, channel_id_, AMQP_REPLY_SUCCESS);
  amqp_rpc_reply_t reply = amqp_get_rpc_reply(connection_);
  return check_reply(reply, error);
}

}  // namespace rabbitmq_c
}  // namespace amqp_client
