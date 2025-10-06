#include <fstream>

#if 0
/**
 * @brief Verifica si una cadena comienza con un prefijo específico.
 * 
 * Esta función auxiliar determina si una cadena dada comienza con
 * el prefijo especificado.
 * 
 * @param str Cadena a verificar.
 * @param prefix Prefijo a buscar al inicio de la cadena.
 * @return true si la cadena comienza con el prefijo, false en caso contrario.
 */
static bool startsWith(const std::string& str, const std::string& prefix)
{
	return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

/**
 * @brief Callback para procesar mensajes obtenidos de Discord.
 * 
 * Esta función se ejecuta cuando se reciben mensajes del canal de Discord
 * configurado. Procesa los mensajes y los reenvía al chat del juego si
 * cumplen con ciertos criterios.
 * 
 * @param success Indica si la solicitud HTTP fue exitosa.
 * @param results Resultados de la solicitud en formato JSON.
 */
void FetchMessageFromDiscordCallback(bool success, std::string results)
{
	//Log::GetLog()->warn("Función: {}", __FUNCTION__);

	if (success)
	{
		if(results.empty()) return;

		try
		{
			nlohmann::json resObj = nlohmann::json::parse(results)[0];

			if (resObj.is_null())
			{
				Log::GetLog()->warn("resObj es nulo");
				return;
			}

			auto globalName = resObj["author"]["global_name"];

			// si no fue enviado por un bot
			if (resObj.contains("bot") && globalName.is_null())
			{
				Log::GetLog()->warn("el remitente es un bot");
				return;
			}

			std::string msg = resObj["content"].get<std::string>();
			
			if (!startsWith(msg, "!"))
			{
				Log::GetLog()->warn("el mensaje no comienza con !");
				return;
			}

			if (PluginTemplate::lastMessageID == resObj["id"].get<std::string>()) return;
			
			std::string sender = fmt::format("Discord: {}", globalName.get<std::string>());

			ArkApi::GetApiUtils().SendChatMessageToAll(FString(sender), msg.c_str());

			PluginTemplate::lastMessageID = resObj["id"].get<std::string>();
		}
		catch (std::exception& error)
		{
			Log::GetLog()->error("Error al analizar resultados JSON. Error: {}",error.what());
		}
	}
	else
	{
		Log::GetLog()->warn("Fallo al obtener mensajes. éxito: {}", success);
	}
}

/**
 * @brief Obtiene mensajes del canal de Discord configurado.
 * 
 * Esta función realiza una solicitud HTTP GET al API de Discord
 * para obtener los mensajes más recientes del canal configurado
 * en el archivo de configuración.
 */
void FetchMessageFromDiscord()
{
	//Log::GetLog()->warn("Función: {}", __FUNCTION__);

	std::string botToken = PluginTemplate::config["DiscordBot"].value("BotToken","");

	std::string channelID = PluginTemplate::config["DiscordBot"].value("ChannelID", "");

	std::string apiURL = FString::Format("https://discord.com/api/v10/channels/{}/messages?limit=1", channelID).ToString();

	std::vector<std::string> headers = {
		"Content-Type: application/json",
		"User-Agent: PluginTemplate/1.0",
		"Connection: keep-alive",
		"Accept: */*",
		"Content-Length: 0",
		"Authorization: Bot " + botToken
	};

	try
	{
		bool req = PluginTemplate::req.CreateGetRequest(apiURL, FetchMessageFromDiscordCallback, headers);

		if (!req)
			Log::GetLog()->error("Fallo al realizar la solicitud GET. req = {}", req);
	}
	catch (const std::exception& error)
	{
		Log::GetLog()->error("Fallo al realizar la solicitud GET. Error: {}", error.what());
	}
}

/**
 * @brief Callback para enviar mensajes a Discord.
 * 
 * Esta función se ejecuta después de intentar enviar un mensaje
 * al webhook de Discord configurado.
 * 
 * @param success Indica si la solicitud HTTP fue exitosa.
 * @param results Resultados de la solicitud.
 */
void SendMessageToDiscordCallback(bool success, std::string results)
{
	if (!success)
	{
		Log::GetLog()->error("Fallo al enviar la solicitud POST. {} {} {}", __FUNCTION__, success, results);
	}
	else
	{
		Log::GetLog()->info("Éxito. {} {} {}", __FUNCTION__, success, results);
	}
}

/**
 * @brief Callback para enviar mensajes a Discord con encabezados de respuesta.
 * 
 * Esta función se ejecuta después de intentar enviar un mensaje
 * al webhook de Discord configurado, incluyendo los encabezados
 * de respuesta del servidor.
 * 
 * @param success Indica si la solicitud HTTP fue exitosa.
 * @param results Resultados de la solicitud.
 * @param responseHeaders Encabezados de respuesta del servidor.
 */
void SendMessageToDiscordCallback1(bool success, std::string results, std::unordered_map<std::string, std::string> responseHeaders)
{
	if (!success)
	{
		Log::GetLog()->error("Fallo al enviar la solicitud POST. {} {} {}", __FUNCTION__, success, results);
	}
	else
	{
		Log::GetLog()->info("Éxito. {} {} {}", __FUNCTION__, success, results);
	}
}

/**
 * @brief Envía un mensaje al webhook de Discord configurado.
 * 
 * Esta función envía un mensaje al webhook de Discord especificado
 * en la configuración del plugin.
 * 
 * @param msg Mensaje a enviar al canal de Discord.
 */
void SendMessageToDiscord(std::string msg)
{

	Log::GetLog()->warn("Función: {}", __FUNCTION__);

	
	std::string webhook = PluginTemplate::config["DiscordBot"].value("Webhook", "");
	std::string botImgUrl = PluginTemplate::config["DiscordBot"].value("BotImageURL", "");

	if (webhook == "" || webhook.empty()) return;

	FString msgFormat = L"{{\"content\":\"{}\",\"username\":\"{}\",\"avatar_url\":\"{}\"}}";

	FString msgOutput = FString::Format(*msgFormat, msg, "ArkBot", botImgUrl);

	std::vector<std::string> headers = {
		"Content-Type: application/json",
		"User-Agent: PluginTemplate/1.0",
		"Connection: keep-alive",
		"Accept: */*"
	};

	try
	{
		bool req = PluginTemplate::req.CreatePostRequest(webhook, SendMessageToDiscordCallback, msgOutput.ToString(), "application/json", headers);

		if(!req)
			Log::GetLog()->error("Fallo al enviar la solicitud POST. req = {}", req);
	}
	catch (const std::exception& error)
	{
		Log::GetLog()->error("Fallo al enviar la solicitud POST. Error: {}", error.what());
	}
}
#endif

/**
 * @brief Gestiona el sistema de puntos del jugador.
 * 
 * Esta función verifica si un jugador tiene suficientes puntos para
 * ejecutar un comando, y si no es una verificación, deduce el costo
 * del comando del saldo del jugador.
 * 
 * @param eos_id ID de EOS del jugador.
 * @param cost Costo en puntos del comando a ejecutar.
 * @param check_points Bandera que indica si solo se debe verificar (true) o también deducir puntos (false).
 * @return true si el jugador tiene suficientes puntos o si la operación fue exitosa, false en caso contrario.
 */
bool Points(FString eos_id, int cost, bool check_points = false)
{
	if (cost == -1)
	{
		if (PluginTemplate::config["Debug"].value("Points", false) == true)
		{
			Log::GetLog()->warn("El costo es -1");
		}
		return false;
	}

	if (cost == 0)
	{
		if (PluginTemplate::config["Debug"].value("Points", false) == true)
		{
			Log::GetLog()->warn("El costo es 0");
		}

		return true;
	}

	nlohmann::json config = PluginTemplate::config["PointsDBSettings"];

	if (config.value("Enabled", false) == false)
	{
		if (PluginTemplate::config["Debug"].value("Points", false) == true)
		{
			Log::GetLog()->warn("El sistema de puntos está deshabilitado");
		}

		return true;
	}

	std::string tablename = config.value("TableName", "ArkShopPlayers");
	std::string unique_id = config.value("UniqueIDField", "EosId");
	std::string points_field = config.value("PointsField", "Points");
	std::string totalspent_field = config.value("TotalSpentField", "TotalSpent");

	if (tablename.empty() || unique_id.empty() || points_field.empty())
	{
		if (PluginTemplate::config["Debug"].value("Points", false) == true)
		{
			Log::GetLog()->warn("Los campos de la base de datos están vacíos");
		}
		return false;
	}

	std::string escaped_eos_id = PluginTemplate::pointsDB->escapeString(eos_id.ToString());

	std::string query = fmt::format("SELECT * FROM {} WHERE {}='{}'", tablename, unique_id, escaped_eos_id);

	std::vector<std::map<std::string, std::string>> results;

	if (!PluginTemplate::pointsDB->read(query, results))
	{
		if (PluginTemplate::config["Debug"].value("Points", false) == true)
		{
			Log::GetLog()->warn("Error al leer la base de datos de puntos");
		}

		return false;
	}

	if (results.size() <= 0)
	{
		if (PluginTemplate::config["Debug"].value("Points", false) == true)
		{
			Log::GetLog()->warn("No se encontró ningún registro");
		}
		return false;
	}

	int points = std::atoi(results[0].at(points_field).c_str());

	if (check_points)
	{
		if (PluginTemplate::config["Debug"].value("Points", false) == true)
		{
			Log::GetLog()->warn("El jugador tiene {} puntos", points);
		}

		if (points >= cost) return true;
	}
	else
	{
		int amount = points - cost;

		std::vector<std::pair<std::string, std::string>> data;

		data.push_back({ points_field, std::to_string(amount) });

		if (totalspent_field != "")
		{
			int total_spent = std::atoi(results[0].at(totalspent_field).c_str());
			std::string total_ts = std::to_string(total_spent + cost);

			data.push_back({totalspent_field, total_ts});
		}

		std::string condition = fmt::format("{}='{}'", unique_id, escaped_eos_id);

		if (PluginTemplate::pointsDB->update(tablename, data, condition))
		{
			if (PluginTemplate::config["Debug"].value("Points", false) == true)
			{
				Log::GetLog()->info("{} Base de datos de puntos actualizada", amount);
			}

			return true;
		}
	}

	return false;
}

/**
 * @brief Obtiene la configuración de un comando para un grupo de permisos específico.
 * 
 * Esta función busca en la configuración del plugin la configuración
 * específica de un comando para un grupo de permisos dado.
 * 
 * @param permission Grupo de permisos del jugador.
 * @param command Nombre del comando a buscar.
 * @return Objeto JSON con la configuración del comando, o un objeto vacío si no se encuentra.
 */
nlohmann::json GetCommandString(const std::string permission, const std::string command)
{
	if (permission.empty()) return {};
	if (command.empty()) return {};

	nlohmann::json config_obj = PluginTemplate::config["PermissionGroups"];
	nlohmann::json perm_obj = config_obj[permission];
	nlohmann::json command_obj = perm_obj["Commands"];
	nlohmann::json setting_obj = command_obj[command];

	return setting_obj;
}

/**
 * @brief Obtiene los grupos de permisos de un jugador.
 * 
 * Esta función consulta la base de datos de permisos para obtener
 * todos los grupos de permisos asignados a un jugador específico.
 * 
 * @param eos_id ID de EOS del jugador.
 * @return TArray de FString con los grupos de permisos del jugador.
 */
TArray<FString> GetPlayerPermissions(FString eos_id)
{
	TArray<FString> PlayerPerms = { "Default" };

	std::string escaped_eos_id = PluginTemplate::permissionsDB->escapeString(eos_id.ToString());

	std::string tablename = PluginTemplate::config["PermissionsDBSettings"].value("TableName", "Players");

	std::string condition = PluginTemplate::config["PermissionsDBSettings"].value("UniqueIDField", "EOS_Id");

	std::string query = fmt::format("SELECT * FROM {} WHERE {}='{}';", tablename, condition, escaped_eos_id);

	std::vector<std::map<std::string, std::string>> results;
	if (!PluginTemplate::permissionsDB->read(query, results))
	{
		if (PluginTemplate::config["Debug"].value("Permissions", false) == true)
		{
			Log::GetLog()->warn("Error al leer la base de datos de permisos");
		}

		return PlayerPerms;
	}

	if (results.size() <= 0) return PlayerPerms;

	std::string permsfield = PluginTemplate::config["PermissionsDBSettings"].value("PermissionGroupField","PermissionGroups");

	FString playerperms = FString(results[0].at(permsfield));

	if (PluginTemplate::config["Debug"].value("Permissions", false) == true)
	{
		Log::GetLog()->info("permisos actuales del jugador {}", playerperms.ToString());
	}

	playerperms.ParseIntoArray(PlayerPerms, L",", true);

	return PlayerPerms;
}

/**
 * @brief Obtiene el grupo de permisos con mayor prioridad de un jugador.
 * 
 * Esta función determina cuál de los grupos de permisos asignados
 * a un jugador tiene la prioridad más alta (número más bajo).
 * 
 * @param eos_id ID de EOS del jugador.
 * @return FString con el nombre del grupo de permisos de mayor prioridad.
 */
FString GetPriorPermByEOSID(FString eos_id)
{
	TArray<FString> player_groups = GetPlayerPermissions(eos_id);

	const nlohmann::json permGroups = PluginTemplate::config["PermissionGroups"];

	std::string defaultGroup = "Default";
	int minPriority = INT_MAX;
	nlohmann::json result;
	FString selectedPerm = "Default";

	for (const FString& param : player_groups)
	{
		if (permGroups.contains(param.ToString()))
		{
			int priority = static_cast<int>(permGroups[param.ToString()]["Priority"]);
			if (priority < minPriority)
			{
				minPriority = priority;
				result = permGroups[param.ToString()];
				selectedPerm = param;
			}
		}
	}

	if (result.is_null() && permGroups.contains(defaultGroup))
	{
		if(!permGroups[defaultGroup].is_null())
			result = permGroups[defaultGroup];

		result = {};
	}

	if (PluginTemplate::config["Debug"].value("Permissions", false) == true)
	{
		Log::GetLog()->info("Permiso seleccionado {}", selectedPerm.ToString());
	}

	return selectedPerm;
}

/**
 * @brief Agrega un nuevo jugador a la base de datos del plugin.
 * 
 * Esta función inserta un nuevo registro en la tabla de jugadores
 * del plugin con la información básica del jugador.
 * 
 * @param eosID ID de EOS del jugador.
 * @param playerID ID del jugador en el juego.
 * @param playerName Nombre del jugador.
 * @return true si el jugador fue agregado exitosamente, false en caso contrario.
 */
bool AddPlayer(FString eosID, int playerID, FString playerName)
{
	std::vector<std::pair<std::string, std::string>> data = {
		{"EosId", eosID.ToString()},
		{"PlayerId", std::to_string(playerID)},
		{"PlayerName", playerName.ToString()}
	};

	return PluginTemplate::pluginTemplateDB->create(PluginTemplate::config["PluginDBSettings"]["TableName"].get<std::string>(), data);
}

/**
 * @brief Verifica si un jugador existe en la base de datos del plugin.
 * 
 * Esta función consulta la base de datos del plugin para determinar
 * si un jugador específico ya tiene un registro.
 * 
 * @param eosID ID de EOS del jugador.
 * @return true si el jugador existe en la base de datos, false en caso contrario.
 */
bool ReadPlayer(FString eosID)
{
	std::string escaped_id = PluginTemplate::pluginTemplateDB->escapeString(eosID.ToString());

	std::string query = fmt::format("SELECT * FROM {} WHERE EosId='{}'", PluginTemplate::config["PluginDBSettings"]["TableName"].get<std::string>(), escaped_id);

	std::vector<std::map<std::string, std::string>> results;
	PluginTemplate::pluginTemplateDB->read(query, results);

	return results.size() <= 0 ? false : true;
}

/**
 * @brief Actualiza la información de un jugador en la base de datos del plugin.
 * 
 * Esta función actualiza el nombre del jugador en la base de datos
 * del plugin. Nota: Actualmente añade "123" al nombre como ejemplo.
 * 
 * @param eosID ID de EOS del jugador.
 * @param playerName Nombre del jugador.
 * @return true si la información fue actualizada exitosamente, false en caso contrario.
 */
bool UpdatePlayer(FString eosID, FString playerName)
{
	std::string unique_id = "EosId";

	std::string escaped_id = PluginTemplate::pluginTemplateDB->escapeString(eosID.ToString());

	std::vector<std::pair<std::string, std::string>> data = {
		{"PlayerName", playerName.ToString() + "123"}
	};

	std::string condition = fmt::format("{}='{}'", unique_id, escaped_id);

	return PluginTemplate::pluginTemplateDB->update(PluginTemplate::config["PluginDBSettings"]["TableName"].get<std::string>(), data, condition);
}

/**
 * @brief Elimina un jugador de la base de datos del plugin.
 * 
 * Esta función elimina el registro de un jugador específico
 * de la base de datos del plugin.
 * 
 * @param eosID ID de EOS del jugador a eliminar.
 * @return true si el jugador fue eliminado exitosamente, false en caso contrario.
 */
bool DeletePlayer(FString eosID)
{
	std::string escaped_id = PluginTemplate::pluginTemplateDB->escapeString(eosID.ToString());

	std::string condition = fmt::format("EosId='{}'", escaped_id);

	return PluginTemplate::pluginTemplateDB->deleteRow(PluginTemplate::config["PluginDBSettings"]["TableName"].get<std::string>(), condition);
}

/**
 * @brief Lee la configuración del plugin desde el archivo config.json.
 * 
 * Esta función carga la configuración del plugin desde el archivo
 * config.json ubicado en el directorio del plugin. También establece
 * las banderas de depuración según la configuración.
 */
void ReadConfig()
{
	try
	{
		const std::string config_path = ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/" + PROJECT_NAME + "/config.json";
		std::ifstream file{config_path};
		if (!file.is_open())
		{
			throw std::runtime_error("No se puede abrir el archivo de configuración.");
		}
		file >> PluginTemplate::config;

		Log::GetLog()->info("{} archivo de configuración cargado.", PROJECT_NAME);

		PluginTemplate::isDebug = PluginTemplate::config["Debug"]["PluginTemplate"].get<bool>();

		Log::GetLog()->warn("Depuración {}", PluginTemplate::isDebug);

	}
	catch(const std::exception& error)
	{
		Log::GetLog()->error("Fallo al cargar la configuración. ERROR: {}", error.what());
		throw;
	}
}

/**
 * @brief Carga las conexiones a las bases de datos del plugin.
 * 
 * Esta función inicializa las conexiones a las bases de datos
 * utilizadas por el plugin, incluyendo la base de datos principal,
 * la base de datos de permisos y la base de datos de puntos.
 * También crea las tablas necesarias si no existen.
 */
void LoadDatabase()
{
	Log::GetLog()->warn("Cargar base de datos");
	PluginTemplate::pluginTemplateDB = DatabaseFactory::createConnector(PluginTemplate::config["PluginDBSettings"]);

	nlohmann::ordered_json tableDefinition = {};
	if (PluginTemplate::config["PluginDBSettings"].value("UseMySQL", true) == true)
	{
		tableDefinition = {
			{"Id", "INT NOT NULL AUTO_INCREMENT"},
			{"EosId", "VARCHAR(50) NOT NULL"},
			{"PlayerId", "VARCHAR(50) NOT NULL"},
			{"PlayerName", "VARCHAR(50) NOT NULL"},
			{"CreateAt", "DATETIME DEFAULT CURRENT_TIMESTAMP"},
			{"PRIMARY", "KEY(Id)"},
			{"UNIQUE", "INDEX EosId_UNIQUE (EosId ASC)"}
		};
	}
	else
	{
		tableDefinition = {
			{"Id","INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT"},
			{"EosId","TEXT NOT NULL UNIQUE"},
			{"PlayerId","TEXT"},
			{"PlayerName","TEXT"},
			{"CreateAt","TIMESTAMP DEFAULT CURRENT_TIMESTAMP"}
		};
	}

	PluginTemplate::pluginTemplateDB->createTableIfNotExist(PluginTemplate::config["PluginDBSettings"].value("TableName", ""), tableDefinition);


	// Base de datos de permisos
	if (PluginTemplate::config["PermissionsDBSettings"].value("Enabled", true) == true)
	{
		PluginTemplate::permissionsDB = DatabaseFactory::createConnector(PluginTemplate::config["PermissionsDBSettings"]);
	}

	// Base de datos de puntos (ArkShop)
	if (PluginTemplate::config["PointsDBSettings"].value("Enabled", true) == true)
	{
		PluginTemplate::pointsDB = DatabaseFactory::createConnector(PluginTemplate::config["PointsDBSettings"]);
	}
	
}