/**
 * Drip C++ SDK - Health Check
 *
 * Minimal test that verifies the SDK can connect and talk to the API.
 * Designed to run alongside the JS/Python health checks in testdrip.
 *
 * Build:
 *   g++ -std=c++11 -I../include -I../third_party -o health_check \
 *       health_check.cpp ../src/client.cpp -lcurl
 *
 * Run:
 *   export DRIP_API_KEY="sk_live_..."
 *   ./health_check
 */

#include <drip/drip.hpp>
#include <iostream>
#include <string>

int main() {
    std::cout << "Drip C++ SDK Health Check v" << DRIP_SDK_VERSION << std::endl;
    std::cout << "==========================================" << std::endl;

    try {
        drip::Client client;
        std::cout << "[1/3] Client initialized (key type: "
                  << (client.key_type() == drip::KEY_SECRET ? "secret" :
                      client.key_type() == drip::KEY_PUBLIC ? "public" : "unknown")
                  << ")" << std::endl;

        // Test 1: Ping
        drip::PingResult health = client.ping();
        if (health.ok) {
            std::cout << "[2/3] Ping OK (" << health.latency_ms << "ms)" << std::endl;
        } else {
            std::cerr << "[2/3] Ping FAILED: " << health.status << std::endl;
            return 1;
        }

        // Test 2: Track a test usage event
        drip::TrackUsageParams usage;
        usage.customer_id = "cust_health_check";
        usage.meter = "sdk_health_check";
        usage.quantity = 1;
        usage.units = "checks";
        usage.description = "C++ SDK health check";
        usage.metadata["sdk_version"] = DRIP_SDK_VERSION;
        usage.metadata["language"] = "cpp";

        drip::TrackUsageResult result = client.trackUsage(usage);
        if (result.success) {
            std::cout << "[3/3] trackUsage OK (event: " << result.usage_event_id << ")" << std::endl;
        } else {
            std::cerr << "[3/3] trackUsage FAILED" << std::endl;
            return 1;
        }

        std::cout << "==========================================" << std::endl;
        std::cout << "All checks passed." << std::endl;
        return 0;

    } catch (const drip::AuthenticationError& e) {
        std::cerr << "FAIL: Authentication error - " << e.what() << std::endl;
        std::cerr << "Set DRIP_API_KEY environment variable." << std::endl;
        return 1;
    } catch (const drip::DripError& e) {
        std::cerr << "FAIL: API error [" << e.status_code() << "] - " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << std::endl;
        return 1;
    }
}
