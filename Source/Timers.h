
void TimerCallback()
{
	// sample broadcast every 20secs
	if (PluginTemplate::counter == 20)
	{
		Log::GetLog()->info("Server is up!");

		FString msg = FString("Welcome to the server");
		ArkApi::GetApiUtils().GetCheatManager()->Broadcast(&msg);
	}

	// sample notif every 50secs
	if (PluginTemplate::counter == 50)
	{
		ArkApi::GetApiUtils().SendNotificationToAll(FColorList::Blue, 1.3f, 15.0f, nullptr, "Hooray Welcome to the server! enjoy!");
	}

	/*if (PluginTemplate::counter % 5 == 0)
	{
		FetchMessageFromDiscord();
	}*/


	PluginTemplate::counter++;
}


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