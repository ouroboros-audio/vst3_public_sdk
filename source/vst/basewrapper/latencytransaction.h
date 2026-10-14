#pragma once
#include "latencyconfiguration.h"
#include <mutex>
#include <optional>

namespace Steinberg::Vst
{
// One host request at a time: later changes wait behind its immutable snapshot.
// Host calls run outside the mutex because acknowledgements may be synchronous,
// asynchronous, or delivered concurrently with the timer.
class LatencyTransaction
{
    struct Pending { LatencyConfiguration configuration; uint64 generation; bool sent = false; };
    std::mutex mutex;
    std::optional<Pending> pending;
    uint64 generation = 0;
    int64 approved = -1;
    bool compensationEnabled = true;
public:
    template<class Capture, class Apply, class Request>
    void service(Capture capture, Apply apply, Request request)
    {
        std::unique_lock lock(mutex);
        if (pending && pending->sent) return;
        if (!pending) {
            LatencyConfiguration configuration;
            if (!capture(configuration)) return;
            if (configuration.samples == approved) {
                if (configuration.id) apply(configuration.id);
                return;
            }
            // With compensation disabled the host deliberately sends no ack.
            if (!compensationEnabled && configuration.id && !apply(configuration.id)) return;
            pending = Pending{configuration, ++generation};
        }
        const auto sent = *pending;
        pending->sent = true;
        lock.unlock();
        const bool accepted = request(sent.configuration.samples);
        lock.lock();
        // A reentrant acknowledgement may already have committed this request.
        if (!pending || pending->generation != sent.generation) return;
        if (!accepted) pending.reset();
        else if (!compensationEnabled) {
            approved = sent.configuration.samples;
            pending.reset();
        }
    }

    template<class Apply>
    void acknowledge(uint32 samples, Apply apply)
    {
        std::lock_guard lock(mutex);
        if (!compensationEnabled) return;
        approved = samples;
        if (!pending || !pending->sent || pending->configuration.samples != samples) return;
        if (pending->configuration.id) apply(pending->configuration.id);
        pending.reset();
    }

    void setCompensationEnabled(bool enabled)
    {
        std::lock_guard lock(mutex);
        if (compensationEnabled == enabled) return;
        compensationEnabled = enabled;
        pending.reset();
        // Re-enabling requires a fresh host request even if the delay is equal.
        approved = -1;
    }
};
}
