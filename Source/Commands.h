/**
 * @brief Agrega o elimina los comandos de chat del plugin.
 * 
 * Esta función se encarga de registrar o desregistrar los comandos de chat
 * proporcionados por el plugin. Los comandos se configuran en el archivo
 * config.json y se pueden habilitar o deshabilitar según sea necesario.
 * 
 * @param addCmd Bandera que indica si se deben agregar (true) o eliminar (false) los comandos.
 */
void AddOrRemoveCommands(bool addCmd = true)
{
	const FString RepairItems = PluginTemplate::config["Commands"]["RepairItemCMD"].get<std::string>().c_str();
	if (!RepairItems.IsEmpty())
	{
		if (addCmd)
		{
			ArkApi::GetCommands().AddChatCommand(RepairItems, &RepairItemsCallback);
		}
		else
		{
			ArkApi::GetCommands().RemoveChatCommand(RepairItems);
		}
	}

	const FString DeletePlayer = PluginTemplate::config["Commands"]["DeletePlayerCMD"].get<std::string>().c_str();
	if (!DeletePlayer.IsEmpty())
	{
		if (addCmd)
		{
			ArkApi::GetCommands().AddChatCommand(DeletePlayer, &DeletePlayerCallback);
		}
		else
		{
			ArkApi::GetCommands().RemoveChatCommand(DeletePlayer);
		}
	}
}