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

#include <iomanip>
#include <sstream>

#include "amqp_client/amqp_error.hpp"
#include "amqp_client/rabbitmq_c_api.hpp"

namespace amqp_client
{

AMQPError::AMQPError()
: type_(AMQPExceptionType::NORMAL),
  server_error_(nullptr),
  library_error_(0),
  message_("")
{
}

AMQPError::~AMQPError()
{
  if (type_ == AMQPExceptionType::SERVER) {
    delete server_error_;
  }
}

bool AMQPError::from_amqp_rpc_reply(
  const amqp_rpc_reply_t * reply,
  AMQPError * error
)
{
  switch (reply->reply_type) {
    case AMQP_RESPONSE_NORMAL:
      error->type_ = AMQPExceptionType::NORMAL;
      break;

    case AMQP_RESPONSE_NONE:
      error->type_ = AMQPExceptionType::NONE;
      break;

    case AMQP_RESPONSE_LIBRARY_EXCEPTION:
      error->type_ = AMQPExceptionType::LIBRARY;
      error->library_error_ = reply->library_error;
      break;

    case AMQP_RESPONSE_SERVER_EXCEPTION:
      error->type_ = AMQPExceptionType::SERVER;
      AMQPServerError * server_error = new AMQPServerError;
      error->server_error_ = server_error;
      server_error->method = reply->reply.id;
      switch (reply->reply.id) {
        case AMQP_CONNECTION_CLOSE_METHOD: {
            const amqp_connection_close_t * m =
              reinterpret_cast<const amqp_connection_close_t *>(
              reply->reply.decoded
              );
            server_error->reply_code = m->reply_code;
            server_error->reply_text = std::string(
              reinterpret_cast<const char *>(m->reply_text.bytes),
              m->reply_text.len
            );
            server_error->class_id = m->class_id;
            server_error->method_id = m->method_id;
            break;
          }
        case AMQP_CHANNEL_CLOSE_METHOD: {
            const amqp_channel_close_t * m =
              reinterpret_cast<const amqp_channel_close_t *>(
              reply->reply.decoded
              );
            server_error->reply_code = m->reply_code;
            server_error->reply_text = std::string(
              reinterpret_cast<const char *>(m->reply_text.bytes),
              m->reply_text.len
            );
            server_error->class_id = m->class_id;
            server_error->method_id = m->method_id;
            break;
          }
        default:
          break;
      }
      break;
  }
  return static_cast<bool>(*error);
}

void AMQPError::from_message(
  const std::string & message,
  AMQPError * error
) {
  error->type_ = AMQPExceptionType::CUSTOM;
  error->message_ = message;
}

std::string AMQPError::str() const
{
  std::string result;
  switch (type_) {
    case AMQPExceptionType::NORMAL:
      result = "operation completed successfully";
      break;
    case AMQPExceptionType::NONE:
      result = "missing RPC reply type!";
      break;
    case AMQPExceptionType::LIBRARY:
      result = amqp_error_string2(library_error_);
      break;
    case AMQPExceptionType::SERVER:
    {
      std::ostringstream oss;
      switch (server_error_->method) {
        case AMQP_CONNECTION_CLOSE_METHOD:
          oss << "server connection error " << server_error_->reply_code <<
            ", message: " << server_error_->reply_text;
          break;
        case AMQP_CHANNEL_CLOSE_METHOD:
          oss << "server channel error " << server_error_->reply_code <<
            ", message: " << server_error_->reply_text;
          break;
        default:
          oss << "unknown server error, method id 0x"
              << std::uppercase << std::hex
              << std::setw(8) << std::setfill('0')
              << server_error_->method;
          break;
      }
      result = oss.str();
      break;
    }
    case AMQPExceptionType::CUSTOM:
      result = message_;
      break;
  }
  return result;
}

AMQPError::operator bool() const
{
  if (type_ > AMQPExceptionType::NORMAL) {
    return true;
  }

  return false;
}


AMQPExceptionType AMQPError::get_type() const
{
  return type_;
}

const AMQPServerError * AMQPError::get_server_error() const
{
  return server_error_;
}

std::string AMQPError::get_library_error() const
{
  return amqp_error_string2(library_error_);
}

}  // namespace amqp_client
