cmake_minimum_required(VERSION 3.14)

project(drip-sdk-cpp
    VERSION 0.1.0
    DESCRIPTION "Official C++ SDK for Drip - usage-based billing infrastructure"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(DRIP_BUILD_EXAMPLES "Build example programs" OFF)
option(DRIP_BUILD_TESTS "Build tests" OFF)
option(DRIP_VENDORED_JSON "Use vendored nlohmann/json instead of FetchContent" OFF)

# =============================================================================
# Dependencies
# =============================================================================

# libcurl - required for HTTP
find_package(CURL REQUIRED)

# nlohmann/json - header-only JSON library
if(DRIP_VENDORED_JSON)
    # Use a local copy at third_party/nlohmann/json.hpp
    add_library(nlohmann_json INTERFACE)
    target_include_directories(nlohmann_json INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party
    )
else()
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.3
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# =============================================================================
# Library
# =============================================================================

add_library(drip_sdk
    src/client.cpp
)

add_library(drip::sdk ALIAS drip_sdk)

target_include_directories(drip_sdk
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(drip_sdk
    PRIVATE
        nlohmann_json::nlohmann_json
        CURL::libcurl
)

set_target_properties(drip_sdk PROPERTIES
    OUTPUT_NAME "drip"
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
)

# =============================================================================
# Examples
# =============================================================================

if(DRIP_BUILD_EXAMPLES)
    add_executable(drip_basic_example examples/basic_usage.cpp)
    target_link_libraries(drip_basic_example PRIVATE drip_sdk)
endif()

# =============================================================================
# Tests
# =============================================================================

if(DRIP_BUILD_TESTS)
    enable_testing()
    add_executable(drip_tests tests/test_client.cpp)
    target_link_libraries(drip_tests PRIVATE drip_sdk)
    add_test(NAME drip_sdk_tests COMMAND drip_tests)
endif()

# =============================================================================
# Install (only when built standalone, not as a subdirectory)
# =============================================================================

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    include(GNUInstallDirs)

    install(TARGETS drip_sdk
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    install(DIRECTORY include/drip
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif()

# =============================================================================
# Makefile generation helper (for non-CMake projects like glades-ml)
# =============================================================================
#
# If your project uses raw Makefiles instead of CMake, you can:
#
# 1. Build this SDK as a static/shared library:
#    mkdir build && cd build
#    cmake .. -DCMAKE_BUILD_TYPE=Release
#    make
#
# 2. Then link against it in your Makefile:
#    CXXFLAGS += -I/path/to/sdk-cpp/include
#    LDFLAGS  += -L/path/to/sdk-cpp/build -ldrip -lcurl
#
# Or use the provided Makefile wrapper (see Makefile in this directory).
# =============================================================================
