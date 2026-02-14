# Contributing to Drip C++ SDK

## Development Setup

### Prerequisites

- C++11 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- libcurl development headers
- CMake 3.14+ (for CMake builds)
- Make (for Makefile builds)

### Build

```bash
# CMake
mkdir build && cd build
cmake .. -DDRIP_BUILD_EXAMPLES=ON -DDRIP_BUILD_TESTS=ON
cmake --build .

# Or Makefile
make
```

### Run Tests

```bash
# CMake
cd build && ctest --output-on-failure

# Or Makefile
make health-check
DRIP_API_KEY=sk_test_... ./build/health_check
```

## Code Style

- C++11 standard (no C++14/17 features)
- `drip::` namespace for all public API
- snake_case for struct fields and local variables
- camelCase for method names (matching API convention)
- PIMPL pattern for implementation hiding
- Header guards: `#ifndef DRIP_<NAME>_HPP`

## Project Structure

```
include/drip/   # Public headers
src/            # Implementation files
examples/       # Usage examples
tests/          # Unit tests
scripts/        # Build and release scripts
```

## Submitting Changes

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Ensure tests pass
5. Submit a pull request
