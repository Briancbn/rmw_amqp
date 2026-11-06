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

#include "amqp_client/rabbitmq_c_api.hpp"
#include "amqp_client/connection_info.hpp"

namespace amqp_client
{

ConnectionInfo::ConnectionInfo()
: host_("localhost"),
  vhost_(AMQP_DEFAULT_VHOST),
  port_(5672),
  ssl_(false),
  channel_max_(AMQP_DEFAULT_MAX_CHANNELS),
  frame_max_(AMQP_DEFAULT_FRAME_SIZE),
  heartbeat_(AMQP_DEFAULT_HEARTBEAT)
{
  sasl_config_.external = false;
  sasl_config_.username = "guest";
  sasl_config_.password = "guest";
  sasl_config_.identity = "";
}

ConnectionInfo::~ConnectionInfo()
{
}

ConnectionInfo::Ptr ConnectionInfo::username(const std::string & username)
{
  sasl_config_.username = username;
  return shared_from_this();
}

const std::string & ConnectionInfo::username() const
{
  return sasl_config_.username;
}

ConnectionInfo::Ptr ConnectionInfo::password(const std::string & password)
{
  sasl_config_.password = password;
  return shared_from_this();
}

const std::string & ConnectionInfo::password() const
{
  return sasl_config_.password;
}

ConnectionInfo::Ptr ConnectionInfo::host(const std::string & host)
{
  host_ = host;
  return shared_from_this();
}

const std::string & ConnectionInfo::host() const
{
  return host_;
}

ConnectionInfo::Ptr ConnectionInfo::vhost(const std::string & vhost)
{
  vhost_ = vhost;
  return shared_from_this();
}

const std::string & ConnectionInfo::vhost() const
{
  return vhost_;
}

ConnectionInfo::Ptr ConnectionInfo::port(int port)
{
  port_ = port;
  return shared_from_this();
}

int ConnectionInfo::port() const
{
  return port_;
}

ConnectionInfo::Ptr ConnectionInfo::ssl(bool ssl)
{
  ssl_ = ssl;
  return shared_from_this();
}

bool ConnectionInfo::ssl() const
{
  return ssl_;
}

ConnectionInfo::Ptr ConnectionInfo::channel_max(int channel_max)
{
  channel_max_ = channel_max;
  return shared_from_this();
}

int ConnectionInfo::channel_max() const
{
  return channel_max_;
}

ConnectionInfo::Ptr ConnectionInfo::frame_max(int frame_max)
{
  frame_max_ = frame_max;
  return shared_from_this();
}

int ConnectionInfo::frame_max() const
{
  return frame_max_;
}

ConnectionInfo::Ptr ConnectionInfo::heartbeat(int heartbeat)
{
  heartbeat_ = heartbeat;
  return shared_from_this();
}

int ConnectionInfo::heartbeat() const
{
  return heartbeat_;
}

ConnectionInfo::Ptr ConnectionInfo::sasl_external(bool sasl_external)
{
  sasl_config_.external = sasl_external;
  return shared_from_this();
}

bool ConnectionInfo::sasl_external() const
{
  return sasl_config_.external;
}

const SASLConfig & ConnectionInfo::sasl_config() const
{
  return sasl_config_;
}

}  // namespace amqp_client
