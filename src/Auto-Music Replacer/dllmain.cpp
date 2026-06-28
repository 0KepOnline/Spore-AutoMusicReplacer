// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "globals.h"
#include <MicroStorageAPI.h>
#include "MusicReplacerEnabled.h"
#include "AddReplacerMusic.h"
#include "RemoveReplacerMusic.h"

MicroStorageClient* MSclient = nullptr;
eastl::map<uint32_t, uint32_t> alternateMusicIDs; // first uint32 is for act index, second for actual music ID

void Initialize()
{
	// This method is executed when the game starts, before the user interface is shown
	// Here you can do things such as:
	//  - Add new cheats
	//  - Add new simulator classes
	//  - Add new game modes
	//  - Add new space tools
	//  - Change materials

	MSclient = new MicroStorageClient("AutoMusicReplacer");
	CheatManager.AddCheat("musicreplacer",new MusicReplacerEnabled());
	CheatManager.AddCheat("addreplacermusic", new AddReplacerMusic());
	CheatManager.AddCheat("removereplacermusic", new RemoveReplacerMusic());
}

member_detour(cScenarioPlayMode_Initialize_dtr, Simulator::cScenarioPlayMode, void()) {

	void detoured() {
		original_function(this);
		// Skip the entire function if the mod isn't active.
		if (active) {

			alternateMusicIDs.clear(); // Cleaning for the next adventure run.

			auto scnRes = ScenarioMode.GetResource();
			uint32_t i = 0;  // iterator; act 1 is 0.
			uint32_t alternateMusicId;

			for (Simulator::cScenarioAct act : scnRes->mActs) {

				Simulator::cScenarioAct* actP = &act;

				IO::SharedPointer* sharedPtr = new IO::SharedPointer(0, nullptr);
				MemoryStreamPtr memoryStream = new IO::MemoryStream(sharedPtr, 0);
				memoryStream->SetData(sharedPtr, 0);
				memoryStream->SetOption(IO::MemoryStream::kOptionResizeEnabled, 1);

				if (MSclient->Read(actP, id("AMR-ReplacingMusicId"), memoryStream.get())) {
					PropertyListPtr propList = new App::PropertyList();
					propList->Read(memoryStream.get());
					if (propList->HasProperty(id("adventureMusicId"))) {
						App::Property::GetUInt32(propList.get(), id("adventureMusicId"), alternateMusicId);
						alternateMusicIDs.emplace(i, alternateMusicId);
					}
				}
				i++;
			}
		}
	}

};

static_detour(AudioSystem_PlayAudio_dtr, void(uint32_t, Audio::AudioTrack)) {

	void detoured(uint32_t soundID, Audio::AudioTrack track) {
		if (Simulator::GetGameModeID() == GameModeIDs::kScenarioMode
			&& ScenarioMode.GetMode() == App::cScenarioMode::Mode::PlayMode
			&& soundID == ScenarioMode.GetResource()->mActs[ScenarioMode.GetPlayMode()->mCurrentActIndex].mActMusicID
			&& track == ScenarioMode.GetPlayMode()->mMusicTrack
			&& active
			&& alternateMusicIDs.find(ScenarioMode.GetPlayMode()->mCurrentActIndex) != alternateMusicIDs.end())
		{
			original_function(alternateMusicIDs[ScenarioMode.GetPlayMode()->mCurrentActIndex], track);

		}
		else {
			original_function(soundID, track);
		}
	}
};

void Dispose()
{
	// This method is called when the game is closing
	if (MSclient) {
		delete MSclient;
		MSclient = nullptr;
	}
}

void AttachDetours()
{
	// Call the attach() method on any detours you want to add
	// For example: cViewer_SetRenderType_detour::attach(GetAddress(cViewer, SetRenderType));

	cScenarioPlayMode_Initialize_dtr::attach(GetAddress(Simulator::cScenarioPlayMode, Initialize));
	AudioSystem_PlayAudio_dtr::attach(GetAddress(Audio,PlayAudio));

}


// Generally, you don't need to touch any code here
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		ModAPI::AddPostInitFunction(Initialize);
		ModAPI::AddDisposeFunction(Dispose);

		PrepareDetours(hModule);
		AttachDetours();
		CommitDetours();
		break;

	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

