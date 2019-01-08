//==============================================================================
/**
@file       MemoryGame.h

@brief      Header for game logic

@copyright  (c) 2018, Corsair Memory, Inc.
			This source code is licensed under the MIT-style license found in the LICENSE file.

**/
//==============================================================================

#pragma once

#include <deque>
#include <random>

class MyStreamDeckPlugin;

// Class with the actual game logic
class MemoryGame
{
public:

	MemoryGame(MyStreamDeckPlugin* inPlugin, const std::string& inDeviceId);
	~MemoryGame();

	// Initializes the game, resets all lists etc
	void InitGame();
	
	// Handling key presses for game tiles.
	void HandleMemoryTilePressed(const std::string& inContext);
	
	// Lets the title "Solved" flash on the keys and resets the game
	void ShowSuccessAnimationAndRestart(const std::vector<std::string>& inContexts);

private:

	bool ContextHasIcon(const std::string& inContext) const;
	bool ContextHasTitle(const std::string& inContext) const;

	std::string GetEncodedIconForContext(const std::string& inContext);
	std::string GetHelperTitleForContext(const std::string& inContext);

	// Methods to display or hide icons / titles on the keys
	void SendRevealContext(const std::string& inContext);
	void SendHideContext(const std::string& inContext);
	void SendSolvedContext(const std::string& inContext);
	void ClearKeys(const std::vector<std::string>& inContexts);

	// Loads all game icons and the reset icon and store them in base 64 encoded strings
	bool LoadIcons();
	// Builds the pairs of keys to be matched by the user
	void BuildActionPairs();
	// Waits for all animations to finish
	void JoinAllAnimationTimers();

	static bool GetEncodedIconStringFromFile(const std::string& inName, std::string& outFileString);

	// Used to get the contexts for the game and reset actions
	std::vector<std::string> GetAllGameActionsForDevice();
	std::vector<std::string> GetAllResetTilesForDevice();

	std::vector<std::string>			mAllGameTileContexts;
	std::map<std::string, std::string>  mUnfinishedPairs;
	std::set<std::string>				mFinishedContexts;

	std::map<std::string, std::string>	mIconsForContexts;
	std::map<std::string, std::string>	mTitlesForContexts; // if not enough icons are loaded

	std::string							mCurrentRevealedContext;

	std::deque<std::thread*>			mDisplayHelperTimers;

	std::vector<std::string>			mIcons;

	std::vector<std::string>			mResetTileContexts;
	std::string							mResetIcon;
	bool								mStopDisplayingMismatch = false;
	std::string							mDeviceId;
	MyStreamDeckPlugin*					mMemoryGamePlugin = nullptr;

	std::uniform_int_distribution<int>	mDistribution;
	static std::default_random_engine	sRandomNumberGenerator;
};

