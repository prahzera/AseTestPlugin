

void RepairItemsCallback(AShooterPlayerController* pc, FString* param, int)
{
	Log::GetLog()->warn("Function: {}", __FUNCTION__);

	// permissions check
	FString steamid = FString(std::to_string(pc->GetLinkedPlayerID64()));
	FString perms = GetPriorPermByEOSID(steamid);
	nlohmann::json command = GetCommandString(perms.ToString(), "RepairItemCMD");

	FString playername;
	pc->GetPlayerCharacterName(&playername);

	if (command.is_null() || (!command.is_null() && command.value("Enabled", false) == false))
	{
		if (PluginTemplate::config["Debug"].value("Permissions", false) == true)
		{
			Log::GetLog()->info("{} No permissions. Command: {}", playername.ToString(), __FUNCTION__);
		}

		ArkApi::GetApiUtils().SendNotification(pc, FColorList::Red, 1.3f, 15.0f, nullptr, PluginTemplate::config["Messages"].value("PermErrorMSG", "You don't have permission to use this command.").c_str());

		return;
	}
	 
	// points checking
	if (Points(steamid, command.value("Cost", 0), true) == false)
	{
		if (PluginTemplate::config["Debug"].value("Points", false) == true)
		{
			Log::GetLog()->info("{} don't have points. Command: {}", playername.ToString(), __FUNCTION__);
		}

		ArkApi::GetApiUtils().SendNotification(pc, FColorList::Red, 1.3f, 15.0f, nullptr, PluginTemplate::config["Messages"].value("PointsErrorMSG", "Not enough points.").c_str());

		return;
	}

	// execute
	ACharacter* character = pc->CharacterField();
	if (!character) return;
	APrimalCharacter* primalCharacter = static_cast<APrimalCharacter*>(character);

	UPrimalInventoryComponent* invComp = primalCharacter->MyInventoryComponentField();
	if (!invComp) return;

	TArray<UPrimalItem*> playerInv = invComp->InventoryItemsField();

	bool ignoreMaterials = PluginTemplate::config["General"]["IgnoreInvRepairRequirements"].get<bool>();

	int affectedItemsCounter = 0;
	for (UPrimalItem* item : playerInv)
	{
		if (item->bIsEngram().Get()) continue;

		if (item->bIsItemSkin().Get()) continue;

		//if(!item->IsBroken()) only fix broken things

		if (!item->UsesDurability()) continue;

		if (item->ItemDurabilityField() == item->SavedDurabilityField()) continue;

		if (ignoreMaterials)
		{
			item->ItemDurabilityField() = item->SavedDurabilityField();
		}
		else
		{
			item->RepairItem(false, 1.0f, 1.0f);
		}

		item->UpdatedItem(false);
		affectedItemsCounter += 1;
	}

	if (ReadPlayer(steamid))
	{
		UpdatePlayer(steamid, playername);
	}
	else
	{
		AddPlayer(steamid, pc->GetLinkedPlayerID64(), playername);
	}

	// points deductions
	Points(steamid, command.value("Cost", 0));

	if (affectedItemsCounter > 0)
	{
		ArkApi::GetApiUtils().SendNotification(pc, FColorList::Green, 1.3f, 15.0f, nullptr, PluginTemplate::config["Messages"].value("RepairItemsMSG", "All items has been repaired. {}").c_str(), playername.ToString());

		std::string msg = fmt::format("Player {} repaired their items. count {}", playername.ToString(), affectedItemsCounter);

		//SendMessageToDiscord(msg);
	}
}


void DeletePlayerCallback(AShooterPlayerController* pc, FString* param, int)
{
	Log::GetLog()->warn("Function: {}", __FUNCTION__);


	//pc->GetEOSId(), pc->GetLinkedPlayerID(), pc->GetCharacterName()

	FString playername;
	pc->GetPlayerCharacterName(&playername);

	FString steamid = FString(std::to_string(pc->GetLinkedPlayerID64()));

	// permissions check
	FString perms = GetPriorPermByEOSID(steamid);
	nlohmann::json command = GetCommandString(perms.ToString(), "DeletePlayerCMD");

	if (command.is_null() || (!command.is_null() && command.value("Enabled", false) == false))
	{
		if (PluginTemplate::config["Debug"].value("Permissions", false) == true)
		{
			Log::GetLog()->info("{} No permissions. Command: {}", playername.ToString(), __FUNCTION__);
		}

		ArkApi::GetApiUtils().SendNotification(pc, FColorList::Red, 1.3f, 15.0f, nullptr, PluginTemplate::config["Messages"].value("PermErrorMSG", "You don't have permission to use this command.").c_str());

		return;
	}

	// points checking
	if (Points(steamid, command.value("Cost", 0), true) == false)
	{
		if (PluginTemplate::config["Debug"].value("Points", false) == true)
		{
			Log::GetLog()->info("{} don't have points. Command: {}", playername.ToString(), __FUNCTION__);
		}

		ArkApi::GetApiUtils().SendNotification(pc, FColorList::Red, 1.3f, 15.0f, nullptr, PluginTemplate::config["Messages"].value("PointsErrorMSG", "Not enough points.").c_str());

		return;
	}


	DeletePlayer(steamid);

	// points deductions
	Points(steamid, command.value("Cost", 0));

	ArkApi::GetApiUtils().SendNotification(pc, FColorList::Orange, 1.3f, 15.0f, nullptr, "Player deleted");
}

