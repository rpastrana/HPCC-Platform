################################################################################
#    HPCC SYSTEMS software Copyright (C) 2019 HPCC Systems®.
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
# Component: espstore

project( dalisecure )

include(${HPCC_SOURCE_DIR}/esp/scm/espscm.cmake)

include_directories (
    ${HPCC_SOURCE_DIR}/system/include
    ${HPCC_SOURCE_DIR}/system/jlib
    ${HPCC_SOURCE_DIR}/dali/base                    #SDS
    ${HPCC_SOURCE_DIR}/system/mp                    #MP, included by dali base
    ${HPCC_SOURCE_DIR}/system/security/shared/
    ${HPCC_SOURCE_DIR}/esp/services/ws_store/espstorelib
    ${HPCC_SOURCE_DIR}/system/security/cryptohelper
)

ADD_DEFINITIONS ( -D_USRDLL -DDALISECURESTORE_EXPORTS )

set ( SRCS
    ${HPCC_SOURCE_DIR}/esp/services/ws_store/dalisecurelib/daliSecureStore.cpp
)

HPCC_ADD_LIBRARY( dalisecurestorelib SHARED ${SRCS} )

install ( TARGETS dalisecurestorelib RUNTIME DESTINATION ${EXEC_DIR} LIBRARY DESTINATION ${LIB_DIR} )

add_dependencies ( dalisecurestorelib jlib espscm)

target_link_libraries ( dalisecurestorelib
    dalibase
    jlib
)
