# Zenoh Bridge - Real Integration Guide

## Overview
This document explains how to integrate the real Zenoh C library into the OpenNeuro project.

## Current Status
- ✅ **HAL Abstraction Layer**: Complete (`zenoh_hal.h`)
- ✅ **Mock Implementation**: Working (for development without Zenoh installed)
- 🔄 **Real Zenoh Integration**: Structured but commented out (requires `libzenohc`)

## Installing Zenoh C Library

### Option 1: Build from Source (Recommended)
```bash
# Clone Zenoh C repository
git clone https://github.com/eclipse-zenoh/zenoh-c.git
cd zenoh-c

# Build with CMake
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release

# Install (optional)
sudo cmake --install .
```

### Option 2: Use Pre-built Binaries
Download from: https://github.com/eclipse-zenoh/zenoh-c/releases

## Enabling Real Zenoh in OpenNeuro

### Step 1: Update `zenoh_hal_full.c`
Uncomment the following sections in `src/zenoh_hal_full.c`:
1. `#include <zenoh.h>` at the top
2. All `/* Real API: ... */` blocks
3. Replace placeholder structs with real `z_owned_*` types

### Step 2: Update CMakeLists.txt
In `zenoh-bridge/CMakeLists.txt`, uncomment:
```cmake
find_package(zenohc REQUIRED)
set(ZENOH_LIB zenohc::lib)
```

### Step 3: Rebuild
```bash
cd zenoh-bridge
cmake -S . -B build -DUSE_ZENOH_PICO=OFF
cmake --build build
```

## API Mapping Reference

### Session Management
```c
// HAL API
zenoh_session_t* s = zenoh_hal_open("tcp/192.168.1.1:7447");

// Real Zenoh C API
z_owned_config_t config = z_config_default();
zp_config_insert(z_loan(config), Z_CONFIG_CONNECT_KEY, 
                 z_string_make("tcp/192.168.1.1:7447"));
z_owned_session_t session = z_open(z_move(config));
```

### Publisher
```c
// HAL API
zenoh_publisher_t* pub = zenoh_hal_declare_publisher(s, "robot/sensor/imu");
zenoh_hal_put(pub, data, len);

// Real Zenoh C API
z_owned_keyexpr_t ke = z_keyexpr("robot/sensor/imu");
z_owned_publisher_t pub = z_declare_publisher(z_loan(session), z_loan(ke), NULL);
z_publisher_put(z_loan(pub), data, len, NULL);
```

### Subscriber
```c
// HAL API
zenoh_hal_declare_subscriber(s, "robot/cmd/motor", callback, ctx);

// Real Zenoh C API
void callback(const z_sample_t *sample, void *arg) {
    // Handle data
}
z_owned_closure_sample_t cb = z_closure(callback, NULL, ctx);
z_owned_keyexpr_t ke = z_keyexpr("robot/cmd/motor");
z_owned_subscriber_t sub = z_declare_subscriber(z_loan(session), 
                                                  z_loan(ke), z_move(cb), NULL);
```

## Testing with Real Zenoh

### 1. Start Zenoh Router
```bash
# Download Zenoh router
wget https://github.com/eclipse-zenoh/zenoh/releases/download/0.11.0/zenohd

# Run router
./zenohd
```

### 2. Run OpenNeuro Tests
```bash
./build/test_zenoh_hal
./build/test_qos_mapping
```

### 3. Monitor with Zenoh Tools
```bash
# Subscribe to PTP status
z_sub openneuro/ptp/status

# Publish test data
z_put robot/test/data "Hello Zenoh"
```

## Zenoh-Pico for Embedded

For ESP32/STM32, use the same HAL but link against `zenoh-pico`:
```cmake
cmake -DUSE_ZENOH_PICO=ON ..
```

Zenoh-Pico API is similar but optimized for constrained devices.

## References
- Zenoh C API Docs: https://zenoh-c.readthedocs.io/
- Zenoh GitHub: https://github.com/eclipse-zenoh/zenoh-c
- Examples: https://github.com/eclipse-zenoh/zenoh-c/tree/main/examples
