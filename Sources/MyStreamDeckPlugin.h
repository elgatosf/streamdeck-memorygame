//==============================================================================
/**
@file       MyStreamDeckPlugin.h

@brief      Protocol used for the communication with the Stream Deck application

@copyright  (c) 2018, Corsair Memory, Inc.
			This source code is licensed under the MIT-style license found in the LICENSE file.

**/
//==============================================================================
#pragma once

#include "Common/ESDBasePlugin.h"
#include "MemoryGame.h"
#include "ActionManager.h"

class MyStreamDeckPlugin : public ESDBasePlugin
{
public:
	
	MyStreamDeckPlugin();
	virtual ~MyStreamDeckPlugin();
	
	void KeyDownForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID) override;
	void KeyUpForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID) override;
	
	void WillAppearForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID) override;
	void WillDisappearForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID) override;
	
	void DeviceDidConnect(const std::string& inDeviceID, const json &inDeviceInfo) override;
	void DeviceDidDisconnect(const std::string& inDeviceID) override;

	// Helpers to allow the games to display images / titles or clear the keys
	void SetTitle(const std::string& inTitle, const std::string& inContext);
	void SetImage(const std::string& inImage, const std::string& inContext);
	void ClearKeys(const std::vector<std::string>& inContexts);

	// Helpers for the games to get the keys belonging the its device
	std::vector<std::string> GetAllGameActionsForDevice(const std::string& inDeviceId);
	std::vector<std::string> GetAllResetTilesForDevice(const std::string& inDeviceId);
	
	// Called if context belonging to ongoing game disapears. Removes game.
	void ActionOfActiveDeviceDisappeared(const std::string& inDeviceId, const std::string& inContext);
	// Called if profile was loaded, new game will be started on device
	void ProfileLoadedForDevice(const std::string& inDeviceId);

private:
	std::vector<std::string> GetAllActionsOfTypeForDevice(const std::string& inDeviceId, const std::string& inType);

	std::map<std::string, MemoryGame*> mGames;
	ActionManager* mActionManager = nullptr;
};
