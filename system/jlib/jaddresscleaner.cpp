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

#include "jaddresscleaner.hpp"
#include "jexcept.hpp"
#include "jstring.hpp"
#include "eclrtl.hpp"
#include <regex>

// Default threshold: 1 millisecond (1,000,000 nanoseconds)
static stat_type addressCleanerThresholdNs = 1000000;

void setAddressCleanerThresholdNs(stat_type thresholdNs)
{
    addressCleanerThresholdNs = thresholdNs;
}

stat_type getAddressCleanerThresholdNs()
{
    return addressCleanerThresholdNs;
}

// Helper function to actually clean the address
static void cleanAddressImpl(StringBuffer &result, const char *src)
{
    if (!src || !*src)
    {
        result.clear();
        return;
    }
    
    std::string input(src);
    std::string cleaned = input;
    
    // Simple address cleaning - remove extra commas, normalize spaces
    // This is a simplified implementation for demonstration
    try
    {
        // Replace multiple consecutive commas with single comma
        cleaned = std::regex_replace(cleaned, std::regex(",+"), ",");
        
        // Remove leading/trailing commas
        cleaned = std::regex_replace(cleaned, std::regex("^,+|,+$"), "");
        
        // Replace multiple spaces with single space
        cleaned = std::regex_replace(cleaned, std::regex(" +"), " ");
        
        // Trim leading/trailing spaces
        cleaned = std::regex_replace(cleaned, std::regex("^ +| +$"), "");
        
        result.clear().append(cleaned.c_str());
    }
    catch (const std::exception &e)
    {
        // If regex fails, throw an exception to test error tracing
        throw MakeStringException(0, "Address cleaning failed: %s", e.what());
    }
}

void rtlCleanAddress(size32_t & __lenResult, char * & __result, size32_t lenSrc, const char * src)
{
    // Create conditional span that only reports if:
    // 1. Operation takes longer than configured threshold, OR
    // 2. Operation fails with an exception
    ConditionalSpanScope span("addresscleaner", getAddressCleanerThresholdNs());
    
    try
    {
        StringBuffer result;
        
        // Add some attributes to help with debugging (forces span reporting)
        if (lenSrc > 1000) // Only add attributes for large inputs
        {
            span.setSpanAttribute("input_length", (unsigned __int64)lenSrc);
        }
        
        cleanAddressImpl(result, src);
        
        __lenResult = result.length();
        __result = (char *)rtlMalloc(__lenResult);
        if (__lenResult > 0)
            memcpy(__result, result.str(), __lenResult);
    }
    catch (IException *e)
    {
        span.recordException(e, true, true);
        throw;
    }
    catch (...)
    {
        span.markFailed();
        throw;
    }
}