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

# Component: metrics

#####################################################
# Description:
# ------------
#    Cmake Input File for prometheus-cpp metrics
#    support libraries
#####################################################

project( metrics )

if(USE_METRICS)

    include(ExternalProject)

    ExternalProject_Add(
        prometheus-cpp-build
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/prometheus-cpp
        BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/prometheus-cpp
        CMAKE_ARGS -DOVERRIDE_CXX_STANDARD_FLAGS=OFF -DENABLE_TESTING=OFF -DENABLE_PUSH=OFF -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_RPATH=${LIB_PATH} -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON
        BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} LDFLAGS=-Wl all
        INSTALL_COMMAND "")

    add_library(prometheus-cpp-pull SHARED IMPORTED GLOBAL)
    set_property(TARGET prometheus-cpp-pull
        PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/prometheus-cpp/lib/libprometheus-cpp-pull${CMAKE_SHARED_LIBRARY_SUFFIX} )
    add_library(prometheus-cpp-core SHARED IMPORTED GLOBAL)
    set_property(TARGET prometheus-cpp-core
        PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/prometheus-cpp/lib/libprometheus-cpp-core${CMAKE_SHARED_LIBRARY_SUFFIX} )

    if(NOT PLUGIN)
        install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/prometheus-cpp/lib/libprometheus-cpp-pull${CMAKE_SHARED_LIBRARY_SUFFIX}
                DESTINATION ${LIB_DIR})
        install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/prometheus-cpp/lib/libprometheus-cpp-pull${CMAKE_SHARED_LIBRARY_SUFFIX}.0.9
                DESTINATION ${LIB_DIR})
        install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/prometheus-cpp/lib/libprometheus-cpp-pull${CMAKE_SHARED_LIBRARY_SUFFIX}.0.9.0
                DESTINATION ${LIB_DIR})
        install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/prometheus-cpp/lib/libprometheus-cpp-core${CMAKE_SHARED_LIBRARY_SUFFIX}
                DESTINATION ${LIB_DIR})
        install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/prometheus-cpp/lib/libprometheus-cpp-core${CMAKE_SHARED_LIBRARY_SUFFIX}.0.9
                DESTINATION ${LIB_DIR})
        install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/prometheus-cpp/lib/libprometheus-cpp-core${CMAKE_SHARED_LIBRARY_SUFFIX}.0.9.0
                DESTINATION ${LIB_DIR})
        #I do not think this is needed...
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/prometheus-cpp/LICENSE
            RENAME prometheus-cpp-LICENSE.txt
            DESTINATION ${LIB_DIR})
    endif(NOT PLUGIN)

    HPCC_ADD_SUBDIRECTORY(prometheus-reporter)
endif()
