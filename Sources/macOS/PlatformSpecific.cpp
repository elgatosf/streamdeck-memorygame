//==============================================================================
/**
@file       PlatformSpecific.cpp

@brief      Platform specific code

@copyright  (c) 2018, Corsair Memory, Inc.
			This source code is licensed under the MIT-style license found in the LICENSE file.

**/
//==============================================================================

#include "PlatformSpecific.h"
#include <AudioToolbox/AudioToolbox.h>
#include <CoreFoundation/CoreFoundation.h>


void PlatformSpecific::PlaySoundGameFinished()
{
	// Playback the alert sound selected by the User in System Preferences
	AudioServicesPlaySystemSound(kSystemSoundID_UserPreferredAlert);
}
