#ifndef DRIP_TYPES_HPP
#define DRIP_TYPES_HPP

#include <string>
#include <vector>
#include <map>
#include <cstdint>

namespace drip {

// =============================================================================
// Configuration
// =============================================================================

/**
 * Configuration for the Drip SDK client.
 *
 * api_key:  Your Drip API key (sk_live_... or pk_live_...).
 *           Falls back to DRIP_API_KEY environment variable.
 * base_url: API base URL. Defaults to production.
 *           Falls back to DRIP_BASE_URL environment variable.
 * timeout_ms: Request timeout in milliseconds. Default: 30000.
 */
struct Config {
    std::string api_key;
    std::string base_url;
    int timeout_ms;

    Config()
        : api_key("")
        , base_url("")
        , timeout_ms(30000)
    {}
};

// =============================================================================
// Key Type
// =============================================================================

enum KeyType {
    KEY_SECRET,   // sk_live_... / sk_test_...
    KEY_PUBLIC,   // pk_live_... / pk_test_...
    KEY_UNKNOWN   // legacy or unrecognized
};

// =============================================================================
// Metadata (JSON-like key-value store)
// =============================================================================

/**
 * Simple string-keyed metadata map.
 * Values are stored as JSON strings. For complex values,
 * serialize them to JSON strings before inserting.
 */
typedef std::map<std::string, std::string> Metadata;

// =============================================================================
// Customer Management
// =============================================================================

struct CreateCustomerParams {
    std::string external_customer_id; // Optional (at least one of this or onchain_address)
    std::string onchain_address;      // Optional (at least one of this or external_customer_id)
    Metadata metadata;                // Optional
};

struct CustomerResult {
    std::string id;
    std::string external_customer_id;
    std::string onchain_address;
    std::string status;              // ACTIVE, LOW_BALANCE, PAUSED
    bool is_internal;
    Metadata metadata;
    std::string created_at;
    std::string updated_at;
};

struct ListCustomersOptions {
    int limit;            // 1-100, default 100
    std::string status;   // Optional: ACTIVE, LOW_BALANCE, PAUSED

    ListCustomersOptions() : limit(100) {}
};

struct ListCustomersResult {
    std::vector<CustomerResult> customers;
    int total;
};

struct BalanceResult {
    std::string customer_id;
    std::string balance_usdc;
};

// =============================================================================
// Health Check
// =============================================================================

struct PingResult {
    bool ok;
    std::string status;
    int latency_ms;
    int64_t timestamp;
};

// =============================================================================
// Usage Tracking
// =============================================================================

struct TrackUsageParams {
    std::string customer_id;     // Required
    std::string meter;           // Required (e.g., "tokens", "api_calls")
    double quantity;             // Required
    std::string idempotency_key; // Optional (auto-generated if empty)
    std::string units;           // Optional (e.g., "tokens", "requests")
    std::string description;     // Optional
    Metadata metadata;           // Optional
};

struct TrackUsageResult {
    bool success;
    std::string usage_event_id;
    std::string customer_id;
    std::string usage_type;
    double quantity;
    bool is_internal;
    std::string message;
};

// =============================================================================
// Run Types (Execution Ledger)
// =============================================================================

enum RunStatus {
    RUN_PENDING,
    RUN_RUNNING,
    RUN_COMPLETED,
    RUN_FAILED,
    RUN_CANCELLED,
    RUN_TIMEOUT
};

/**
 * Convert RunStatus enum to API string.
 */
inline const char* run_status_to_string(RunStatus s) {
    switch (s) {
        case RUN_PENDING:   return "PENDING";
        case RUN_RUNNING:   return "RUNNING";
        case RUN_COMPLETED: return "COMPLETED";
        case RUN_FAILED:    return "FAILED";
        case RUN_CANCELLED: return "CANCELLED";
        case RUN_TIMEOUT:   return "TIMEOUT";
        default:            return "UNKNOWN";
    }
}

inline RunStatus run_status_from_string(const std::string& s) {
    if (s == "PENDING")   return RUN_PENDING;
    if (s == "RUNNING")   return RUN_RUNNING;
    if (s == "COMPLETED") return RUN_COMPLETED;
    if (s == "FAILED")    return RUN_FAILED;
    if (s == "CANCELLED") return RUN_CANCELLED;
    if (s == "TIMEOUT")   return RUN_TIMEOUT;
    return RUN_PENDING;
}

struct StartRunParams {
    std::string customer_id;     // Required
    std::string workflow_id;     // Required
    std::string external_run_id; // Optional
    std::string correlation_id;  // Optional
    std::string parent_run_id;   // Optional
    Metadata metadata;           // Optional
};

struct RunResult {
    std::string id;
    std::string customer_id;
    std::string workflow_id;
    std::string workflow_name;
    RunStatus status;
    std::string correlation_id;
    std::string created_at;
};

struct EndRunParams {
    RunStatus status;            // Required: COMPLETED, FAILED, CANCELLED, TIMEOUT
    std::string error_message;   // Optional
    std::string error_code;      // Optional
    Metadata metadata;           // Optional
};

struct EndRunResult {
    std::string id;
    RunStatus status;
    std::string ended_at;
    int duration_ms;
    int event_count;
    std::string total_cost_units;
};

// =============================================================================
// Event Types
// =============================================================================

struct EmitEventParams {
    std::string run_id;           // Required
    std::string event_type;       // Required (e.g., "training.epoch", "training.tokens")
    double quantity;              // Optional
    std::string units;            // Optional
    std::string description;      // Optional
    double cost_units;            // Optional
    std::string idempotency_key;  // Optional
    Metadata metadata;            // Optional

    EmitEventParams()
        : quantity(0)
        , cost_units(0)
    {}
};

struct EventResult {
    std::string id;
    std::string run_id;
    std::string event_type;
    double quantity;
    double cost_units;
    bool is_duplicate;
    std::string timestamp;
};

// =============================================================================
// RecordRun (all-in-one)
// =============================================================================

struct RecordRunEvent {
    std::string event_type;   // Required
    double quantity;           // Optional
    std::string units;         // Optional
    std::string description;   // Optional
    double cost_units;         // Optional
    Metadata metadata;         // Optional

    RecordRunEvent()
        : quantity(0)
        , cost_units(0)
    {}
};

struct RecordRunParams {
    std::string customer_id;                // Required
    std::string workflow;                   // Required (slug or ID)
    std::vector<RecordRunEvent> events;     // Required
    RunStatus status;                       // Required: COMPLETED, FAILED, etc.
    std::string error_message;              // Optional
    std::string error_code;                 // Optional
    std::string external_run_id;            // Optional
    std::string correlation_id;             // Optional
    Metadata metadata;                      // Optional

    RecordRunParams()
        : status(RUN_COMPLETED)
    {}
};

struct RecordRunResult {
    struct Run {
        std::string id;
        std::string workflow_id;
        std::string workflow_name;
        RunStatus status;
        int duration_ms;
    } run;

    struct Events {
        int created;
        int duplicates;
    } events;

    std::string total_cost_units;
    std::string summary;
};

} // namespace drip

#endif // DRIP_TYPES_HPP
