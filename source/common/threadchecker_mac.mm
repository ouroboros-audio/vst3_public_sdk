//-----------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Examples
// Filename    : public.sdk/source/common/threadchecker_mac.mm
// Created by  : Steinberg, 01/2019
// Description : macOS thread checker
//
//-----------------------------------------------------------------------------
// This file is part of a Steinberg SDK. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this distribution
// and at www.steinberg.net/sdklicenses.
// No part of the SDK, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the LICENSE file.
//-----------------------------------------------------------------------------

#include "threadchecker.h"

#if SMTG_OS_MACOS

#include <pthread.h>
#include <Foundation/Foundation.h>

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
class MacThreadChecker : public ThreadChecker
{
public:
	bool test (const char* failmessage = nullptr, bool exit = false) override
	{
		// VST3's ConnectionProxy uses this to decide whether messages can be delivered
		// immediately or must be queued/flushed on the UI/main thread.
		//
		// In AAX hosts, ConnectionProxy can be constructed on a non-main thread during
		// plug-in instantiation, which would cause the original "capture pthread_self()"
		// logic to treat that thread as the "UI thread" and permanently block message
		// delivery via the main run loop.
		//
		// On macOS we instead key off the actual process main thread.
		if (pthread_main_np ())
			return true;
		if (failmessage)
			NSLog (@"%s", failmessage);
		if (exit)
			std::terminate ();
		return false;
	}
};

//------------------------------------------------------------------------
std::unique_ptr<ThreadChecker> ThreadChecker::create ()
{
	return std::unique_ptr<ThreadChecker> (new MacThreadChecker);
}

//------------------------------------------------------------------------
} // Vst
} // Steinberg

#endif
