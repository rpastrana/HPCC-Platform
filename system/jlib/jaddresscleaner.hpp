/*##############################################################################

    HPCC SYSTEMS software Copyright (C) 2024 HPCC Systems®.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
############################################################################## */

#ifndef JADDRESSCLEANER_HPP
#define JADDRESSCLEANER_HPP

#include "jlib.hpp"
#include "jtrace.hpp"

// Configuration for address cleaner tracing
extern jlib_decl stat_type getAddressCleanerThresholdNs();
extern jlib_decl void setAddressCleanerThresholdNs(stat_type thresholdNs);

// Address cleaning function with conditional tracing
extern jlib_decl void rtlCleanAddress(size32_t & __lenResult, char * & __result, size32_t lenSrc, const char * src);

#endif