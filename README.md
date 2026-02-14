# Drip SDK (C++)

Drip is a C++ SDK for **usage-based tracking and execution logging** in systems where spend is tied to computation -- AI training, inference, APIs, and infra workloads.

This **Core SDK** is optimized for pilots: capture usage and run data first, add billing later.

**One line to start tracking:** `client.trackUsage(params)`

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++11](https://img.shields.io/badge/C%2B%2B-11%2B-blue.svg)](https://isocpp.org/)

---

## 60-Second Quickstart

### 1. Add to your project

**CMake (FetchContent):**

```cmake
include(FetchContent)
FetchContent_Declare(
    drip_sdk
    GIT_REPOSITORY https://github.com/MichaelLevin5908/drip-sdk-cpp.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(drip_sdk)
target_link_libraries(your_target PRIVATE drip::sdk)
```

**Makefile (standalone):**

```bash
git clone https://github.com/MichaelLevin5908/drip-sdk-cpp.git
cd drip-sdk-cpp && make
# Link: -I<path>/include -L<path>/build -ldrip -lcurl
```

### 2. Set your API key

```bash
export DRIP_API_KEY=sk_test_...
```

### 3. Track usage

```cpp
#include <drip/drip.hpp>

int main() {
    drip::Client client;  // reads DRIP_API_KEY from env

    drip::TrackUsageParams params;
    params.customer_id = "cust_123";
    params.meter = "api_calls";
    params.quantity = 1;

    client.trackUsage(params);
    return 0;
}
```

### Full Example

```cpp
#include <drip/drip.hpp>
#include <iostream>

int main() {
    try {
        drip::Client client;

        // Health check
        auto health = client.ping();
        std::cout << "API healthy: " << (health.ok ? "yes" : "no")
                  << " (latency: " << health.latency_ms << "ms)\n";

        // Track usage
        drip::TrackUsageParams usage;
        usage.customer_id = "cust_123";
        usage.meter = "tokens";
        usage.quantity = 1500;
        usage.metadata["model"] = "llama-3";
        client.trackUsage(usage);

        // Record complete execution
        drip::RecordRunParams run;
        run.customer_id = "cust_123";
        run.workflow = "training-run";
        run.status = drip::RUN_COMPLETED;

        drip::RecordRunEvent e1;
        e1.event_type = "training.epoch";
        e1.quantity = 10;
        e1.units = "epochs";
        run.events.push_back(e1);

        drip::RecordRunEvent e2;
        e2.event_type = "training.tokens";
        e2.quantity = 50000;
        e2.units = "tokens";
        run.events.push_back(e2);

        auto result = client.recordRun(run);
        std::cout << result.summary << "\n";

    } catch (const drip::DripError& e) {
        std::cerr << "Error [" << e.status_code() << "]: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
```

---

## Core Concepts

| Concept | Description |
|---------|-------------|
| `customer_id` | The entity you're attributing usage to |
| `meter` | What's being measured (tokens, calls, epochs, etc.) |
| `quantity` | Numeric usage value |
| `run` | A single request or job execution |
| `workflow` | Workflow slug or ID (auto-created if new) |

**Status values:** `RUN_PENDING` | `RUN_RUNNING` | `RUN_COMPLETED` | `RUN_FAILED` | `RUN_CANCELLED` | `RUN_TIMEOUT`

---

## API Reference

| Method | Description |
|--------|-------------|
| `ping()` | Verify API connection, measure latency |
| `trackUsage(params)` | Record metered usage (no billing) |
| `recordRun(params)` | Log complete execution with events (hero method) |
| `startRun(params)` | Start an execution trace |
| `emitEvent(params)` | Log event within a run |
| `endRun(run_id, params)` | Complete execution trace |

---

## Build Options

### CMake (recommended for most projects)

```bash
mkdir build && cd build
cmake .. -DDRIP_BUILD_EXAMPLES=ON -DDRIP_BUILD_TESTS=ON
cmake --build .
ctest --output-on-failure
```

### Makefile (for raw Makefile projects)

```bash
make                  # Build static library (libdrip.a)
make shared           # Build shared library (libdrip.so)
make examples         # Build examples
make health-check     # Build health check binary
make install PREFIX=/usr/local  # Install headers + library
```

The Makefile auto-downloads `nlohmann/json` if not present.

### Linking in your Makefile

```makefile
DRIP_SDK = /path/to/drip-sdk-cpp
CXXFLAGS += -I$(DRIP_SDK)/include -I$(DRIP_SDK)/third_party
LDFLAGS  += -L$(DRIP_SDK)/build -ldrip -lcurl
```

---

## Error Handling

```cpp
#include <drip/drip.hpp>

try {
    auto result = client.trackUsage(params);
} catch (const drip::AuthenticationError& e) {
    // 401 - bad API key
} catch (const drip::RateLimitError& e) {
    // 429 - slow down
} catch (const drip::NotFoundError& e) {
    // 404 - resource not found
} catch (const drip::NetworkError& e) {
    // Connection/DNS failure
} catch (const drip::TimeoutError& e) {
    // Request timed out
} catch (const drip::DripError& e) {
    // Catch-all: e.status_code(), e.code(), e.what()
}
```

---

## Requirements

- C++11 or later
- libcurl (system dependency)
- nlohmann/json (auto-fetched by CMake FetchContent or Makefile)

### Installing libcurl

```bash
# Ubuntu/Debian
sudo apt-get install libcurl4-openssl-dev

# macOS (pre-installed)
# Already available via system frameworks

# Fedora/RHEL
sudo dnf install libcurl-devel
```

---

## Who This Is For

- AI/ML training pipelines (token metering, epoch tracking, GPU compute)
- AI agents (tool calls, execution traces)
- API companies (per-request billing, endpoint attribution)
- RPC providers (multi-chain call tracking)
- Cloud/infra (compute seconds, storage, bandwidth)

---

## License

MIT
