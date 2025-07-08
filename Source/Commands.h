
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