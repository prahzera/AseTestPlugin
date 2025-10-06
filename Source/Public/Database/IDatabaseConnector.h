#ifndef IDATABASECONNECTOR_H
#define IDATABASECONNECTOR_H

// Añadir esto para solucionar errores de redefinición
#define WIN32_LEAN_AND_MEAN // requerido al incluir API/ARK/Ark.h

#pragma once

#include <json.hpp>
#include <string>
#include <vector>

#include <API/ARK/Ark.h> // requerido en GetLog y Log

/**
 * @class IDatabaseConnector
 * @brief Interfaz abstracta para conectores de bases de datos.
 * 
 * Esta clase define la interfaz común que deben implementar todos
 * los conectores de bases de datos utilizados por el plugin.
 * Proporciona métodos para crear, leer, actualizar y eliminar datos
 * en diferentes tipos de bases de datos (MySQL, SQLite, etc.).
 */
class IDatabaseConnector
{
public:
	virtual ~IDatabaseConnector() = default;

	/**
	 * @brief Crea una tabla si no existe.
	 * 
	 * Este método crea una tabla en la base de datos con la definición
	 * especificada si la tabla aún no existe.
	 * 
	 * @param tableName Nombre de la tabla a crear.
	 * @param tableDefinition Definición de la estructura de la tabla en formato JSON.
	 * @return true si la operación fue exitosa, false en caso contrario.
	 */
	virtual bool createTableIfNotExist(const std::string& tableName, const nlohmann::ordered_json& tableDefinition) = 0;

	/**
	 * @brief Modifica una tabla si no existe una columna específica.
	 * 
	 * Este método añade columnas a una tabla existente si dichas
	 * columnas aún no existen en la tabla.
	 * 
	 * @param tableName Nombre de la tabla a modificar.
	 * @param tableDefinition Definición de las columnas a añadir en formato JSON.
	 * @return true si la operación fue exitosa, false en caso contrario.
	 */
	virtual bool alterTableIfNotExists(const std::string& tableName, const nlohmann::ordered_json& tableDefinition) = 0;

	/**
	 * @brief Reordena las columnas de una tabla.
	 * 
	 * Este método reordena las columnas de una tabla según
	 * la definición especificada.
	 * 
	 * @param tableName Nombre de la tabla a reordenar.
	 * @param tableDefinition Definición del nuevo orden de columnas en formato JSON.
	 * @return true si la operación fue exitosa, false en caso contrario.
	 */
	virtual bool reorderColumns(const std::string& tableName, const nlohmann::ordered_json& tableDefinition) = 0;

	/**
	 * @brief Crea un nuevo registro en una tabla.
	 * 
	 * Este método inserta un nuevo registro en la tabla especificada
	 * con los datos proporcionados.
	 * 
	 * @param tableName Nombre de la tabla donde insertar el registro.
	 * @param data Vector de pares clave-valor con los datos a insertar.
	 * @return true si la operación fue exitosa, false en caso contrario.
	 */
	virtual bool create(const std::string& tableName, const std::vector<std::pair<std::string, std::string>>& data) = 0;

	/**
	 * @brief Lee datos de la base de datos.
	 * 
	 * Este método ejecuta una consulta SELECT y devuelve los resultados
	 * en un vector de mapas.
	 * 
	 * @param query Consulta SQL a ejecutar.
	 * @param results Vector donde se almacenarán los resultados.
	 * @return true si la operación fue exitosa, false en caso contrario.
	 */
	virtual bool read(const std::string& query, std::vector<std::map<std::string,std::string>>& results) = 0;

	/**
	 * @brief Actualiza registros en una tabla.
	 * 
	 * Este método actualiza registros en la tabla especificada
	 * según la condición proporcionada.
	 * 
	 * @param tableName Nombre de la tabla a actualizar.
	 * @param data Vector de pares clave-valor con los datos a actualizar.
	 * @param condition Condición SQL para determinar qué registros actualizar.
	 * @return true si la operación fue exitosa, false en caso contrario.
	 */
	virtual bool update(const std::string& tableName, const std::vector<std::pair<std::string, std::string>>& data, const std::string& condition) = 0;

	/**
	 * @brief Elimina registros de una tabla.
	 * 
	 * Este método elimina registros de la tabla especificada
	 * según la condición proporcionada.
	 * 
	 * @param tableName Nombre de la tabla de la cual eliminar registros.
	 * @param condition Condición SQL para determinar qué registros eliminar.
	 * @return true si la operación fue exitosa, false en caso contrario.
	 */
	virtual bool deleteRow(const std::string& tableName, const std::string& condition) = 0;

	/**
	 * @brief Escapa una cadena para prevenir inyecciones SQL.
	 * 
	 * Este método procesa una cadena para hacerla segura
	 * para su uso en consultas SQL, reemplazando caracteres
	 * especiales por sus equivalentes escapados.
	 * 
	 * @param value Cadena a escapar.
	 * @return Cadena escapada y segura para usar en consultas SQL.
	 */
	virtual std::string escapeString(const std::string& value) = 0;
};


#endif //IDATABASECONNECTOR_H