//==============================================================================
/**
@file       ActionManager.h

@brief      Helper for managing actions / keys

@copyright  (c) 2018, Corsair Memory, Inc.
			This source code is licensed under the MIT-style license found in the LICENSE file.

**/
//==============================================================================

#pragma once

#include <mutex>
#include "../Common/EPLJSONUtils.h"
#include "StreamDeckAction.h"
#include "StreamDeckDevice.h"

class MyStreamDeckPlugin;

// Class to manage actions and devices. Notifies the plugin if the game can be started, or has to be stopped.
class ActionManager
{
public:

	ActionManager(MyStreamDeckPlugin* inMemoryGamePlugin);
	~ActionManager();	

	// Methods for accessing the appeared actions
	std::vector<StreamDeckAction> GetAllActions();
	std::vector<StreamDeckAction> GetAllGameActionsForDevice(const std::string& inDeviceId);

	bool IsDeviceOnline(const std::string& inDeviceId);

	// Returns the device data stored for device. If the device id is unknown, returns an invalid device
	StreamDeckDevice GetDeviceInfoForId(const std::string& inDeviceId);

	// Method to add devices. If all the actions have already appeared, it notifies the plugin
	void AddDevice(const StreamDeckDevice& inDevice);

	// Method to remove devices
	void RemoveDevice(const std::string& inDeviceId);

	// Method to add actions. If the actions belongs to a connected device and all actions for this device appeared, it notifies the plugin
	void AddAction(const StreamDeckAction&  inAction);

	// Methods to remove actions
	void RemoveAction(const StreamDeckAction& inAction);

	// Returns true if the number of appeared actions for device equals the devices' size.
	bool IsCompleteProfileLoaded(const std::string& inDeviceId);

private:

	// Signals the plugin that a possibly used context disappeared
	void ActionOfActiveDeviceDisappeared(const std::string& inDeviceId, const std::string& inContext);

	std::mutex mMutex;
	MyStreamDeckPlugin* mMemoryGamePlugin = nullptr;

	std::map<std::string, StreamDeckDevice>				mActiveDevicesById;
	std::map<std::string, std::set<StreamDeckAction>>	mActionsByDeviceId;

};

