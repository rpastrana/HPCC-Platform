# Address Cleaner Conditional Tracing

This feature implements conditional tracing for address cleaner operations using Jtrace, where spans are only reported if they meet certain conditions.

## Overview

The `ConditionalSpanScope` class provides a way to track operations that:
1. Take longer than a configurable threshold, OR
2. Fail with an exception or error

This prevents trace pollution from fast, successful operations while ensuring that slow operations and failures are always captured for debugging.

## Usage

### Basic Usage

```cpp
#include "jtrace.hpp"

void someOperation() {
    // Only report spans that take longer than 1ms (1,000,000 ns) or fail
    ConditionalSpanScope span("operation_name", 1000000);
    
    // Do your work here
    doWork();
    
    // Span will be automatically reported if conditions are met
}
```

### Marking Operations as Failed

```cpp
void operationThatMightFail() {
    ConditionalSpanScope span("risky_operation", 500000); // 0.5ms threshold
    
    try {
        doRiskyWork();
    } catch (IException* e) {
        span.recordException(e, true, true);
        throw;
    } catch (...) {
        span.markFailed();
        throw;
    }
}
```

### Address Cleaner Implementation

The address cleaner function `rtlCleanAddress` uses conditional tracing:

```cpp
void rtlCleanAddress(size32_t & __lenResult, char * & __result, size32_t lenSrc, const char * src) {
    ConditionalSpanScope span("addresscleaner", getAddressCleanerThresholdNs());
    
    try {
        // Clean the address
        cleanAddressImpl(result, src);
        
        // Set result
        __lenResult = result.length();
        __result = (char *)rtlMalloc(__lenResult);
        memcpy(__result, result.str(), __lenResult);
    } catch (IException *e) {
        span.recordException(e, true, true);
        throw;
    }
}
```

## Configuration

### Environment Variables

Set the threshold for address cleaner tracing:

```bash
export ADDRESSCLEANER_THRESHOLD_NS=500000  # 0.5ms threshold
```

### Programmatic Configuration

```cpp
#include "jaddresscleaner.hpp"

// Set threshold to 2ms
setAddressCleanerThresholdNs(2000000);

// Get current threshold
stat_type threshold = getAddressCleanerThresholdNs();
```

## Span Reporting Conditions

A span will be reported if ANY of these conditions are true:

1. **Time Threshold Exceeded**: The operation takes longer than the configured threshold
2. **Explicit Failure Marking**: `markFailed()` was called
3. **Exception Recorded**: `recordException()` was called
4. **Error Recorded**: `recordError()` was called
5. **Attributes Set**: `setSpanAttribute()` was called (forces reporting to capture the attribute)

## Benefits

1. **Reduced Trace Noise**: Fast, successful operations don't generate traces
2. **Automatic Failure Capture**: All failures are automatically traced regardless of duration
3. **Performance Insight**: Slow operations are captured for performance analysis
4. **Configurable Thresholds**: Adjust sensitivity based on your use case

## Implementation Details

- Uses `SpanTimeStamp` for high-precision timing
- Creates backdated spans using `createBackdatedInternalSpan()`
- Integrates with existing Jtrace infrastructure
- Thread-safe and minimal overhead for fast operations

## Example Output

```
# Fast operation - no trace generated
Operation elapsed time: 192600 ns
NOT reporting span (elapsed 192600 ns < threshold 1000000 ns)

# Slow operation - trace generated
Operation elapsed time: 5071992 ns
Reporting span (elapsedNs=5071992 >= threshold=1000000)
Creating backdated span 'addresscleaner' with elapsed time: 5071992 ns

# Failed operation - trace generated regardless of time
markFailed() called - will force span reporting
Operation elapsed time: 171039 ns
Reporting span (forceReport=1, elapsedNs=171039 >= threshold=1000000)
```