#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace Steinberg::Vst
{
// Optional bridge for wrappers with a separate main-thread parameter API.
// Accepted zero-offset controls must be visible to getState immediately, without
// processing audio. Send sample-offset/audio-thread automation through process()
// instead; a successfully accepted control must not also be sent there.
class IMainThreadParameterControl : public FUnknown
{
public:
    virtual tresult PLUGIN_API setParameterNormalizedControl (ParamID id, ParamValue value) = 0;
    static inline const FUID iid = FUID (0x969097C9, 0x6C304633, 0x81F8C6B1, 0xD70DF9F0);
};
}
