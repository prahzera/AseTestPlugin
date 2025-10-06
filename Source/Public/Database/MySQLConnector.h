#ifndef MYSQLCONNECTOR_H
#define MYSQLCONNECTOR_H

#pragma once

#include <mysql/mysql.h>
#include "IDatabaseConnector.h"


#pragma comment(lib,"mysqlclient.lib")

/**
 * @class MySQLConnector
 * @brief Conector específico para bases de datos MySQL.
 * 
 * Esta clase implementa la interfaz IDatabaseConnector para
 * proporcionar funcionalidad de conexión y operaciones con
 * bases de datos MySQL. Gestiona la conexión, ejecución de
 * consultas y procesamiento de resultados.
 */
class MySQLConnector : public IDatabaseConnector
{
public:
	/**
	 * @brief Constructor que inicializa la conexión a MySQL.
	 * 
	 * Este constructor establece los parámetros de conexión
	 * y trata de conectarse a la base de datos MySQL.
	 * 
	 * @param host Dirección del servidor MySQL.
	 * @param user Nombre de usuario para la conexión.
	 * @param password Contraseña para la conexión.
	 * @param dbname Nombre de la base de datos a utilizar.
	 * @param port Puerto del servidor MySQL.
	 * @param ssl_mode Modo SSL para la conexión.
	 * @param tls_version Versión TLS a utilizar.
	 */
	MySQLConnector(const std::string& host, const std::string& user, const std::string& password, const std::string& dbname, unsigned int port, int ssl_mode, const std::string& tls_version)
	{
		if (!MySQLConnect()) return;
	}

	/**
	 * @brief Destructor que cierra la conexión a MySQL.
	 * 
	 * Este destructor se asegura de cerrar adecuadamente
	 * la conexión a la base de datos MySQL al destruir
	 * la instancia del conector.
	 */
	~MySQLConnector()
	{
		if (conn != nullptr)
		{
			mysql_close(conn);
		}
	}

	/**
	 * @brief Crea una tabla si no existe.
	 * 
	 * Este método crea una tabla en la base de datos MySQL
	 * con la definición especificada si la tabla aún no existe.
	 * 
	 * @param tableName Nombre de la tabla a crear.
	 * @param tableDefinition Definición de la estructura de la tabla en formato JSON.
	 * @return true si la operación fue exitosa, false en caso contrario.
	 */
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

	/**
	 * @brief Modifica una tabla si no existe una columna específica.
	 * 
	 * Este método añade columnas a una tabla existente en MySQL
	 * si dichas columnas aún no existen en la tabla.
	 * 
	 * @param tableName Nombre de la tabla a modificar.
	 * @param tableDefinition Definición de las columnas a añadir en formato JSON.
	 * @return true si la operación fue exitosa, false en caso contrario.
	 */
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

	/**
	 * @brief Reordena las columnas de una tabla.
	 * 
	 * Este método reordena las columnas de una tabla en MySQL
	 * según la definición especificada.
	 * 
	 * @param tableName Nombre de la tabla a reordenar.
	 * @param tableDefinition Definición del nuevo orden de columnas en formato JSON.
	 * @return true si la operación fue exitosa, false en caso contrario.
	 */
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

	/**
	 * @brief Crea un nuevo registro en una tabla.
	 * 
	 * Este método inserta un nuevo registro en la tabla especificada
	 * en la base de datos MySQL con los datos proporcionados.
	 * 
	 * @param tableName Nombre de la tabla donde insertar el registro.
	 * @param data Vector de pares clave-valor con los datos a insertar.
	 * @return true si la operación fue exitosa, false en caso contrario.
	 */
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

	/**
	 * @brief Lee datos de la base de datos MySQL.
	 * 
	 * Este método ejecuta una consulta SELECT en la base de datos MySQL
	 * y devuelve los resultados en un vector de mapas.
	 * 
	 * @param query Consulta SQL a ejecutar.
	 * @param results Vector donde se almacenarán los resultados.
	 * @return true si la operación fue exitosa, false en caso contrario.
	 */
	bool read(const std::string& query, std::vector<std::map<std::string, std::string>>& results)
	{
		if (!executeQuery(query)) return false;

		result = mysql_store_result(conn);
		if (result == nullptr)
		{
			printError("Fallo al almacenar resultados");
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

	/**
	 * @brief Actualiza registros en una tabla.
	 * 
	 * Este método actualiza registros en la tabla especificada
	 * en la base de datos MySQL según la condición proporcionada.
	 * 
	 * @param tableName Nombre de la tabla a actualizar.
	 * @param data Vector de pares clave-valor con los datos a actualizar.
	 * @param condition Condición SQL para determinar qué registros actualizar.
	 * @return true si la operación fue exitosa, false en caso contrario.
	 */
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

	/**
	 * @brief Elimina registros de una tabla.
	 * 
	 * Este método elimina registros de la tabla especificada
	 * en la base de datos MySQL según la condición proporcionada.
	 * 
	 * @param tableName Nombre de la tabla de la cual eliminar registros.
	 * @param condition Condición SQL para determinar qué registros eliminar.
	 * @return true si la operación fue exitosa, false en caso contrario.
	 */
	bool deleteRow(const std::string& tableName, const std::string& condition)
	{
		std::string query = "DELETE FROM " + tableName + " WHERE " + condition;

		return executeQuery(query);
	}


	/**
	 * @brief Escapa una cadena para prevenir inyecciones SQL.
	 * 
	 * Este método procesa una cadena para hacerla segura
	 * para su uso en consultas SQL de MySQL, utilizando
	 * la función mysql_real_escape_string.
	 * 
	 * @param value Cadena a escapar.
	 * @return Cadena escapada y segura para usar en consultas SQL.
	 */
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

	/**
	 * @brief Configura las opciones SSL para la conexión MySQL.
	 * 
	 * Este método configura las opciones SSL/TLS para la conexión
	 * a la base de datos MySQL según los parámetros especificados.
	 * 
	 * @param ssl_mode Modo SSL a configurar.
	 * @param tls_version Versión TLS a utilizar.
	 */
	void configureSSL(int ssl_mode, const std::string& tls_version)
	{
		if (ssl_mode >= 0 && ssl_mode <= 4)
		{
			mysql_options(conn, MYSQL_OPT_SSL_MODE, &ssl_mode);
		}
		else
		{
            // Reemplazar la línea problemática con lo siguiente:
            int ssl_mode_disabled = SSL_MODE_DISABLED;
            mysql_options(conn, MYSQL_OPT_SSL_MODE, &ssl_mode_disabled);
		}

		if (!tls_version.empty())
		{
			mysql_options(conn, MYSQL_OPT_TLS_VERSION, tls_version.c_str());
		}
	}
	
	/**
	 * @brief Imprime un mensaje de error en el registro.
	 * 
	 * Este método registra un mensaje de error junto con
	 * el mensaje de error específico de MySQL.
	 * 
	 * @param message Mensaje de error personalizado.
	 */
	void printError(const std::string& message)
	{
		Log::GetLog()->error("{} {}", message, mysql_error(conn));
	}
	
	/**
	 * @brief Ejecuta una consulta SQL en la base de datos MySQL.
	 * 
	 * Este método ejecuta una consulta SQL y maneja la reconexión
	 * automática en caso de pérdida de conexión.
	 * 
	 * @param query Consulta SQL a ejecutar.
	 * @return true si la consulta se ejecutó exitosamente, false en caso contrario.
	 */
	bool executeQuery(const std::string& query)
	{
		if (mysql_query(conn, query.c_str()))
		{
			printError("Consulta fallida. Reintentando reconexión.");

			unsigned int errorCode = mysql_errno(conn);
			if (errorCode == 2003) // Conexión perdida o no se puede conectar al servidor
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
	
	/**
	 * @brief Establece la conexión con la base de datos MySQL.
	 * 
	 * Este método inicializa la conexión con el servidor MySQL
	 * utilizando los parámetros de configuración proporcionados.
	 * 
	 * @return true si la conexión se estableció exitosamente, false en caso contrario.
	 */
	bool MySQLConnect()
	{
		conn = mysql_init(nullptr);
		if (!conn)
		{
			printError("mysql_init() falló");
			return false;
		}

		configureSSL(_ssl_mode, _tls_version);

		if (!mysql_real_connect(conn, _host.c_str(), _user.c_str(), _password.c_str(), _dbname.c_str(), _port, nullptr, 0))
		{
			printError("Conexión fallida.");
			return false;
		}

		return true;
	}

};

#endif