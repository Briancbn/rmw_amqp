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

#ifndef AMQP_CLIENT__TRANPORT_HPP_
#define AMQP_CLIENT__TRANPORT_HPP_

#include <memory>
#include <string>

#include "amqp_client/macros.hpp"
#include "amqp_client/connection_info.hpp"
#include "amqp_client/amqp_error.hpp"

namespace amqp_client
{

class Connection;

class Transport : public std::enable_shared_from_this<Transport>
{
public:
  Transport() = default;
  virtual ~Transport() = default;
  std::shared_ptr<Connection> create_connection(
    AMQPError * error = nullptr
  ) {
    return create_connection(
      std::make_shared<ConnectionInfo>(), error
    );
  }

  virtual std::shared_ptr<Connection> create_connection(
    const ConnectionInfo::ConstPtr & info,
    AMQPError * error = nullptr
  ) = 0;

  // TODO(anyone): add support for SSL

  static std::shared_ptr<Transport> create_default();

private:
  AMQP_CLIENT_DISABLE_COPY(Transport)
};

}  // namespace amqp_client

#endif  // AMQP_CLIENT__TRANSPORT_HPP_
