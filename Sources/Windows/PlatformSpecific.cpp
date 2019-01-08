//==============================================================================
/**
@file       PlatformSpecific.cpp

@brief      Platform specific code

@copyright  (c) 2018, Corsair Memory, Inc.
			This source code is licensed under the MIT-style license found in the LICENSE file.

**/
//==============================================================================

#include "PlatformSpecific.h"

#pragma comment(lib,"Winmm.lib")

void PlatformSpecific::PlaySoundGameFinished()
{
	PlaySound(L"SystemExclamation", nullptr, SND_ASYNC);
}

