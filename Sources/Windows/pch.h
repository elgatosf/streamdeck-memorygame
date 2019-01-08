//==============================================================================
/**
@file       pch.h

@brief      Precompiled header for Stream Deck plugin

@copyright  (c) 2018, Corsair Memory, Inc.
			This source code is licensed under the MIT-style license found in the LICENSE file.

**/
//==============================================================================

#ifndef PCH_H
#define PCH_H

// On Windows disable the MIN/MAX defines from windows.h
#define NOMINMAX


//-------------------------------------------------------------------
// C++ headers
//-------------------------------------------------------------------

#include <winsock2.h>
#include <Windows.h>
#include <string>
#include <set>
#include <thread>
#include <iostream>
#include <fstream>
#include <strsafe.h>


//-------------------------------------------------------------------
// Debug logging
//-------------------------------------------------------------------

#ifdef _DEBUG
	#define DEBUG 1
#else
	#define DEBUG 0
#endif

#if DEBUG
	#define DEBUG_SHOW_DEBUG_TITLES 0
#else
	#define DEBUG_SHOW_DEBUG_TITLES 0
#endif

void __cdecl dbgprintf(const char *format, ...);

#if DEBUG
	#define DebugPrint			dbgprintf
#else
	#define DebugPrint(...)		while(0)
#endif


//-------------------------------------------------------------------
// json
//-------------------------------------------------------------------

#include "../Vendor/json/src/json.hpp"
using json = nlohmann::json;


//-------------------------------------------------------------------
// websocketpp
//-------------------------------------------------------------------

#define ASIO_STANDALONE




#endif //PCH_H
