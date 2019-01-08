//==============================================================================
/**
@file       StreamDeckAction.h

@brief      Helper class to store action data

@copyright  (c) 2018, Corsair Memory, Inc.
			This source code is licensed under the MIT-style license found in the LICENSE file.

**/
//==============================================================================

#pragma once


#define kActionNameTile  "com.elgato.memorygame.tile"
#define kActionNameReset "com.elgato.memorygame.reset"

class StreamDeckAction 
{
public:
	std::string mContext;
	std::string mDeviceId;
	std::string mActionType;

	StreamDeckAction(const std::string& inContext, const std::string& inDeviceId, const std::string& inActionType);

	bool IsValid() const;
};

inline bool operator< (const StreamDeckAction& inLhs, const StreamDeckAction& inRhs)
{
	if (inLhs.mContext != inRhs.mContext)
		return inLhs.mContext < inRhs.mContext;
	else if (inLhs.mDeviceId < inRhs.mDeviceId)
		return inLhs.mDeviceId < inRhs.mDeviceId;
	return inLhs.mActionType < inRhs.mActionType;
}

inline bool operator== (const StreamDeckAction& inLhs, const StreamDeckAction& inRhs)
{
	return inLhs.mDeviceId == inRhs.mDeviceId && inLhs.mContext == inRhs.mContext && inLhs.mActionType == inRhs.mActionType;
}
