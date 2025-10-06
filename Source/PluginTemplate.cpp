#define WIN32_LEAN_AND_MEAN

#include "API/ARK/Ark.h"

#include "PluginTemplate.h"

#include "Utils.h"

// Todas las demás cabeceras van aquí
#include "RepairItems.h"

// Fin de otras cabeceras

#include "Hooks.h"

#include "Timers.h"

#include "Commands.h"

#include "Reload.h"

#pragma comment(lib, "ArkApi.lib")

/**
 * @brief Función llamada cuando el servidor está listo.
 * 
 * Esta función se ejecuta cuando el servidor ARK ha terminado de cargar
 * y está listo para aceptar conexiones. Inicializa el plugin cargando
 * la configuración, bases de datos, comandos, temporizadores y ganchos.
 */
void OnServerReady()
{
	Log::GetLog()->info("PluginTemplate Inicializado");

	// Agregar función aquí
	ReadConfig();
	LoadDatabase();
	AddReloadCommands();
	SetTimers();
	SetHooks();
}

/**
 * @brief Gancho para el evento BeginPlay del modo de juego.
 * 
 * Este gancho se activa cuando el modo de juego comienza a ejecutarse.
 * Se utiliza para inicializar el plugin cuando el servidor está completamente listo.
 */
DECLARE_HOOK(AShooterGameMode_BeginPlay, void, AShooterGameMode*);
void Hook_AShooterGameMode_BeginPlay(AShooterGameMode* _this)
{
	AShooterGameMode_BeginPlay_original(_this);

	OnServerReady();
}

/**
 * @brief Punto de entrada principal del plugin.
 * 
 * Esta función es llamada por el sistema de plugins de ARK cuando
 * el plugin se carga. Inicializa el sistema de registro y configura
 * los ganchos necesarios para la funcionalidad del plugin.
 */
extern "C" __declspec(dllexport) void Plugin_Init()
{
	Log::Get().Init(PROJECT_NAME);

	ArkApi::GetHooks().SetHook("AShooterGameMode.BeginPlay", &Hook_AShooterGameMode_BeginPlay, &AShooterGameMode_BeginPlay_original);

	if (ArkApi::GetApiUtils().GetStatus() == ArkApi::ServerStatus::Ready)
		OnServerReady();
}

/**
 * @brief Función de limpieza cuando el plugin se descarga.
 * 
 * Esta función es llamada por el sistema de plugins de ARK cuando
 * el plugin se descarga. Se encarga de deshabilitar los ganchos,
 * comandos y temporizadores para liberar recursos correctamente.
 */
extern "C" __declspec(dllexport) void Plugin_Unload()
{
	ArkApi::GetHooks().DisableHook("AShooterGameMode.BeginPlay", &Hook_AShooterGameMode_BeginPlay);

	// Realizar limpieza aquí
	AddReloadCommands(false);
	SetTimers(false);
	SetHooks(false);
}