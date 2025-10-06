/**
 * @brief Recarga la configuración del plugin.
 * 
 * Esta función se encarga de recargar la configuración del plugin
 * desde el archivo config.json sin necesidad de reiniciar el servidor.
 * Útil para aplicar cambios de configuración en tiempo de ejecución.
 */
void Reload()
{
	ReadConfig();

	//AddOrRemoveCommands(false);

	//AddOrRemoveCommands();
}

/**
 * @brief Recarga la configuración a través de un comando de consola.
 * 
 * Esta función es llamada cuando un administrador utiliza el comando
 * de consola para recargar la configuración del plugin. Maneja
 * posibles errores durante la recarga y notifica al administrador.
 * 
 * @param pc Controlador del jugador que ejecuta el comando.
 * @param param Parámetros adicionales del comando (no utilizados).
 * @param unused Parámetro no utilizado.
 */
void ReloadConfig(APlayerController* pc, FString*, bool)
{
	auto* spc = static_cast<AShooterPlayerController*>(pc);

	try
	{
		Reload();
	}
	catch (const std::exception& error)
	{
		ArkApi::GetApiUtils().SendServerMessage(spc, FColorList::Red, "Fallo al recargar la configuración. ERROR: {}", error.what());
	}

	ArkApi::GetApiUtils().SendServerMessage(spc, FColorList::Green, "Configuración recargada.");
}

/**
 * @brief Recarga la configuración a través de RCON.
 * 
 * Esta función es llamada cuando se utiliza RCON para recargar
 * la configuración del plugin. Maneja posibles errores y
 * responde adecuadamente al cliente RCON.
 * 
 * @param rcon_connection Conexión RCON del cliente.
 * @param rcon_packet Paquete RCON recibido.
 * @param unused Parámetro no utilizado.
 */
void ReloadConfigRcon(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld*)
{
	FString reply;

	try
	{
		Reload();
	}
	catch (const std::exception& error)
	{
		reply = error.what();
		rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
		return;
	}

	reply = "Configuración recargada.";
	rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
}

/**
 * @brief Agrega o elimina los comandos de recarga.
 * 
 * Esta función se encarga de registrar o desregistrar los comandos
 * de consola y RCON utilizados para recargar la configuración
 * del plugin en tiempo de ejecución.
 * 
 * @param addCmd Bandera que indica si se deben agregar (true) o eliminar (false) los comandos.
 */
void AddReloadCommands(bool addCmd = true)
{
	FString reloadCmd = std::string(PROJECT_NAME + std::string(".Reload")).c_str();

	if (addCmd)
	{
		ArkApi::GetCommands().AddConsoleCommand(reloadCmd, &ReloadConfig);
		ArkApi::GetCommands().AddRconCommand(reloadCmd, &ReloadConfigRcon);
	}
	else
	{
		ArkApi::GetCommands().RemoveConsoleCommand(reloadCmd);
		ArkApi::GetCommands().RemoveRconCommand(reloadCmd);
	}
}