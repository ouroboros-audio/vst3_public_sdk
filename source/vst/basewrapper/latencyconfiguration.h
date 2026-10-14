#pragma once
#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace Steinberg::Vst
{
struct LatencyConfiguration
{
    uint64 id = 0; // Zero means the processor already uses this configuration.
    uint32 samples = 0;
};

// Optional adapter contract for hosts that acknowledge a latency request before
// the processor may change its delay. Calls are serialized on the control side,
// with audio callbacks excluded while applying configurations or changing mode.
class ILatencyConfiguration : public FUnknown
{
public:
    // Enable before setupProcessing so initial/reprepared queues also await host approval.
    virtual tresult PLUGIN_API setHostManagedLatency (TBool enabled) = 0;
    virtual tresult PLUGIN_API captureLatencyConfiguration (int32 processMode, LatencyConfiguration& configuration) = 0;
    virtual tresult PLUGIN_API applyLatencyConfiguration (uint64 id, int32 processMode) = 0;
    // Change scheduling immediately, retaining the currently approved delay.
    virtual tresult PLUGIN_API setProcessingModePreservingLatency (int32 processMode) = 0;
    static inline const FUID iid = FUID (0xE642A7B8, 0x1889419C, 0xA126669B, 0x47F425A7);
};
}
