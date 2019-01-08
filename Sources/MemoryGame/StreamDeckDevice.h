//==============================================================================
/**
@file       StreamDeckDevice.h

@brief      Helper class for storing device data

@copyright  (c) 2018, Corsair Memory, Inc.
			This source code is licensed under the MIT-style license found in the LICENSE file.

**/
//==============================================================================

#pragma once

class StreamDeckDevice 
{
public:

	std::string mDeviceId;
	int mRows = -1;
	int mColumns = -1;
		
	StreamDeckDevice();
	StreamDeckDevice(const std::string& inDeviceId, const json& inDeviceInfo);
	StreamDeckDevice(const std::string& inDeviceId, int inRows, int inColumns);
		
	int Size() const;

	bool IsValid() const;
	
};

inline bool operator< (const StreamDeckDevice& inLhs, const StreamDeckDevice& inRhs)
{
	return inLhs.mDeviceId < inRhs.mDeviceId;
}

inline bool operator== (const StreamDeckDevice& inLhs, const StreamDeckDevice& inRhs)
{
	return inLhs.mDeviceId == inRhs.mDeviceId && inLhs.mRows == inRhs.mRows && inLhs.mColumns == inRhs.mColumns;
}
