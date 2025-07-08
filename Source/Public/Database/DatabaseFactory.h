#ifndef DATABASEFACTORY_H
#define DATABASEFACTORY_H

#pragma once

#include "MySQLConnector.h"
#include "SQLiteConnector.h"
#include <json.hpp>
#include <memory>

class DatabaseFactory
{
public:
	static std::unique_ptr<IDatabaseConnector> createConnector(const nlohmann::json config)
	{
		if (config["UseMySQL"].get<bool>() == true)
		{	
			return std::make_unique<MySQLConnector>(
				config["Host"],
				config["User"],
				config["Password"],
				config["Database"],
				config["Port"],
				config["MysqlSSLMode"],
				config["MysqlTLSVersion"]
			);
		}
		else
		{
			std::string default_path = ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/" + PROJECT_NAME + "/" + PROJECT_NAME + ".db";
			std::string sqlitePath = config["SQLiteDatabasePath"];
			std::string db_path = (sqlitePath.empty()) ? default_path : sqlitePath;

			return std::make_unique<SQLiteConnector>(db_path);
		}
	}
};

#endif // DATABASEFACTORY_H