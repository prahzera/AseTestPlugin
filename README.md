# Plantilla de Plugin para AseApi

Esta es una plantilla para crear plugins personalizados para servidores ARK: Survival Evolved usando AseApi.

## Estructura del Proyecto

### 📁 Build/
Directorio para archivos compilados del plugin.

### 📁 Configs/
Archivos de configuración del plugin:
- `PluginInfo.json`: Metadatos del plugin
- `config.json`: Archivo de configuración principal
- `config-commented.json`: Configuración con comentarios explicativos

### 📁 Includes/
Encabezados necesarios para la compilación:
- Librerías MySQL para conectividad de base de datos
- Encabezados OpenSSL

### 📁 Libs/
Librerías compiladas requeridas por el plugin:
- `libmysql.lib`: Librería cliente MySQL
- `mysqlclient.lib`: Cliente MySQL completo

### 📁 Source/
Código fuente del plugin organizado en:

#### Archivo Principal
- `PluginTemplate.cpp`: Punto de entrada principal del plugin que inicializa y descarga el plugin, establece hooks y llama a las funciones de inicialización.

#### Componentes del Plugin
- `Commands.h`: Implementación de comandos del servidor como `/repairitems` y `/deleteplayer`
- `Hooks.h`: Hooks para interceptar eventos del juego, como la muerte de personajes
- `Reload.h`: Funcionalidad para recargar la configuración del plugin sin reiniciar
- `RepairItems.h`: Sistema de reparación de ítems del inventario de jugadores
- `Timers.h`: Gestión de temporizadores para ejecutar tareas periódicas
- `Utils.h`: Funciones utilitarias para manejo de bases de datos, permisos, puntos y configuración

#### 📁 Source/Database/
Conectores de base de datos:
- `MySQLConnector.cpp/.h`: Conector para bases de datos MySQL con funciones CRUD completas
- `SQLiteConnector.cpp/.h`: Conector para bases de datos SQLite con funciones CRUD completas
- `DatabaseFactory.h`: Fábrica para crear instancias de conectores según la configuración
- `IDatabaseConnector.h`: Interfaz común para todos los conectores con métodos como `createTableIfNotExist`, `create`, `read`, `update`, `deleteRow` y `escapeString`

#### 📁 Source/Public/
Interfaces públicas y encabezados compartidos:
- `PluginTemplate.h`: Declaraciones principales del plugin, incluyendo variables globales como la configuración, conectores de base de datos y contadores

### 📁 Source/Public/Database/
Encabezados de base de datos públicos:
- `sqlite3/`: Librería SQLite embebida
- Varios archivos de encabezado para conectores de base de datos

## Descripción General

Este proyecto proporciona una plantilla completa para desarrollar plugins personalizados para servidores ARK utilizando AseApi. Incluye:

1. **Arquitectura modular** - Código organizado en componentes reutilizables
2. **Soporte de múltiples bases de datos** - Conectores para MySQL y SQLite con una interfaz común
3. **Sistema de comandos** - Implementación lista para usar de comandos de servidor
4. **Hooks del juego** - Puntos de extensión para interceptar eventos del juego
5. **Gestión de configuración** - Sistema flexible de configuración basado en JSON
6. **Funcionalidades útiles** - Reparación de ítems, temporizadores, utilidades varias
7. **Sistema de permisos** - Integración con sistemas de permisos basados en grupos
8. **Sistema de puntos** - Integración con sistemas de puntos (como ArkShop)

## Detalles del Código

### Punto de Entrada (`PluginTemplate.cpp`)
El archivo principal establece hooks en `AShooterGameMode.BeginPlay` para inicializar el plugin cuando el servidor está listo. También maneja la inicialización y descarga del plugin.

### Gestión de Configuración (`Utils.h`)
La función `ReadConfig()` carga la configuración desde `config.json` y la almacena en la variable global `PluginTemplate::config`. La función `LoadDatabase()` inicializa los conectores de base de datos según la configuración.

### Comandos (`Commands.h` y `RepairItems.h`)
Los comandos se registran dinámicamente desde la configuración. El comando `/repairitems` repara todos los ítems del inventario del jugador, mientras que `/deleteplayer` elimina los datos del jugador de la base de datos.

### Hooks (`Hooks.h`)
Los hooks permiten interceptar eventos del juego. El ejemplo muestra cómo interceptar la muerte de un personaje para registrar información en los logs.

### Temporizadores (`Timers.h`)
Los temporizadores ejecutan funciones periódicamente. El ejemplo muestra cómo enviar notificaciones a todos los jugadores en intervalos específicos.

### Base de Datos (`Database/`)
Los conectores implementan la interfaz `IDatabaseConnector` y proporcionan funciones CRUD completas. La `DatabaseFactory` crea la instancia apropiada según la configuración.

## Requisitos del API

### AseApi (Ark Server Extension API)
Esta plantilla requiere **AseApi** para funcionar. AseApi no está incluido en este proyecto y debe instalarse por separado en el servidor ARK.

**AseApi debe instalarse en el servidor:**
1. Descargar e instalar AseApi siguiendo las instrucciones oficiales
2. Colocar los archivos del plugin compilado en `ArkApi/Plugins/NombreDelPlugin/`
3. Asegurarse de que las dependencias (`ArkApi.lib`) están disponibles durante la compilación

### Dependencias Incluidas
- **SQLite**: Incluida como librería embebida en `Source/Public/Database/sqlite3/`
- **MySQL**: Las librerías cliente (`mysqlclient.lib`) se incluyen en la carpeta `Libs/`

### Dependencias del Sistema
- Visual Studio o entorno compatible con MSBuild
- vcpkg para gestión de paquetes (configurado en `vcpkg.json`)
- Windows SDK

## Instalación y Configuración

### vcpkg
**¡Buenas noticias!** vcpkg ya está instalado y configurado en tu proyecto. Puedes ver que:
- El archivo `vcpkg.json` define las dependencias del proyecto (actualmente solo "fmt")
- El directorio `vcpkg_installed` contiene las librerías instaladas
- La configuración del proyecto en `PluginTemplate.vcxproj` ya está configurada para usar vcpkg

No necesitas realizar ninguna instalación adicional de vcpkg.

### AseApi - Instalación Requerida
A diferencia de vcpkg, **AseApi no está incluido en este proyecto** y debes instalarlo por separado. Sin embargo, veo que ya tienes AseApi instalado en tu sistema:

✅ **AseApi encontrado**: `I:\dev\ARK\AseApi`

El proyecto ya está configurado para usar esta ubicación, por lo que no necesitas realizar ninguna configuración adicional.

### Compilación

#### Requisitos Previos
- Visual Studio 2022 o superior
- vcpkg (ya instalado en el proyecto)
- AseApi ya está disponible en tu sistema en `I:\dev\ARK\AseApi`

#### Proceso de Compilación
1. Abrir `PluginTemplate.sln` en Visual Studio
2. Seleccionar la configuración "Release" y plataforma "x64"
3. Compilar la solución (Ctrl+Shift+B)
4. El plugin compilado se generará en la carpeta `Build/` y se copiará automáticamente a `Output/`

#### Solución de Problemas Comunes
- Si obtienes errores de enlazado con `ArkApi.lib`, verifica que AseApi esté correctamente instalado
- Si hay errores de dependencias, Visual Studio debería restaurar automáticamente los paquetes vcpkg
- Asegúrate de que estás usando la configuración "Release" y plataforma "x64"

### Configuración del Proyecto
- Las dependencias se configuran en `PluginTemplate.vcxproj`
- Las rutas de inclusión se definen en las propiedades del proyecto
- Las librerías se enlazan automáticamente durante la compilación

## Contribución

Esta plantilla está diseñada para acelerar el desarrollo de plugins personalizados para ARK. Siéntete libre de modificar cualquier parte del código para adaptarlo a tus necesidades específicas.