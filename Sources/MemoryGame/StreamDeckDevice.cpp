//==============================================================================
/**
@file       StreamDeckDevice.cpp

@brief      Helper class for storing device data

@copyright  (c) 2018, Corsair Memory, Inc.
			This source code is licensed under the MIT-style license found in the LICENSE file.

**/
//==============================================================================

#include "StreamDeckDevice.h"
#include "../Common/EPLJSONUtils.h"
#include "../Common/ESDSDKDefines.h"


StreamDeckDevice::StreamDeckDevice() {}

StreamDeckDevice::StreamDeckDevice(const std::string& inDeviceId, const json& inDeviceInfo)
{
	mRows = -1;
	mColumns = -1;
	mDeviceId = inDeviceId;
	try
	{
		json sizeInfo;
		if (EPLJSONUtils::GetObjectByName(inDeviceInfo, kESDSDKDeviceInfoSize, sizeInfo))
		{
			mRows = EPLJSONUtils::GetIntByName(sizeInfo, kESDSDKDeviceInfoSizeRows, -1);
			mColumns = EPLJSONUtils::GetIntByName(sizeInfo, kESDSDKDeviceInfoSizeColumns, -1);
		}
	}
	catch (...)
	{
	}
}

StreamDeckDevice::StreamDeckDevice(const std::string& inDeviceId, int inRows, int inColumns)
{
	mDeviceId = inDeviceId;
	mRows = inRows;
	mColumns = inColumns;
}

int StreamDeckDevice::Size() const
{
	return mRows >= 0 ? mRows * mColumns : 0;
}

bool StreamDeckDevice::IsValid() const
{
	return !mDeviceId.empty() && mColumns > 0 && mRows > 0;
}
