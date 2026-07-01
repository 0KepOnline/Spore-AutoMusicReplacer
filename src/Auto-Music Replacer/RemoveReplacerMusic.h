#pragma once

#include <Spore\BasicIncludes.h>

class RemoveReplacerMusic 
	: public ArgScript::ICommand
	, public UTFWin::IWinProc
	, public DefaultRefCounted
{
public:
#pragma region constants
	static const uint32_t SND_SKINNING_REMOVE = id("editor_trash");
#pragma endregion Sound IDs

	RemoveReplacerMusic();
	~RemoveReplacerMusic();

	// Called when the cheat is invoked
	void ParseLine(const ArgScript::Line& line) override;
	
	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
	int GetEventFlags() const override;
	bool HandleUIMessage(UTFWin::IWindow* pWindow, const UTFWin::Message& message) override;
	
	// Returns a string containing the description. If mode != DescriptionMode::Basic, return a more elaborated description
	const char* GetDescription(ArgScript::DescriptionMode mode) const override;
};

