#ifndef SQLITECONNECTOR_H
#define SQLITECONNECTOR_H

#pragma once

#include "IDatabaseConnector.h"
#include "sqlite3/sqlite3.h"

/**
 * @class SQLiteConnector
 * @brief Conector específico para bases de datos SQLite.
 * 
 * Esta clase implementa la interfaz IDatabaseConnector para
 * proporcionar funcionalidad de conexión y operaciones con
 * bases de datos SQLite. Gestiona la conexión, ejecución de
 * consultas y procesamiento de resultados.
 */
class SQLiteConnector : public IDatabaseConnector
{
public:
	/**
	 * @brief Constructor que inicializa la conexión a SQLite.
	 * 
	 * Este constructor establece la ruta de la base de datos SQLite
	 * y trata de abrir la conexión con la base de datos.
	 * 
	 * @param dbName Ruta al archivo de la base de datos SQLite.
	 */
	SQLiteConnector(const std::string& dbName)
	{
		if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK)
		{
			printError("Fallo al abrir la base de datos SQLite");
			db = nullptr;
		}
	}
	
	/**
	 * @brief Destructor que cierra la conexión a SQLite.
	 * 
	 * Este destructor se asegura de cerrar adecuadamente
	 * la conexión a la base de datos SQLite al destruir
	 * la instancia del conector.
	 */
	~SQLiteConnector()
	{
		if (db != nullptr)
		{
			sqlite3_close(db);
		}
	}


	/**
	 * @brief Crea una tabla si no existe.
	 * 
	 * Este método crea una tabla en la base de datos SQLite
	 * con la definición especificada si la tabla aún no existe.
	 * 
	 * @param tableName Nombre de la tabla a crear.
	 * @param tableDefinition Definición de la estructura de la tabla en formato JSON.
	 * @return true si la operación fue exitosa, false en caso contrario.
	 */
	bool createTableIfNotExist(const std::string& tableName, const nlohmann::ordered_json& tableDefinition)
	{
		std::string query = "CREATE TABLE IF NOT EXISTS " + tableName + " (";

		bool first = true;
		for (auto& [columnName, dataType] : tableDefinition.items())
		{
			if (!first)
			{
				query += ", ";
			}
			first = false;
			query += columnName + " " + dataType.get<std::string>();
		}

		query += ")";

		return executeQuery(query);
	}

	/**
	 * @brief Modifica una tabla si no existe una columna específica.
	 * 
	 * Este método añade columnas a una tabla existente en SQLite
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
	 * Este método está deshabilitado para SQLite ya que
	 * SQLite no soporta reordenamiento de columnas de la misma
	 * manera que otros sistemas de bases de datos.
	 * 
	 * @param tableName Nombre de la tabla (no utilizado).
	 * @param tableDefinition Definición de tabla (no utilizada).
	 * @return Siempre retorna false ya que la operación no está soportada.
	 */
	bool reorderColumns(const std::string& tableName, const nlohmann::ordered_json& tableDefinition)
	{
		return false;
	}

	/**
	 * @brief Crea un nuevo registro en una tabla.
	 * 
	 * Este método inserta un nuevo registro en la tabla especificada
	 * en la base de datos SQLite con los datos proporcionados.
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
	 * @brief Lee datos de la base de datos SQLite.
	 * 
	 * Este método ejecuta una consulta SELECT en la base de datos SQLite
	 * y devuelve los resultados en un vector de mapas.
	 * 
	 * @param query Consulta SQL a ejecutar.
	 * @param results Vector donde se almacenarán los resultados.
	 * @return true si la operación fue exitosa, false en caso contrario.
	 */
	bool read(const std::string& query, std::vector<std::map<std::string, std::string>>& results)
	{
		sqlite3_stmt* stmt;

		if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
		{
			printError("Fallo al preparar la sentencia SQLite");
			return false;
		}

		int numCols = sqlite3_column_count(stmt);
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			std::map<std::string, std::string> rowData;

			for (int i = 0; i < numCols; i++)
			{
				const char* columnName = sqlite3_column_name(stmt, i);
				const char* val = (const char*)sqlite3_column_text(stmt, i);
				rowData[columnName] = val ? val : "NULL";
			}
			results.push_back(rowData);
		}

		sqlite3_finalize(stmt);
		return true;
	}

	/**
	 * @brief Actualiza registros en una tabla.
	 * 
	 * Este método actualiza registros en la tabla especificada
	 * en la base de datos SQLite según la condición proporcionada.
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
	 * en la base de datos SQLite según la condición proporcionada.
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
	 * para su uso en consultas SQL de SQLite, reemplazando
	 * caracteres especiales por sus equivalentes escapados.
	 * 
	 * @param value Cadena a escapar.
	 * @return Cadena escapada y segura para usar en consultas SQL.
	 */
	std::string escapeString(const std::string& value)
	{
		std::string escaped;
		escaped.reserve(value.length());

		for (char c : value)
		{
			switch (c)
			{
			case '\'':
				escaped += "''";
				break;
			case '\"':
				escaped += "\"";
				break;
			case '\\':
				escaped += "\\";
				break;
			default:
				escaped += c;
				break;
			}
		}

		return escaped;
	}

private:
	sqlite3* db;
	char* errorMessage;

	/**
	 * @brief Imprime un mensaje de error en el registro.
	 * 
	 * Este método registra un mensaje de error junto con
	 * el mensaje de error específico de SQLite.
	 * 
	 * @param message Mensaje de error personalizado.
	 */
	void printError(const std::string& message)
	{
		Log::GetLog()->error("{} {}", message, sqlite3_errmsg(db));
	}
	
	/**
	 * @brief Ejecuta una consulta SQL en la base de datos SQLite.
	 * 
	 * Este método ejecuta una consulta SQL utilizando la API
	 * de SQLite y maneja los errores apropiadamente.
	 * 
	 * @param query Consulta SQL a ejecutar.
	 * @return true si la consulta se ejecutó exitosamente, false en caso contrario.
	 */
	bool executeQuery(const std::string& query)
	{
		int rc = sqlite3_exec(db, query.c_str(), 0, 0, &errorMessage);
		if (rc != SQLITE_OK)
		{
			printError(std::string("Error en consulta SQLite: ") + errorMessage);

			sqlite3_free(errorMessage);
			return false;
		}

		return true;
	}

};

#endif