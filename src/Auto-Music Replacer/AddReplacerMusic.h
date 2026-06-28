#pragma once

#include <Spore\BasicIncludes.h>
#include "globals.h"

class AddReplacerMusic 
	: public ArgScript::ICommand,
	public Sporepedia::IShopperListener
{
public:
	AddReplacerMusic();
	~AddReplacerMusic();

	// Called when the cheat is invoked
	void ParseLine(const ArgScript::Line& line) override;
	void OnShopperAccept(const ResourceKey& selection) override;
	
	// Returns a string containing the description. If mode != DescriptionMode::Basic, return a more elaborated description
	const char* GetDescription(ArgScript::DescriptionMode mode) const override;
};

