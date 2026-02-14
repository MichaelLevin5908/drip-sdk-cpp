/**
 * Drip C++ SDK - Basic Usage Example
 *
 * Demonstrates:
 *   1. Health check (ping)
 *   2. Usage tracking (trackUsage)
 *   3. Recording a training run with events (recordRun)
 *   4. Incremental run management (startRun + emitEvent + endRun)
 *
 * Build:
 *   # With CMake:
 *   cmake -B build -DDRIP_BUILD_EXAMPLES=ON && cmake --build build
 *   ./build/drip_basic_example
 *
 *   # With Make:
 *   make examples
 *   ./build/basic_usage
 *
 * Set environment variables before running:
 *   export DRIP_API_KEY="sk_live_your_key_here"
 *   export DRIP_BASE_URL="https://drip-app-hlunj.ondigitalocean.app/v1"  # optional
 */

#include <drip/drip.hpp>
#include <iostream>
#include <string>
#include <cstdlib>

int main() {
    try {
        // =====================================================================
        // Initialize client (reads DRIP_API_KEY from environment)
        // =====================================================================
        drip::Client client;

        std::cout << "Drip C++ SDK v" << DRIP_SDK_VERSION << std::endl;
        std::cout << "Key type: "
                  << (client.key_type() == drip::KEY_SECRET ? "secret" :
                      client.key_type() == drip::KEY_PUBLIC ? "public" : "unknown")
                  << std::endl;

        // =====================================================================
        // 1. Health check
        // =====================================================================
        std::cout << "\n--- Ping ---" << std::endl;
        drip::PingResult health = client.ping();
        std::cout << "API healthy: " << (health.ok ? "yes" : "no") << std::endl;
        std::cout << "Latency: " << health.latency_ms << "ms" << std::endl;

        if (!health.ok) {
            std::cerr << "API is not healthy, exiting." << std::endl;
            return 1;
        }

        // =====================================================================
        // 2. Track usage (no billing - good for pilot/testing)
        // =====================================================================
        std::cout << "\n--- Track Usage ---" << std::endl;
        drip::TrackUsageParams usage;
        usage.customer_id = "cust_example_123";
        usage.meter = "training_tokens";
        usage.quantity = 50000;
        usage.units = "tokens";
        usage.description = "Model training - epoch 1 token consumption";

        // Add training metadata
        usage.metadata["model_type"] = "transformer";
        usage.metadata["dataset"] = "training_set_v2";

        drip::TrackUsageResult usage_result = client.trackUsage(usage);
        std::cout << "Tracked: " << usage_result.usage_event_id << std::endl;
        std::cout << "Message: " << usage_result.message << std::endl;

        // =====================================================================
        // 3. Record a complete training run (all-in-one)
        // =====================================================================
        std::cout << "\n--- Record Training Run ---" << std::endl;
        drip::RecordRunParams run;
        run.customer_id = "cust_example_123";
        run.workflow = "training-run";
        run.status = drip::RUN_COMPLETED;
        run.external_run_id = "train_20260214_001";

        // Training metadata
        run.metadata["model_type"] = "transformer";
        run.metadata["learning_rate"] = "0.001";
        run.metadata["batch_size"] = "32";

        // Event: training started
        drip::RecordRunEvent start_event;
        start_event.event_type = "training.start";
        start_event.description = "Training job initialized";
        run.events.push_back(start_event);

        // Event: epochs completed
        drip::RecordRunEvent epoch_event;
        epoch_event.event_type = "training.epoch";
        epoch_event.quantity = 50;
        epoch_event.units = "epochs";
        epoch_event.description = "50 training epochs completed";
        epoch_event.metadata["final_loss"] = "0.023";
        run.events.push_back(epoch_event);

        // Event: tokens consumed
        drip::RecordRunEvent token_event;
        token_event.event_type = "training.tokens";
        token_event.quantity = 2500000;
        token_event.units = "tokens";
        run.events.push_back(token_event);

        // Event: training ended
        drip::RecordRunEvent end_event;
        end_event.event_type = "training.end";
        end_event.description = "Training completed successfully";
        run.events.push_back(end_event);

        drip::RecordRunResult run_result = client.recordRun(run);
        std::cout << "Run ID: " << run_result.run.id << std::endl;
        std::cout << "Summary: " << run_result.summary << std::endl;
        std::cout << "Events created: " << run_result.events.created << std::endl;

        // =====================================================================
        // 4. Incremental run (start -> emit events -> end)
        //    Useful for long-running training where you emit during training.
        // =====================================================================
        std::cout << "\n--- Incremental Run ---" << std::endl;

        drip::StartRunParams start_params;
        start_params.customer_id = "cust_example_123";
        start_params.workflow_id = run_result.run.workflow_id;  // Reuse workflow from above

        drip::RunResult active_run = client.startRun(start_params);
        std::cout << "Started run: " << active_run.id << std::endl;

        // Emit events as training progresses
        for (int epoch = 1; epoch <= 3; ++epoch) {
            drip::EmitEventParams evt;
            evt.run_id = active_run.id;
            evt.event_type = "training.epoch";
            evt.quantity = 1;
            evt.units = "epochs";

            std::string desc = "Epoch " + std::to_string(epoch) + " completed";
            evt.description = desc;

            drip::EventResult evt_result = client.emitEvent(evt);
            std::cout << "  Epoch " << epoch << " event: " << evt_result.id << std::endl;
        }

        // End the run
        drip::EndRunParams end_params;
        end_params.status = drip::RUN_COMPLETED;

        drip::EndRunResult end_result = client.endRun(active_run.id, end_params);
        std::cout << "Run completed in " << end_result.duration_ms << "ms" << std::endl;
        std::cout << "Total events: " << end_result.event_count << std::endl;

        std::cout << "\nAll examples completed successfully." << std::endl;
        return 0;

    } catch (const drip::AuthenticationError& e) {
        std::cerr << "Authentication error: " << e.what() << std::endl;
        std::cerr << "Set DRIP_API_KEY environment variable." << std::endl;
        return 1;
    } catch (const drip::DripError& e) {
        std::cerr << "Drip error [" << e.status_code() << "]: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
