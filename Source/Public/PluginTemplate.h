#pragma once

#include "json.hpp"

#include "Database/DatabaseFactory.h"

#include "Requests.h"

/**
 * @namespace PluginTemplate
 * @brief Espacio de nombres principal para el plugin de Plantilla de Plugin.
 * 
 * Este espacio de nombres contiene todas las variables globales y configuraciones
 * utilizadas por el plugin de Plantilla de Plugin para ARK: Survival Evolved.
 */
namespace PluginTemplate
{
	/**
	 * @var config
	 * @brief Configuración del plugin cargada desde el archivo config.json.
	 * 
	 * Esta variable almacena toda la configuración del plugin, incluyendo
	 * comandos, mensajes, configuraciones de base de datos y grupos de permisos.
	 */
	inline nlohmann::json config;
	
	/**
	 * @var isDebug
	 * @brief Bandera para habilitar o deshabilitar el modo de depuración.
	 * 
	 * Cuando está activa, se mostrarán mensajes de registro adicionales
	 * para ayudar en el diagnóstico de problemas.
	 */
	inline bool isDebug{ false };

	/**
	 * @var counter
	 * @brief Contador utilizado para temporizadores y eventos periódicos.
	 * 
	 * Este contador se incrementa cada cierto tiempo y se utiliza para
	 * ejecutar acciones programadas como anuncios o verificaciones periódicas.
	 */
	inline int counter = 0;

	/**
	 * @var pluginTemplateDB
	 * @brief Conector a la base de datos principal del plugin.
	 * 
	 * Puntero único a un conector de base de datos que se utiliza para
	 * almacenar y recuperar datos específicos del plugin.
	 */
	inline std::unique_ptr<IDatabaseConnector> pluginTemplateDB;

	/**
	 * @var permissionsDB
	 * @brief Conector a la base de datos de permisos.
	 * 
	 * Puntero único a un conector de base de datos que se utiliza para
	 * verificar y gestionar los permisos de los jugadores.
	 */
	inline std::unique_ptr<IDatabaseConnector> permissionsDB;

	/**
	 * @var pointsDB
	 * @brief Conector a la base de datos de puntos (ArkShop).
	 * 
	 * Puntero único a un conector de base de datos que se utiliza para
	 * gestionar el sistema de puntos del plugin ArkShop.
	 */
	inline std::unique_ptr<IDatabaseConnector> pointsDB;

	/**
	 * @var req
	 * @brief Instancia del sistema de solicitudes HTTP.
	 * 
	 * Referencia estática al sistema de solicitudes utilizado para
	 * realizar llamadas HTTP/HTTPS a servicios externos.
	 */
	static API::Requests& req = API::Requests::Get();

	/**
	 * @var lastMessageID
	 * @brief ID del último mensaje procesado de Discord.
	 * 
	 * Se utiliza para evitar procesar el mismo mensaje de Discord
	 * múltiples veces, asegurando que cada mensaje se procese solo una vez.
	 */
	inline std::string lastMessageID;

}