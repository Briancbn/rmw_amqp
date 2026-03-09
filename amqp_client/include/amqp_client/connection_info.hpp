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

#ifndef AMQP_CLIENT__CONNECTION_INFO_HPP_
#define AMQP_CLIENT__CONNECTION_INFO_HPP_

#include <memory>
#include <string>

#include "amqp_client/sasl_config.hpp"
#include "amqp_client/macros.hpp"

namespace amqp_client
{

class ConnectionInfo : public std::enable_shared_from_this<ConnectionInfo> {
public:
  using Ptr = std::shared_ptr<ConnectionInfo>;
  using ConstPtr = std::shared_ptr<const ConnectionInfo>;

  ConnectionInfo();

  virtual ~ConnectionInfo();

  ConnectionInfo::Ptr username(const std::string & username);
  const std::string & username() const;

  ConnectionInfo::Ptr password(const std::string & password);
  const std::string & password() const;

  ConnectionInfo::Ptr identity(const std::string & identity);
  const std::string & identity() const;

  ConnectionInfo::Ptr host(const std::string & host);
  const std::string & host() const;

  ConnectionInfo::Ptr vhost(const std::string & vhost);
  const std::string & vhost() const;

  ConnectionInfo::Ptr port(int port);
  int port() const;

  ConnectionInfo::Ptr ssl(bool ssl);
  bool ssl() const;

  ConnectionInfo::Ptr channel_max(int channel_max);
  int channel_max() const;

  ConnectionInfo::Ptr frame_max(int frame_max);
  int frame_max() const;

  ConnectionInfo::Ptr heartbeat(int heartbeat);
  int heartbeat() const;

  ConnectionInfo::Ptr sasl_external(bool sasl_external);
  bool sasl_external() const;

  const SASLConfig & sasl_config() const;

private:
  AMQP_CLIENT_DISABLE_COPY(ConnectionInfo)

  std::string host_;
  std::string vhost_;
  int port_;
  bool ssl_;

  int channel_max_;
  int frame_max_;
  int heartbeat_;

  SASLConfig sasl_config_;
};

}  // namespace amqp_client

#endif  // AMQP_CLIENT__CONNECTION_INFO_HPP_
