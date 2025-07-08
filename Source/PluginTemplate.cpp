#define WIN32_LEAN_AND_MEAN

#include "API/ARK/Ark.h"

#include "PluginTemplate.h"

#include "Utils.h"

// all other headers goes here
#include "RepairItems.h"

//end of other headers

#include "Hooks.h"

#include "Timers.h"

#include "Commands.h"

#include "Reload.h"

#pragma comment(lib, "ArkApi.lib")

void OnServerReady()
{
	Log::GetLog()->info("PluginTemplate Initialized");

	// add function here
	ReadConfig();
	LoadDatabase();
	AddReloadCommands();
	SetTimers();
	SetHooks();
}

DECLARE_HOOK(AShooterGameMode_BeginPlay, void, AShooterGameMode*);
void Hook_AShooterGameMode_BeginPlay(AShooterGameMode* _this)
{
	AShooterGameMode_BeginPlay_original(_this);

	OnServerReady();
}

extern "C" __declspec(dllexport) void Plugin_Init()
{
	Log::Get().Init(PROJECT_NAME);

	ArkApi::GetHooks().SetHook("AShooterGameMode.BeginPlay", &Hook_AShooterGameMode_BeginPlay, &AShooterGameMode_BeginPlay_original);

	if (ArkApi::GetApiUtils().GetStatus() == ArkApi::ServerStatus::Ready)
		OnServerReady();
}

extern "C" __declspec(dllexport) void Plugin_Unload()
{
	ArkApi::GetHooks().DisableHook("AShooterGameMode.BeginPlay", &Hook_AShooterGameMode_BeginPlay);

	// Do cleanup here
	AddReloadCommands(false);
	SetTimers(false);
	SetHooks(false);
}

