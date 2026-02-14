/**
 * Drip C++ SDK - Basic compilation and unit tests.
 *
 * Tests type construction and serialization without requiring a live API.
 * For integration tests against a running backend, set DRIP_API_KEY.
 */

#include <drip/drip.hpp>
#include <iostream>
#include <cassert>
#include <string>

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) \
    std::cout << "  " << #name << "... "; \
    try

#define PASS() \
    std::cout << "OK" << std::endl; \
    tests_passed++;

#define FAIL(msg) \
    std::cout << "FAIL: " << msg << std::endl; \
    tests_failed++;

// =============================================================================
// Type construction tests
// =============================================================================

void test_config_defaults() {
    TEST(config_defaults) {
        drip::Config cfg;
        assert(cfg.api_key.empty());
        assert(cfg.base_url.empty());
        assert(cfg.timeout_ms == 30000);
        PASS();
    } catch (const std::exception& e) {
        FAIL(e.what());
    }
}

void test_track_usage_params() {
    TEST(track_usage_params) {
        drip::TrackUsageParams params;
        params.customer_id = "cust_123";
        params.meter = "tokens";
        params.quantity = 1500;
        params.units = "tokens";
        params.description = "Training epoch 1";
        params.metadata["model"] = "transformer";

        assert(params.customer_id == "cust_123");
        assert(params.meter == "tokens");
        assert(params.quantity == 1500);
        assert(params.metadata.size() == 1);
        assert(params.metadata["model"] == "transformer");
        PASS();
    } catch (const std::exception& e) {
        FAIL(e.what());
    }
}

void test_record_run_params() {
    TEST(record_run_params) {
        drip::RecordRunParams run;
        run.customer_id = "cust_456";
        run.workflow = "training-run";
        run.status = drip::RUN_COMPLETED;

        drip::RecordRunEvent epoch;
        epoch.event_type = "training.epoch";
        epoch.quantity = 50;
        epoch.units = "epochs";
        run.events.push_back(epoch);

        drip::RecordRunEvent tokens;
        tokens.event_type = "training.tokens";
        tokens.quantity = 2500000;
        tokens.units = "tokens";
        run.events.push_back(tokens);

        assert(run.events.size() == 2);
        assert(run.events[0].event_type == "training.epoch");
        assert(run.events[1].quantity == 2500000);
        PASS();
    } catch (const std::exception& e) {
        FAIL(e.what());
    }
}

void test_run_status_conversion() {
    TEST(run_status_conversion) {
        assert(std::string(drip::run_status_to_string(drip::RUN_COMPLETED)) == "COMPLETED");
        assert(std::string(drip::run_status_to_string(drip::RUN_FAILED)) == "FAILED");
        assert(std::string(drip::run_status_to_string(drip::RUN_PENDING)) == "PENDING");
        assert(std::string(drip::run_status_to_string(drip::RUN_TIMEOUT)) == "TIMEOUT");

        assert(drip::run_status_from_string("COMPLETED") == drip::RUN_COMPLETED);
        assert(drip::run_status_from_string("FAILED") == drip::RUN_FAILED);
        assert(drip::run_status_from_string("RUNNING") == drip::RUN_RUNNING);
        assert(drip::run_status_from_string("CANCELLED") == drip::RUN_CANCELLED);
        PASS();
    } catch (const std::exception& e) {
        FAIL(e.what());
    }
}

void test_error_types() {
    TEST(error_types) {
        drip::DripError base("test error", 500, "INTERNAL");
        assert(base.status_code() == 500);
        assert(base.code() == "INTERNAL");
        assert(std::string(base.what()) == "test error");

        drip::AuthenticationError auth;
        assert(auth.status_code() == 401);

        drip::NotFoundError nf;
        assert(nf.status_code() == 404);

        drip::RateLimitError rl;
        assert(rl.status_code() == 429);

        drip::TimeoutError te;
        assert(te.status_code() == 408);

        drip::NetworkError ne;
        assert(ne.status_code() == 0);
        PASS();
    } catch (const std::exception& e) {
        FAIL(e.what());
    }
}

void test_client_requires_api_key() {
    TEST(client_requires_api_key) {
        // Unset env var for this test
        bool threw = false;
        try {
            drip::Config cfg;
            cfg.api_key = "";
            drip::Client client(cfg);
        } catch (const drip::DripError& e) {
            threw = true;
            assert(e.code() == "NO_API_KEY");
        }
        assert(threw);
        PASS();
    } catch (const std::exception& e) {
        FAIL(e.what());
    }
}

void test_emit_event_defaults() {
    TEST(emit_event_defaults) {
        drip::EmitEventParams evt;
        assert(evt.quantity == 0);
        assert(evt.cost_units == 0);
        assert(evt.run_id.empty());
        assert(evt.event_type.empty());
        PASS();
    } catch (const std::exception& e) {
        FAIL(e.what());
    }
}

void test_version_defined() {
    TEST(version_defined) {
        assert(DRIP_SDK_VERSION_MAJOR == 0);
        assert(DRIP_SDK_VERSION_MINOR == 1);
        assert(DRIP_SDK_VERSION_PATCH == 0);
        assert(std::string(DRIP_SDK_VERSION) == "0.1.0");
        PASS();
    } catch (const std::exception& e) {
        FAIL(e.what());
    }
}

// =============================================================================
// Main
// =============================================================================

int main() {
    std::cout << "Drip C++ SDK Tests" << std::endl;
    std::cout << "==================" << std::endl;

    test_config_defaults();
    test_track_usage_params();
    test_record_run_params();
    test_run_status_conversion();
    test_error_types();
    test_client_requires_api_key();
    test_emit_event_defaults();
    test_version_defined();

    std::cout << std::endl;
    std::cout << "Results: " << tests_passed << " passed, "
              << tests_failed << " failed" << std::endl;

    return tests_failed > 0 ? 1 : 0;
}
