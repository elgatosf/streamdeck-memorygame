//==============================================================================
/**
@file       StreamDeckAction.cpp

@brief      Helper class to store action data

@copyright  (c) 2018, Corsair Memory, Inc.
			This source code is licensed under the MIT-style license found in the LICENSE file.

**/
//==============================================================================

#include "StreamDeckAction.h"


StreamDeckAction::StreamDeckAction(const std::string& inContext, const std::string& inDeviceId, const std::string& inActionType)
{
	mContext = inContext;
	mDeviceId = inDeviceId;
	mActionType = inActionType;
}

bool StreamDeckAction::IsValid() const
{
	return !mContext.empty() && !mDeviceId.empty() && (mActionType == kActionNameReset || mActionType == kActionNameTile);
}

