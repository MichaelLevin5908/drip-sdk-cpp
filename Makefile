# =============================================================================
# Drip C++ SDK - Makefile (for non-CMake projects)
# =============================================================================
#
# This Makefile builds the SDK as a static library (.a) that can be
# linked into projects that use raw Makefiles (like glades-ml).
#
# Usage:
#   make                    # Build static library
#   make shared             # Build shared library (.so)
#   make examples           # Build examples
#   make clean              # Clean build artifacts
#   make install PREFIX=/usr/local  # Install headers and library
#
# Prerequisites:
#   - libcurl development headers (apt: libcurl4-openssl-dev)
#   - nlohmann/json header (auto-downloaded or place at third_party/nlohmann/json.hpp)
#
# =============================================================================

CXX      ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -O2
AR       ?= ar
PREFIX   ?= /usr/local

# Directories
SRC_DIR     = src
INCLUDE_DIR = include
BUILD_DIR   = build
THIRD_PARTY = third_party

# Sources
SOURCES = $(SRC_DIR)/client.cpp
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

# Library outputs
STATIC_LIB = $(BUILD_DIR)/libdrip.a
SHARED_LIB = $(BUILD_DIR)/libdrip.so

# Include paths
INCLUDES = -I$(INCLUDE_DIR) -I$(THIRD_PARTY)

# Linker flags for consumers
LIBS = -lcurl

# Check for nlohmann/json
JSON_HEADER = $(THIRD_PARTY)/nlohmann/json.hpp

# =============================================================================
# Targets
# =============================================================================

.PHONY: all shared examples health-check clean install fetch-json

all: fetch-json $(STATIC_LIB)

shared: fetch-json $(SHARED_LIB)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Static library
$(STATIC_LIB): $(OBJECTS)
	$(AR) rcs $@ $^

# Shared library
$(SHARED_LIB): CXXFLAGS += -fPIC
$(SHARED_LIB): $(OBJECTS)
	$(CXX) -shared -o $@ $^ $(LIBS)

# Object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR) fetch-json
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

# Fetch nlohmann/json if not present
fetch-json:
	@if [ ! -f "$(JSON_HEADER)" ]; then \
		echo "Downloading nlohmann/json..."; \
		mkdir -p $(THIRD_PARTY)/nlohmann; \
		curl -sL https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp \
			-o $(JSON_HEADER); \
		echo "Downloaded to $(JSON_HEADER)"; \
	fi

# Examples
examples: all
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BUILD_DIR)/basic_usage \
		examples/basic_usage.cpp -L$(BUILD_DIR) -ldrip $(LIBS)

# Health check (for testdrip)
health-check: all
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BUILD_DIR)/health_check \
		examples/health_check.cpp -L$(BUILD_DIR) -ldrip $(LIBS)
	@echo "Run: DRIP_API_KEY=sk_live_... ./build/health_check"

# Install
install: all
	mkdir -p $(PREFIX)/include/drip
	mkdir -p $(PREFIX)/lib
	cp $(INCLUDE_DIR)/drip/*.hpp $(PREFIX)/include/drip/
	cp $(STATIC_LIB) $(PREFIX)/lib/

# Clean
clean:
	rm -rf $(BUILD_DIR)
