//==============================================================================
/**
@file       ActionManager.cpp

@brief      Helper for managing actions / keys

@copyright  (c) 2018, Corsair Memory, Inc.
			This source code is licensed under the MIT-style license found in the LICENSE file.

**/
//==============================================================================

#include "ActionManager.h"
#include "../MyStreamDeckPlugin.h"

ActionManager::ActionManager(MyStreamDeckPlugin* inMemoryGamePlugin)
{
	mMemoryGamePlugin = inMemoryGamePlugin;
}

ActionManager::~ActionManager()
{
}

std::vector<StreamDeckAction> ActionManager::GetAllActions()
{
	std::vector<StreamDeckAction> allActions;
	mMutex.lock();
	for (auto actionGroup : mActionsByDeviceId)
		allActions.insert(--allActions.end(), actionGroup.second.begin(), actionGroup.second.end());
	mMutex.unlock();
	return allActions;
}

std::vector<StreamDeckAction> ActionManager::GetAllGameActionsForDevice(const std::string& inDeviceId)
{
	mMutex.lock();
	std::vector<StreamDeckAction> ret;
	if (mActionsByDeviceId.find(inDeviceId) != mActionsByDeviceId.end())
		ret = std::vector<StreamDeckAction>(mActionsByDeviceId[inDeviceId].begin(), mActionsByDeviceId[inDeviceId].end());

	mMutex.unlock();
	return ret;
}

bool ActionManager::IsDeviceOnline(const std::string& inDeviceId)
{
	mMutex.lock();
	bool knownDevice = mActiveDevicesById.find(inDeviceId) != mActiveDevicesById.end();
	mMutex.unlock();
	return knownDevice;
}

StreamDeckDevice ActionManager::GetDeviceInfoForId(const std::string& inDeviceId) 
{
	mMutex.lock();
	auto it = mActiveDevicesById.find(inDeviceId);
	StreamDeckDevice device = it != mActiveDevicesById.end() ? it->second : StreamDeckDevice("", -1, -1);
	mMutex.unlock();
	return device;
}

void ActionManager::AddDevice(const StreamDeckDevice& inDevice)
{
	if (!inDevice.IsValid())
		return;
	mMutex.lock();
	bool newDevice = mActiveDevicesById.find(inDevice.mDeviceId) == mActiveDevicesById.end();
	if (newDevice)
		mActiveDevicesById[inDevice.mDeviceId] = inDevice;
	mMutex.unlock();
	if (newDevice && IsCompleteProfileLoaded(inDevice.mDeviceId) && mMemoryGamePlugin != nullptr)
	{
		mMemoryGamePlugin->ProfileLoadedForDevice(inDevice.mDeviceId);
	}
}

void ActionManager::RemoveDevice(const std::string& inDeviceId)
{
	mMutex.lock();
	bool isKnownDevice = mActiveDevicesById.find(inDeviceId) != mActiveDevicesById.end();
	if (isKnownDevice)
		mActiveDevicesById.erase(inDeviceId);
	mMutex.unlock();
}

void ActionManager::AddAction(const StreamDeckAction& inAction)
{
	if (!inAction.IsValid())
		return;
	bool updateDevice = false;
	mMutex.lock();
	if (mActionsByDeviceId.find(inAction.mDeviceId) == mActionsByDeviceId.end()) 
	{
		mActionsByDeviceId[inAction.mDeviceId] = std::set<StreamDeckAction>({ inAction });
		updateDevice = true;
	}
	else
	{
		updateDevice = mActionsByDeviceId[inAction.mDeviceId].find(inAction) == mActionsByDeviceId[inAction.mDeviceId].end();
		mActionsByDeviceId[inAction.mDeviceId].insert(inAction);
	}
	mMutex.unlock();
	if (updateDevice && IsCompleteProfileLoaded(inAction.mDeviceId) && mMemoryGamePlugin != nullptr)
		mMemoryGamePlugin->ProfileLoadedForDevice(inAction.mDeviceId);
}

void ActionManager::RemoveAction(const StreamDeckAction& inAction)
{
	if (!inAction.IsValid())
		return;
	bool isInUse = IsDeviceOnline(inAction.mDeviceId);
	mMutex.lock();
	if (mActionsByDeviceId.find(inAction.mDeviceId) != mActionsByDeviceId.end())
	{
		mActionsByDeviceId[inAction.mDeviceId].erase(inAction);
		isInUse = isInUse && true;
	}
	mMutex.unlock();
	if (isInUse)
		ActionOfActiveDeviceDisappeared(inAction.mDeviceId, inAction.mContext);
}

bool ActionManager::IsCompleteProfileLoaded(const std::string& inDeviceId)
{
	auto device = GetDeviceInfoForId(inDeviceId);
	auto actions = GetAllGameActionsForDevice(inDeviceId);
	return device.Size() == actions.size();
}


void ActionManager::ActionOfActiveDeviceDisappeared(const std::string& inDeviceId, const std::string& inContext) 
{
	if (mMemoryGamePlugin != nullptr)
		mMemoryGamePlugin->ActionOfActiveDeviceDisappeared(inDeviceId, inContext);
}
