//==============================================================================
/**
@file       MyStreamDeckPlugin.cpp

@brief      Protocol used for the communication with the Stream Deck application

@copyright  (c) 2018, Corsair Memory, Inc.
			This source code is licensed under the MIT-style license found in the LICENSE file.

**/
//==============================================================================

#include "MyStreamDeckPlugin.h"
#include "Common/ESDConnectionManager.h"
#include "ESDLocalizer.h"

MyStreamDeckPlugin::MyStreamDeckPlugin()
{
	mActionManager = new ActionManager(this);
}

MyStreamDeckPlugin::~MyStreamDeckPlugin()
{
	delete mActionManager;
}

void MyStreamDeckPlugin::KeyDownForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID)
{
	// Nothing to do
}

void MyStreamDeckPlugin::KeyUpForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID)
{
	// if the key belongs to a game, handle it
	if (mGames.find(inDeviceID) != mGames.end())
	{
		if (inAction == kActionNameTile)
			mGames[inDeviceID]->HandleMemoryTilePressed(inContext);
		else if (inAction == kActionNameReset)
			mGames[inDeviceID]->InitGame();
	}
}

void MyStreamDeckPlugin::WillAppearForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID)
{
	if (mActionManager != nullptr)
		mActionManager->AddAction(StreamDeckAction(inContext, inDeviceID, inAction));
}

void MyStreamDeckPlugin::WillDisappearForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID)
{
	if (mActionManager != nullptr)
		mActionManager->RemoveAction(StreamDeckAction(inContext, inDeviceID, inAction));
}

void MyStreamDeckPlugin::DeviceDidConnect(const std::string& inDeviceID, const json &inDeviceInfo)
{
	if (mActionManager != nullptr)
		mActionManager->AddDevice(StreamDeckDevice(inDeviceID, inDeviceInfo));
}

void MyStreamDeckPlugin::DeviceDidDisconnect(const std::string& inDeviceID)
{
	if (mActionManager != nullptr)
		mActionManager->RemoveDevice(inDeviceID);
	// remove game
	if (mGames.find(inDeviceID) != mGames.end())
	{
		if(mGames[inDeviceID] != nullptr)
		{
			delete mGames[inDeviceID];
		}
		
		mGames.erase(inDeviceID);
	}
}

void MyStreamDeckPlugin::SetTitle(const std::string& inTitle, const std::string& inContext) 
{
	if (mConnectionManager != nullptr)
		mConnectionManager->SetTitle(inTitle, inContext, kESDSDKTarget_HardwareAndSoftware);
}

void MyStreamDeckPlugin::SetImage(const std::string& inImage, const std::string& inContext) 
{
	if (mConnectionManager != nullptr)
		mConnectionManager->SetImage(inImage, inContext, kESDSDKTarget_HardwareAndSoftware);
}

void MyStreamDeckPlugin::ClearKeys(const std::vector<std::string>& inContexts)
{
	for (const auto& context : inContexts)
	{
		// delete titles + icons
		SetTitle("", context);
		SetImage("", context);
	}
}

std::vector<std::string> MyStreamDeckPlugin::GetAllGameActionsForDevice(const std::string& inDeviceId)
{
	return GetAllActionsOfTypeForDevice(inDeviceId, kActionNameTile);
}

std::vector<std::string> MyStreamDeckPlugin::GetAllResetTilesForDevice(const std::string& inDeviceId)
{
	return GetAllActionsOfTypeForDevice(inDeviceId, kActionNameReset);
}

std::vector<std::string> MyStreamDeckPlugin::GetAllActionsOfTypeForDevice(const std::string& inDeviceId, const std::string& inType)
{
	std::vector<std::string> ret;
	if (mActionManager != nullptr)
	{
		auto allActions = mActionManager->GetAllGameActionsForDevice(inDeviceId);
		for (auto action : allActions)
			if (action.mActionType == inType)
				ret.push_back(action.mContext);
	}
	return ret;
}

void MyStreamDeckPlugin::ActionOfActiveDeviceDisappeared(const std::string& inDeviceId, const std::string& inContext) 
{
	if (mGames.find(inDeviceId) != mGames.end())
	{
		if(mGames[inDeviceId] != nullptr)
		{
			delete mGames[inDeviceId];
		}
		
		mGames.erase(inDeviceId);
	}
}

void MyStreamDeckPlugin::ProfileLoadedForDevice(const std::string& inDeviceId)
{
	if (mGames.find(inDeviceId) == mGames.end())
	{
		mGames[inDeviceId] = new MemoryGame(this, inDeviceId);
	}
}
