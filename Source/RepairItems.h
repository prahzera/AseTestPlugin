/**
 * @brief Callback para el comando de reparación de objetos.
 * 
 * Esta función se ejecuta cuando un jugador utiliza el comando de reparación
 * de objetos. Verifica los permisos del jugador, el costo en puntos (si aplica),
 * y repara todos los objetos dañados en el inventario del jugador.
 * 
 * @param pc Controlador del jugador que ejecuta el comando.
 * @param param Parámetros adicionales del comando (no utilizados).
 * @param unused Parámetro no utilizado.
 */
void RepairItemsCallback(AShooterPlayerController* pc, FString* param, int)
{
	Log::GetLog()->warn("Función: {}", __FUNCTION__);

	// Verificación de permisos
	FString steamid = FString(std::to_string(pc->GetLinkedPlayerID64()));
	FString perms = GetPriorPermByEOSID(steamid);
	nlohmann::json command = GetCommandString(perms.ToString(), "RepairItemCMD");

	FString playername;
	pc->GetPlayerCharacterName(&playername);

	if (command.is_null() || (!command.is_null() && command.value("Enabled", false) == false))
	{
		if (PluginTemplate::config["Debug"].value("Permissions", false) == true)
		{
			Log::GetLog()->info("{} No tiene permisos. Comando: {}", playername.ToString(), __FUNCTION__);
		}

		ArkApi::GetApiUtils().SendNotification(pc, FColorList::Red, 1.3f, 15.0f, nullptr, PluginTemplate::config["Messages"].value("PermErrorMSG", "No tienes permiso para usar este comando.").c_str());

		return;
	}
	 
	// Verificación de puntos
	if (Points(steamid, command.value("Cost", 0), true) == false)
	{
		if (PluginTemplate::config["Debug"].value("Points", false) == true)
		{
			Log::GetLog()->info("{} no tiene suficientes puntos. Comando: {}", playername.ToString(), __FUNCTION__);
		}

		ArkApi::GetApiUtils().SendNotification(pc, FColorList::Red, 1.3f, 15.0f, nullptr, PluginTemplate::config["Messages"].value("PointsErrorMSG", "No tienes suficientes puntos.").c_str());

		return;
	}

	// Ejecución
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

		//if(!item->IsBroken()) solo reparar cosas rotas

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

	// Deducción de puntos
	Points(steamid, command.value("Cost", 0));

	if (affectedItemsCounter > 0)
	{
		ArkApi::GetApiUtils().SendNotification(pc, FColorList::Green, 1.3f, 15.0f, nullptr, PluginTemplate::config["Messages"].value("RepairItemsMSG", "Todos los objetos han sido reparados. {}").c_str(), playername.ToString());

		std::string msg = fmt::format("El jugador {} reparó sus objetos. cantidad {}", playername.ToString(), affectedItemsCounter);

		//SendMessageToDiscord(msg);
	}
}

/**
 * @brief Callback para el comando de eliminación de jugador.
 * 
 * Esta función se ejecuta cuando un jugador utiliza el comando de eliminación
 * de jugador. Verifica los permisos del jugador, el costo en puntos (si aplica),
 * y elimina los datos del jugador de la base de datos del plugin.
 * 
 * @param pc Controlador del jugador que ejecuta el comando.
 * @param param Parámetros adicionales del comando (no utilizados).
 * @param unused Parámetro no utilizado.
 */
void DeletePlayerCallback(AShooterPlayerController* pc, FString* param, int)
{
	Log::GetLog()->warn("Función: {}", __FUNCTION__);


	//pc->GetEOSId(), pc->GetLinkedPlayerID(), pc->GetCharacterName()

	FString playername;
	pc->GetPlayerCharacterName(&playername);

	FString steamid = FString(std::to_string(pc->GetLinkedPlayerID64()));

	// Verificación de permisos
	FString perms = GetPriorPermByEOSID(steamid);
	nlohmann::json command = GetCommandString(perms.ToString(), "DeletePlayerCMD");

	if (command.is_null() || (!command.is_null() && command.value("Enabled", false) == false))
	{
		if (PluginTemplate::config["Debug"].value("Permissions", false) == true)
		{
			Log::GetLog()->info("{} No tiene permisos. Comando: {}", playername.ToString(), __FUNCTION__);
		}

		ArkApi::GetApiUtils().SendNotification(pc, FColorList::Red, 1.3f, 15.0f, nullptr, PluginTemplate::config["Messages"].value("PermErrorMSG", "No tienes permiso para usar este comando.").c_str());

		return;
	}

	// Verificación de puntos
	if (Points(steamid, command.value("Cost", 0), true) == false)
	{
		if (PluginTemplate::config["Debug"].value("Points", false) == true)
		{
			Log::GetLog()->info("{} no tiene suficientes puntos. Comando: {}", playername.ToString(), __FUNCTION__);
		}

		ArkApi::GetApiUtils().SendNotification(pc, FColorList::Red, 1.3f, 15.0f, nullptr, PluginTemplate::config["Messages"].value("PointsErrorMSG", "No tienes suficientes puntos.").c_str());

		return;
	}


	DeletePlayer(steamid);

	// Deducción de puntos
	Points(steamid, command.value("Cost", 0));

	ArkApi::GetApiUtils().SendNotification(pc, FColorList::Orange, 1.3f, 15.0f, nullptr, "Jugador eliminado");
}