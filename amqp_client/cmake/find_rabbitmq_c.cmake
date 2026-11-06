# Copyright 2026 Chen Bainian
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# Find RabbitMQ C library
#

macro(find_rabbitmq_c)

  # Try using the CMake config modules first
  find_package(rabbitmq-c CONFIG QUIET)
  if(rabbitmq-c_FOUND)
      set(Rabbitmqc_LIBRARY rabbitmq::rabbitmq)
      get_target_property(Rabbitmqc_INCLUDE_DIRS ${Rabbitmqc_LIBRARY} INTERFACE_INCLUDE_DIRECTORIES)
  else()
    # Find the Rabbitmq C library
    find_path(Rabbitmqc_INCLUDE_DIR
      NAMES amqp.h
      HINTS ${Rabbitmqc_DIR}/include
    )

    find_library(Rabbitmqc_LIBRARY
      NAMES rabbitmq
      HINTS ${Rabbitmqc_DIR}/lib
    )

    set(Rabbitmqc_PROCESS_INCLUDES Rabbitmqc_INCLUDE_DIR)
    set(Rabbitmqc_PROCESS_LIBS Rabbitmqc_LIBRARY)

    _amqp_libfind_process(Rabbitmqc)

    find_file(_Rabbitmqc_SSL_HEADER
      NAMES amqp_ssl_socket.h
      PATHS ${Rabbitmqc_INCLUDE_DIR}
      NO_DEFAULT_PATH
    )

    string(COMPARE NOTEQUAL "${_Rabbitmqc_SSL_HEADER}"
      "_Rabbitmqc_SSL_HEADER-NOTFOUND" _rmqc_ssl_enabled)

    set(Rabbitmqc_SSL_ENABLED ${_rmqc_ssl_enabled} CACHE BOOL
      "Rabbitmqc is SSL Enabled" FORCE)
    mark_as_advanced(_Rabbitmqc_SSL_HEADER Rabbitmqc_SSL_ENABLED)
    include_directories(SYSTEM ${Rabbitmqc_INCLUDE_DIRS})
  endif()
endmacro()

macro(_amqp_libfind_process PREFIX)
  # Skip processing if already processed during this run
  if(NOT ${PREFIX}_FOUND)
    # Start with the assumption that the library was found
    set(${PREFIX}_FOUND TRUE)

    # Process all includes and set _FOUND to false if any are missing
    foreach(i ${${PREFIX}_PROCESS_INCLUDES})
      if(${i})
        set(${PREFIX}_INCLUDE_DIRS ${${PREFIX}_INCLUDE_DIRS} ${${i}})
        mark_as_advanced(${i})
      else()
        set(${PREFIX}_FOUND FALSE)
      endif()
    endforeach()

    # Process all libraries and set _FOUND to false if any are missing
    foreach(i ${${PREFIX}_PROCESS_LIBS})
      if(${i})
        set(${PREFIX}_LIBRARIES ${${PREFIX}_LIBRARIES} ${${i}})
        mark_as_advanced(${i})
      else()
        set(${PREFIX}_FOUND FALSE)
      endif()
    endforeach()

    # Print message and/or exit on fatal error
    if(${PREFIX}_FOUND)
      if(NOT ${PREFIX}_FIND_QUIETLY)
        message(STATUS "Found ${PREFIX} ${${PREFIX}_VERSION}")
      endif()
    else()
      if(${PREFIX}_FIND_REQUIRED)
        foreach(i ${${PREFIX}_PROCESS_INCLUDES} ${${PREFIX}_PROCESS_LIBS})
          message("${i}=${${i}}")
        endforeach()
        message(FATAL_ERROR "Required library ${PREFIX} NOT FOUND.\n\
          Install the library (dev version) and try again. \
          If the library is already installed, \
          use cmake to set the missing variables manually.")
      endif()
    endif()
  endif()
endmacro()
