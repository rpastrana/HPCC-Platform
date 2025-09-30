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
#include "jlib.hpp"
#include "jtrace.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main()
{
    // Initialize trace manager for testing
    initTraceManager("test", nullptr, nullptr);
    
    std::cout << "Testing address cleaner with conditional tracing..." << std::endl;
    
    // Test 1: Fast operation (should not generate span)
    {
        const char* input1 = "123 Main St,, Apt 4,,, City";
        size32_t lenResult;
        char* result;
        
        std::cout << "Test 1 - Fast operation: " << input1 << std::endl;
        rtlCleanAddress(lenResult, result, strlen(input1), input1);
        std::cout << "Result: " << std::string(result, lenResult) << std::endl;
        rtlFree(result);
    }
    
    // Test 2: Simulated slow operation by using long input (should generate span due to attribute)
    {
        std::string longInput(2000, 'X'); // 2000 character string
        longInput += ", Main St,, Apt 4,,, City";
        
        size32_t lenResult;
        char* result;
        
        std::cout << "\nTest 2 - Long input (forces span due to attribute): " << longInput.substr(0, 50) << "..." << std::endl;
        rtlCleanAddress(lenResult, result, longInput.length(), longInput.c_str());
        std::cout << "Result length: " << lenResult << std::endl;
        rtlFree(result);
    }
    
    // Test 3: Operation with invalid regex (should generate span due to exception)
    {
        const char* input3 = "123 Main St";
        size32_t lenResult;
        char* result;
        
        std::cout << "\nTest 3 - Normal operation: " << input3 << std::endl;
        try
        {
            rtlCleanAddress(lenResult, result, strlen(input3), input3);
            std::cout << "Result: " << std::string(result, lenResult) << std::endl;
            rtlFree(result);
        }
        catch (IException* e)
        {
            std::cout << "Exception caught: " << e->errorMessage() << std::endl;
            e->Release();
        }
    }
    
    std::cout << "\nTesting complete. Check trace output for conditional spans." << std::endl;
    return 0;
}