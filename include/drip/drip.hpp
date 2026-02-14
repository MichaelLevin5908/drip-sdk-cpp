#ifndef DRIP_HPP
#define DRIP_HPP

/**
 * Drip C++ SDK - Official client library for Drip billing infrastructure.
 *
 * Include this single header to get the full SDK:
 *
 *   #include <drip/drip.hpp>
 *
 *   int main() {
 *       drip::Config cfg;
 *       cfg.api_key = "sk_live_abc123";
 *       drip::Client client(cfg);
 *
 *       // Health check
 *       auto health = client.ping();
 *
 *       // Track usage (no billing)
 *       drip::TrackUsageParams usage;
 *       usage.customer_id = "cust_123";
 *       usage.meter = "tokens";
 *       usage.quantity = 1500;
 *       client.trackUsage(usage);
 *
 *       // Record a complete run with events
 *       drip::RecordRunParams run;
 *       run.customer_id = "cust_123";
 *       run.workflow = "training-run";
 *       run.status = drip::RUN_COMPLETED;
 *
 *       drip::RecordRunEvent epoch;
 *       epoch.event_type = "training.epoch";
 *       epoch.quantity = 50;
 *       epoch.units = "epochs";
 *       run.events.push_back(epoch);
 *
 *       auto result = client.recordRun(run);
 *   }
 *
 * Dependencies:
 *   - libcurl (linked at build time)
 *   - nlohmann/json (header-only, fetched automatically by CMake/Make)
 *
 * Minimum C++ standard: C++11
 */

#include "types.hpp"
#include "errors.hpp"
#include "client.hpp"

/**
 * Version information
 */
#define DRIP_SDK_VERSION_MAJOR 0
#define DRIP_SDK_VERSION_MINOR 1
#define DRIP_SDK_VERSION_PATCH 0
#define DRIP_SDK_VERSION "0.1.0"

#endif // DRIP_HPP
