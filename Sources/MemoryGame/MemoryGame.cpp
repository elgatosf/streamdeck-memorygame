//==============================================================================
/**
@file       MemoryGame.cpp

@brief      Class for game logic

@copyright  (c) 2018, Corsair Memory, Inc.
			This source code is licensed under the MIT-style license found in the LICENSE file.

**/
//==============================================================================

#include "MemoryGame.h"
#include "../MyStreamDeckPlugin.h"
#include "../Vendor/cppcodec/cppcodec/base64_rfc4648.hpp"
#include "../Common/ESDLocalizer.h"
#include "../Common/ESDUtilities.h"

#ifdef __APPLE__
	#include "../macOS/PlatformSpecific.h"
#else
	#include "../Windows/PlatformSpecific.h"
#endif
std::default_random_engine MemoryGame::sRandomNumberGenerator(std::random_device{}());

MemoryGame::MemoryGame(MyStreamDeckPlugin* inPlugin, const std::string& inDeviceId)
{
	mMemoryGamePlugin = inPlugin;
	mDeviceId = inDeviceId;
	if (inPlugin != nullptr && !inDeviceId.empty())
		InitGame();
}

MemoryGame::~MemoryGame()
{
	ClearKeys(mAllGameTileContexts);
	ClearKeys(mResetTileContexts);
	JoinAllAnimationTimers();
}

bool MemoryGame::ContextHasIcon(const std::string& inContext) const
{
	return mIconsForContexts.find(inContext) != mIconsForContexts.end();
}

bool MemoryGame::ContextHasTitle(const std::string& inContext) const
{
	return mTitlesForContexts.find(inContext) != mTitlesForContexts.end();
}

std::string MemoryGame::GetEncodedIconForContext(const std::string& inContext)
{
	if (ContextHasIcon(inContext))
		return mIconsForContexts[inContext];
	return "";
}

std::string MemoryGame::GetHelperTitleForContext(const std::string& inContext)
{
	if (ContextHasTitle(inContext))
		return mTitlesForContexts[inContext];
	
	return "";
}

// Initializes the game, resets all lists etc
void MemoryGame::InitGame()
{
	// make sure no animation is playing anymore
	JoinAllAnimationTimers();

	// Clear all keys
	ClearKeys(mAllGameTileContexts);
	ClearKeys(mResetTileContexts);

	// load icons
	bool success = LoadIcons();
	if (!success)
	{
		DebugPrint("Something went wrong when loading icons, use titles instead.\n");
		mIcons.clear();
	}

	// clear all lists etc
	mResetTileContexts.clear();
	mUnfinishedPairs.clear();
	mFinishedContexts.clear();
	mCurrentRevealedContext.clear();
	mIconsForContexts.clear();

	// rebuild the pairs
	BuildActionPairs();

	// display the reset image or title on the reset keys
	mResetTileContexts = GetAllResetTilesForDevice();
	for (const auto& context : mResetTileContexts)
	{
		if (!mResetIcon.empty())
		{
			mMemoryGamePlugin->SetImage(mResetIcon, context);
		}
		else
		{
			mMemoryGamePlugin->SetTitle(ESDLocalizer::GetLocalizedString("Reset"), context);
		}
	}
}

// Handling key presses for game tiles.
// when no tile is revealed, reveal tile
// when there is a already tile revealed and new tile does not match, reveal both for a short time, then hide both
// when there is a already tile revealed and new tile matches, mark pair as resolved etc
void MemoryGame::HandleMemoryTilePressed(const std::string& inContext)
{
	if (mFinishedContexts.find(inContext) != mFinishedContexts.end() || inContext == mCurrentRevealedContext)
		return;
	else if (mCurrentRevealedContext.empty())
	{
		// reveal image of key
		mStopDisplayingMismatch = true;
		mCurrentRevealedContext = inContext;
		SendRevealContext(inContext);
	}
	else if (mCurrentRevealedContext != inContext && mUnfinishedPairs[inContext] != mCurrentRevealedContext)
	{
		// not a match, reveal both for a second and hide both
		SendRevealContext(inContext);
		mStopDisplayingMismatch = false;
		std::string context2 = mCurrentRevealedContext;
		// thread will wait a second or until a new key is pressed and hide both keys
		std::thread* displayHelperTimer = new std::thread([inContext, context2, this]()
		{
			for (int i = 0; i < 10; i++)
			{
				if (mStopDisplayingMismatch) // new key was pressed
					break;
				ESDUtilities::DoSleep(100);
			}

			SendHideContext(inContext);
			SendHideContext(context2);
			mStopDisplayingMismatch = false;
		});
		mDisplayHelperTimers.push_back(displayHelperTimer);
		mCurrentRevealedContext = "";
	}
	else if (mCurrentRevealedContext != inContext && mUnfinishedPairs[inContext] == mCurrentRevealedContext)
	{
		// pair matches, show both keys as solved and check if game is finished
		SendSolvedContext(inContext);
		SendSolvedContext(mCurrentRevealedContext);

		mFinishedContexts.insert(inContext);
		mFinishedContexts.insert(mCurrentRevealedContext);
		mUnfinishedPairs.erase(inContext);
		mUnfinishedPairs.erase(mCurrentRevealedContext);
		mCurrentRevealedContext = "";

		if (mUnfinishedPairs.empty())
		{
			// Game is finished, play sound and show animation
			PlatformSpecific::PlaySoundGameFinished();
			auto contextsForAnimation = mAllGameTileContexts;
			ShowSuccessAnimationAndRestart(contextsForAnimation);
		}
	}
}

void MemoryGame::ShowSuccessAnimationAndRestart(const std::vector<std::string>& inContexts)
{
	if (mMemoryGamePlugin == nullptr)
		return;
	// show animation in thread by letting the title "Solved" flash on all keys and reinitialize the game
	std::thread* ani = new std::thread([inContexts, this]
	{
		for (int i = 0; i < 5; i++)
		{
			ESDUtilities::DoSleep(500);
			for (auto context : inContexts)
			{
				mMemoryGamePlugin->SetTitle("", context);
			}
			ESDUtilities::DoSleep(500);
			for (auto context : inContexts)
			{
				mMemoryGamePlugin->SetTitle(ESDLocalizer::GetLocalizedString("Solved"), context);
			}
		}
		InitGame();

	});
	ani->detach();
	mDisplayHelperTimers.push_back(ani);
}


// Reveal the Image / Caption of the key when guessing
void MemoryGame::SendRevealContext(const std::string& inContext)
{
	if (mMemoryGamePlugin != nullptr)
	{
		if (ContextHasIcon(inContext))
		{
			mMemoryGamePlugin->SetImage(GetEncodedIconForContext(inContext), inContext);
		}
		else if (ContextHasTitle(inContext))
		{
			mMemoryGamePlugin->SetTitle(GetHelperTitleForContext(inContext), inContext);
		}
		else
		{
			DebugPrint("Error: No title and no icon found\n");
		}
	}
}

// Hide the Image / Caption of the key
void MemoryGame::SendHideContext(const std::string& inContext)
{
	if (mMemoryGamePlugin != nullptr)
	{
		if (ContextHasIcon(inContext))
		{
			mMemoryGamePlugin->SetImage("", inContext);
		}
		else if (ContextHasTitle(inContext))
		{
			mMemoryGamePlugin->SetTitle("", inContext);
		}
		else
		{
			DebugPrint("Error: No title and no icon found\n");
		}
	}
}

// Reveal the Image or display "Solved" when image pair was succesfully matched
void MemoryGame::SendSolvedContext(const std::string& inContext)
{
	if (mMemoryGamePlugin != nullptr)
	{
		if (ContextHasIcon(inContext))
		{
			mMemoryGamePlugin->SetImage(GetEncodedIconForContext(inContext), inContext);
		}
		else if (ContextHasTitle(inContext))
		{
			mMemoryGamePlugin->SetTitle(ESDLocalizer::GetLocalizedString("Solved"), inContext);
		}
		else
		{
			DebugPrint("Error: No title and no icon found\n");
		}
	}
}


// Make sure, all animation threads are done
void MemoryGame::JoinAllAnimationTimers()
{
	while (!mDisplayHelperTimers.empty())
	{
		auto timerPtr = *mDisplayHelperTimers.begin();
		if (timerPtr != nullptr && timerPtr->joinable())
		{
			timerPtr->join();
			delete timerPtr;
			timerPtr = nullptr;
		}
		mDisplayHelperTimers.pop_front();
	}
	mStopDisplayingMismatch = false;
}

// load images for game tiles and reset icon and stores them as base 64 encoded string
bool MemoryGame::LoadIcons()
{
	mIcons.clear();
	mResetIcon.clear();
	
	bool didLoadIcons = true;
	
	std::string pluginPath = ESDUtilities::GetPluginPath();
	if (!pluginPath.empty())
	{
		// load tile icons
		for (int i = 1; i < 16; i++)
		{
			std::string encodedFile;
			bool couldLoad = GetEncodedIconStringFromFile(ESDUtilities::AddPathComponent(pluginPath, std::to_string(i) + ".png"), encodedFile);
			if (couldLoad)
			{
				mIcons.push_back(encodedFile);
			}
			else
			{
				didLoadIcons = false;
				DebugPrint("Could not load icon: %d.png\n", i);
			}
		}
		//load reset icon
		bool couldLoad = GetEncodedIconStringFromFile(ESDUtilities::AddPathComponent(pluginPath, "startover.png"), mResetIcon);
		if (!couldLoad)
		{
			mResetIcon.clear();
			didLoadIcons = false;
			DebugPrint("Could not load icon: startover.png\n");
		}
	}

	return didLoadIcons;
}

// Builds a list of pairs from all game tiles and assigns icons / titles to them.
// These pairs have to be matched by the user.
void MemoryGame::BuildActionPairs()
{
	auto actionList = GetAllGameActionsForDevice();
	//mDistribution = std::uniform_int_distribution<int>(0, (int)actionList.size() - 1);

	mAllGameTileContexts = actionList;
	if (actionList.size() % 2 != 0)
	{
		DebugPrint("actionList has wrong size %ld\n", actionList.size());
		actionList.pop_back(); // remove last action to get even number for pairs		
	}

	int iconCaption = (int)mIcons.size();
	while (!actionList.empty())
	{
		std::string context1 = actionList[actionList.size() - 1];
		actionList.pop_back();
		int index = mDistribution(sRandomNumberGenerator) % actionList.size();
		std::string context2 = actionList[index];
		actionList.erase(actionList.begin() + index);
		mUnfinishedPairs[context1] = context2;
		mUnfinishedPairs[context2] = context1;	

		if (!mIcons.empty())
		{
			int iconIndex = mDistribution(sRandomNumberGenerator) % mIcons.size();
			mIconsForContexts[context1] = mIcons[iconIndex];
			mIconsForContexts[context2] = mIcons[iconIndex];
			mIcons.erase(mIcons.begin() + iconIndex);
		}
		else
		{
			mTitlesForContexts[context1] = std::to_string(iconCaption);
			mTitlesForContexts[context2] = std::to_string(iconCaption);
			iconCaption++;
		}
	}
}

void MemoryGame::ClearKeys(const std::vector<std::string>& inContexts) 
{
	if (mMemoryGamePlugin != nullptr)
		mMemoryGamePlugin->ClearKeys(inContexts);
}

std::vector<std::string> MemoryGame::GetAllGameActionsForDevice()
{
	if (mMemoryGamePlugin != nullptr && !mDeviceId.empty())
	{
		return mMemoryGamePlugin->GetAllGameActionsForDevice(mDeviceId);
	}
	
	return std::vector<std::string>();
}

std::vector<std::string> MemoryGame::GetAllResetTilesForDevice()
{
	if (mMemoryGamePlugin != nullptr && !mDeviceId.empty())
	{
		return mMemoryGamePlugin->GetAllResetTilesForDevice(mDeviceId);
	}
	
	return std::vector<std::string>();
}

// Helper method, reads file into base64 encoded string
bool MemoryGame::GetEncodedIconStringFromFile(const std::string& inName, std::string& outFileString)
{
	bool success = false;
	std::ifstream pngFile(inName, std::ios::binary | std::ios::ate);
	if (pngFile.is_open())
	{
		std::ifstream::pos_type pos = pngFile.tellg();

		std::vector<char> result(pos);

		pngFile.seekg(0, std::ios::beg);
		pngFile.read(&result[0], pos);

		std::string base64encodedImage = cppcodec::base64_rfc4648::encode(&result[0], result.size());
		outFileString = base64encodedImage;
		success = true;
	}
	else
	{
		success = false;
		DebugPrint("Could not load icon: %s", inName.c_str());
	}
	return success;
}

