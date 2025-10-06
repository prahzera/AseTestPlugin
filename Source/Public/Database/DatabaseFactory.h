#ifndef DATABASEFACTORY_H
#define DATABASEFACTORY_H

#pragma once

#include "MySQLConnector.h"
#include "SQLiteConnector.h"
#include <json.hpp>
#include <memory>

/**
 * @class DatabaseFactory
 * @brief Fábrica para crear conectores de bases de datos.
 * 
 * Esta clase proporciona un método estático para crear instancias
 * de conectores de bases de datos según la configuración especificada.
 * Permite crear conectores para MySQL o SQLite dependiendo de
 * la configuración del plugin.
 */
class DatabaseFactory
{
public:
	/**
	 * @brief Crea un conector de base de datos según la configuración.
	 * 
	 * Este método fábrica crea una instancia del conector de base de datos
	 * apropiado (MySQL o SQLite) basado en la configuración proporcionada.
	 * 
	 * @param config Configuración JSON que especifica el tipo de base de datos y sus parámetros.
	 * @return Puntero único al conector de base de datos creado.
	 */
	static std::unique_ptr<IDatabaseConnector> createConnector(const nlohmann::json config)
	{
		int sslMode = config.value("MysqlSSLMode", -1);
		std::string tlsVersion = config.value("MysqlTLSVersion", "");

		if (config.value("UseMySQL", true) == true)
		{	
			return std::make_unique<MySQLConnector>(
				config["Host"],
				config["User"],
				config["Password"],
				config["Database"],
				config["Port"],
				sslMode,
				tlsVersion
			);
		}
		else
		{
			std::string default_path = ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/" + PROJECT_NAME + "/" + PROJECT_NAME + ".db";
			std::string sqlitePath = config.value("SQLiteDatabasePath","");
			std::string db_path = (sqlitePath.empty()) ? default_path : sqlitePath;

			return std::make_unique<SQLiteConnector>(db_path);
		}
	}
};

#endif // DATABASEFACTORY_H