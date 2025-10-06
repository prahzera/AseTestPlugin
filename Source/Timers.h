/**
 * @brief Función de devolución de llamada para eventos temporizados.
 * 
 * Esta función se ejecuta periódicamente según el temporizador configurado.
 * Se encarga de realizar acciones programadas como anuncios al servidor
 * y notificaciones a los jugadores en intervalos específicos.
 */
void TimerCallback()
{
	// Anuncio de muestra cada 20 segundos
	if (PluginTemplate::counter == 20)
	{
		Log::GetLog()->info("¡El servidor está activo!");

		//FString msg = FString("Bienvenido al servidor");
		//ArkApi::GetApiUtils().GetCheatManager()->Broadcast(&msg);
	}

	// Notificación de muestra cada 50 segundos
	if (PluginTemplate::counter == 50)
	{
		ArkApi::GetApiUtils().SendNotificationToAll(FColorList::Blue, 1.3f, 15.0f, nullptr, "¡Hurra! ¡Bienvenido al servidor! ¡Disfruta!");
	}

	/*if (PluginTemplate::counter % 5 == 0)
	{
		FetchMessageFromDiscord();
	}*/


	PluginTemplate::counter++;
}

/**
 * @brief Configura o elimina los temporizadores del plugin.
 * 
 * Esta función se encarga de agregar o remover el callback del temporizador
 * utilizado por el plugin. Cuando se agrega, se ejecuta periódicamente
 * para realizar acciones programadas.
 * 
 * @param addTmr Bandera que indica si se debe agregar (true) o eliminar (false) el temporizador.
 */
void SetTimers(bool addTmr = true)
{
	if (addTmr)
	{
		ArkApi::GetCommands().AddOnTimerCallback("PluginTemplateTimerTick", &TimerCallback);
	}
	else
	{
		ArkApi::GetCommands().RemoveOnTimerCallback("PluginTemplateTimerTick");
	}
}