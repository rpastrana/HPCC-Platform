################################################################################
#    HPCC SYSTEMS software Copyright (C) 2020 HPCC Systems®.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
################################################################################

project( prometheusreporter ) 

#if(PROMETHEUS_CPP_ENABLE_PUSH)
#  add_executable(sample-client sample_client.cc)
#  target_link_libraries(sample-client PRIVATE prometheus-cpp::push)
#endif()

#if(PROMETHEUS_CPP_ENABLE_PULL)
#  add_executable(sample-server sample_server.cc)
#  target_link_libraries(sample-server PRIVATE prometheus-cpp::pull)
#endif()

include_directories (            
         ./../../system/jlib
         ./../../system/include
         ${HPCC_SOURCE_DIR}/metrics/prometheus-cpp/core/include
         ${HPCC_SOURCE_DIR}/metrics/prometheus-cpp/core/include/detail
         ${CMAKE_CURRENT_BINARY_DIR}/metrics/prometheus-cpp/core/include
         /home/ubuntu/GIT/hpcc-build/metrics/prometheus-cpp/core/include
         ${HPCC_SOURCE_DIR}/metrics/prometheus-cpp/pull/include
         ${HPCC_SOURCE_DIR}/metrics/prometheus-cpp/pull/include/detail
         /home/ubuntu/GIT/hpcc-build/metrics/prometheus-cpp/pull/include

    )
message("^^^^^^^^^^^^^^ ${CMAKE_CURRENT_BINARY_DIR}/metrics/prometheus-cpp/core/include/prometheus/detail")
ADD_DEFINITIONS( -DMODULE_PRIORITY=1 -D_CONSOLE )

set ( SRCS PrometheusReporter.cpp )
HPCC_ADD_EXECUTABLE ( PrometheusReporter ${SRCS} )
target_link_libraries ( PrometheusReporter
         jlib
         prometheus-cpp-core
         prometheus-cpp-pull
         )
install ( TARGETS PrometheusReporter RUNTIME DESTINATION ${EXEC_DIR} )