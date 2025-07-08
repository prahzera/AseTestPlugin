#ifndef MYSQLCONNECTOR_H
#define MYSQLCONNECTOR_H

#pragma once

#include <mysql/mysql.h>
#include "IDatabaseConnector.h"


#pragma comment(lib,"mysqlclient.lib")

class MySQLConnector : public IDatabaseConnector
{
public:
	MySQLConnector(const std::string& host, const std::string& user, const std::string& password, const std::string& dbname, unsigned int port, int ssl_mode, const std::string& tls_version)
	{
		if (!MySQLConnect()) return;
	}

	~MySQLConnector()
	{
		if (conn != nullptr)
		{
			mysql_close(conn);
		}
	}

	bool createTableIfNotExist(const std::string& tableName, const nlohmann::ordered_json& tableDefinition)
	{
		std::string query = "CREATE TABLE IF NOT EXISTS " + tableName + "(";

		bool first = true;
		for (auto& [columnName, dataType] : tableDefinition.items())
		{
			if (!first)
			{
				query += ", ";
			}
			first = false;
			query += columnName + " " + dataType.get <std::string>();
		}

		query += ")";

		return executeQuery(query);
	}

	bool alterTableIfNotExists(const std::string& tableName, const nlohmann::ordered_json& tableDefinition)
	{
		std::string query = "ALTER TABLE " + tableName + " ";

		bool first = true;
		for (auto& [columnName, dataType] : tableDefinition.items())
		{
			if (!first)
			{
				query += ", ";
			}
			first = false;
			query += "ADD COLUMN " + columnName + " " + dataType.get<std::string>();
		}

		query += ";";

		return executeQuery(query);
	}

	bool reorderColumns(const std::string& tableName, const nlohmann::ordered_json& tableDefinition)
	{
		std::string query = "ALTER TABLE " + tableName + " ";

		bool first = true;
		std::string prev_column = "";
		for (auto& [columnName, dataType] : tableDefinition.items())
		{
			if (!first)
			{
				query += ", MODIFY " + columnName + " " + dataType.get<std::string>() + " AFTER " + prev_column;
			}
			else
			{
				query += " MODIFY " + columnName + " " + dataType.get<std::string>() + " FIRST";
				first = false;
			}
			prev_column = columnName;
		}

		return executeQuery(query);
	}

	bool create(const std::string& tableName, const std::vector<std::pair<std::string, std::string>>& data)
	{
		std::string query = "INSERT INTO " + tableName + " (";
		std::string values = " VALUES (";

		for (size_t i = 0; i < data.size(); i++)
		{
			query += data[i].first;
			values += "'" + escapeString(data[i].second) + "'";
			if (i < data.size() - 1)
			{
				query += ", ";
				values += ", ";
			}
		}

		query += ") " + values + ")";

		return executeQuery(query);
	}

	bool read(const std::string& query, std::vector<std::map<std::string, std::string>>& results)
	{
		if (!executeQuery(query)) return false;

		result = mysql_store_result(conn);
		if (result == nullptr)
		{
			printError("Store result failed");
			return false;
		}

		int num_fields = mysql_num_fields(result);

		MYSQL_ROW row;
		MYSQL_FIELD* fields = mysql_fetch_fields(result);

		while (true)
		{
			row = mysql_fetch_row(result);
			if (!row) break;

			std::map<std::string, std::string> rowData;

			for (int i = 0; i < num_fields; i++)
			{
				std::string column_name = fields[i].name;
				rowData[column_name] = row[i] ? row[i] : "NULL";
			}

			results.push_back(rowData);
		}

		mysql_free_result(result);

		return true;
	}

	bool update(const std::string& tableName, const std::vector<std::pair<std::string, std::string>>& data, const std::string& condition)
	{
		std::string query = "UPDATE " + tableName + " SET ";

		for (size_t i = 0; i < data.size(); ++i)
		{
			query += data[i].first + " = '" + escapeString(data[i].second) + "'";
			if (i < data.size() - 1)
			{
				query += ", ";
			}
		}

		query += " WHERE " + condition;

		return executeQuery(query);
	}

	bool deleteRow(const std::string& tableName, const std::string& condition)
	{
		std::string query = "DELETE FROM " + tableName + " WHERE " + condition;

		return executeQuery(query);
	}


	std::string escapeString(const std::string& value)
	{
		char* escaped = new char[value.length() * 2 + 1];
		mysql_real_escape_string(conn, escaped, value.c_str(), value.length());
		std::string escapeStr(escaped);
		delete[] escaped;

		return escapeStr;
	}

private:
	MYSQL* conn;
	MYSQL_RES* result;
	std::string _host;
	std::string _user;
	std::string _password;
	std::string _dbname;
	unsigned int _port;
	int _ssl_mode;
	std::string _tls_version;

	void configureSSL(int ssl_mode, const std::string& tls_version)
	{
		if (ssl_mode >= 0 && ssl_mode <= 4)
		{
			mysql_options(conn, MYSQL_OPT_SSL_MODE, &ssl_mode);
		}
		else
		{
            // Replace the problematic line with the following:
            int ssl_mode_disabled = SSL_MODE_DISABLED;
            mysql_options(conn, MYSQL_OPT_SSL_MODE, &ssl_mode_disabled);
		}

		if (!tls_version.empty())
		{
			mysql_options(conn, MYSQL_OPT_TLS_VERSION, tls_version.c_str());
		}
	}
	void printError(const std::string& message)
	{
		Log::GetLog()->error("{} {}", message, mysql_error(conn));
	}
	bool executeQuery(const std::string& query)
	{
		if (mysql_query(conn, query.c_str()))
		{
			printError("Queryfailed. Retrying to reconnect.");

			unsigned int errorCode = mysql_errno(conn);
			if (errorCode == 2003) // Lost connection or can't connect to server
			{
				mysql_close(conn);

				if (!MySQLConnect()) return false;
			}

			if (mysql_query(conn, query.c_str()))
			{
				return false;
			}
		}

		return true;
	}
	bool MySQLConnect()
	{
		conn = mysql_init(nullptr);
		if (!conn)
		{
			printError("mysql_init() failed");
			return false;
		}

		//configureSSL(_ssl_mode, _tls_version);

		if (!mysql_real_connect(conn, _host.c_str(), _user.c_str(), _password.c_str(), _dbname.c_str(), _port, nullptr, 0))
		{
			printError("Connection failed.");
			return false;
		}

		return true;
	}

};

#endif