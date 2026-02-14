#ifndef DRIP_CLIENT_HPP
#define DRIP_CLIENT_HPP

#include "types.hpp"
#include "errors.hpp"

#include <string>
#include <memory>

namespace drip {

/**
 * Drip SDK Client - C++ interface for Drip's REST API.
 *
 * Core methods:
 *   - ping()        - Health check and latency measurement
 *   - trackUsage()  - Record usage without billing
 *   - recordRun()   - Record a complete execution with events
 *   - startRun()    - Start a run for incremental event emission
 *   - emitEvent()   - Emit a single event to a run
 *   - endRun()      - Complete a run
 *
 * Example:
 *   drip::Config cfg;
 *   cfg.api_key = "sk_live_abc123";
 *   drip::Client client(cfg);
 *
 *   auto health = client.ping();
 *   if (health.ok) {
 *       drip::TrackUsageParams params;
 *       params.customer_id = "cust_123";
 *       params.meter = "tokens";
 *       params.quantity = 1500;
 *       client.trackUsage(params);
 *   }
 */
class Client {
public:
    /**
     * Construct a Drip client.
     *
     * If config.api_key is empty, reads from DRIP_API_KEY env var.
     * If config.base_url is empty, reads from DRIP_BASE_URL env var,
     * then falls back to production default.
     *
     * @throws DripError if no API key is available.
     */
    explicit Client(const Config& config = Config());

    ~Client();

    // Non-copyable, movable
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
    Client(Client&& other);
    Client& operator=(Client&& other);

    /** The detected key type (secret, public, unknown). */
    KeyType key_type() const;

    // =========================================================================
    // Health Check
    // =========================================================================

    /**
     * Ping the Drip API. Returns health status and latency.
     *
     * @throws NetworkError on connection failure.
     * @throws TimeoutError if the request exceeds timeout_ms.
     */
    PingResult ping();

    // =========================================================================
    // Usage Tracking (No Billing)
    // =========================================================================

    /**
     * Record usage for tracking WITHOUT billing.
     *
     * Use this for pilot programs, internal tracking, or pre-billing.
     * For actual billing, use charge() from the full SDK.
     */
    TrackUsageResult trackUsage(const TrackUsageParams& params);

    // =========================================================================
    // Run & Event Methods (Execution Ledger)
    // =========================================================================

    /**
     * Start a new run for tracking execution.
     * Use emitEvent() to add events, then endRun() to complete.
     */
    RunResult startRun(const StartRunParams& params);

    /**
     * End a run with a final status.
     */
    EndRunResult endRun(const std::string& run_id, const EndRunParams& params);

    /**
     * Emit an event to a running run.
     */
    EventResult emitEvent(const EmitEventParams& params);

    /**
     * Record a complete run in a single call.
     *
     * This is the primary method for most integrations. It:
     * 1. Finds or creates the workflow
     * 2. Creates the run
     * 3. Emits all events
     * 4. Ends the run
     *
     * Example:
     *   RecordRunParams params;
     *   params.customer_id = "cust_123";
     *   params.workflow = "training-run";
     *   params.status = RUN_COMPLETED;
     *
     *   RecordRunEvent e1;
     *   e1.event_type = "training.epoch";
     *   e1.quantity = 100;
     *   e1.units = "epochs";
     *   params.events.push_back(e1);
     *
     *   auto result = client.recordRun(params);
     */
    RecordRunResult recordRun(const RecordRunParams& params);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace drip

#endif // DRIP_CLIENT_HPP
