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

#ifndef AMQP_CLIENT__AMQP_ERROR_HPP_
#define AMQP_CLIENT__AMQP_ERROR_HPP_

#include <string>
#include <vector>

typedef struct amqp_rpc_reply_t_ amqp_rpc_reply_t;

namespace amqp_client
{

enum class AMQPExceptionType
{
  NORMAL = 0,
  NONE,
  LIBRARY,
  SERVER
};

struct AMQPServerError
{
  uint32_t method;
  uint16_t reply_code;
  std::string reply_text;
  uint16_t class_id;
  uint16_t method_id;
};

class AMQPError
{
public:
  AMQPError();
  virtual ~AMQPError();

  static bool from_amqp_rpc_reply(
    const amqp_rpc_reply_t * reply,
    AMQPError * error
  );

  std::string str() const;

  /// boolean operator for the AMQPError class
  /**
   * \return true if there is an error
   */
  operator bool() const;

  AMQPExceptionType get_type() const;
  const AMQPServerError * get_server_error() const;
  std::string get_library_error() const;

private:
  AMQPExceptionType type_;
  AMQPServerError * server_error_;
  int library_error_;
};

}  // namespace amqp_client

#endif  // AMQP_CLIENT__AMQP_ERROR_HPP_
