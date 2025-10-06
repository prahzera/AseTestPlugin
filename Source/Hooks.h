/**
 * @brief Declaración del gancho para el evento de muerte de personaje.
 * 
 * Este gancho intercepta el evento cuando un personaje jugador muere
 * en el juego, permitiendo realizar acciones personalizadas como
 * registrar la muerte o modificar el comportamiento por defecto.
 */
DECLARE_HOOK(AShooterCharacter_Die, bool, AShooterCharacter*, float, FDamageEvent*, AController*, AActor*);

/**
 * @brief Implementación del gancho para el evento de muerte de personaje.
 * 
 * Esta función se ejecuta cuando un personaje jugador muere en el juego.
 * Registra el nombre del jugador que murió en el registro del servidor
 * y luego llama a la implementación original del evento.
 * 
 * @param shooter_character Puntero al personaje que murió.
 * @param KillingDamage Cantidad de daño que causó la muerte.
 * @param DamageEvent Evento de daño asociado a la muerte.
 * @param Killer Controlador del personaje que causó la muerte.
 * @param DamageCauser Actor que causó el daño.
 * @return bool Valor booleano indicando si la muerte se procesó correctamente.
 */
bool Hook_AShooterCharacter_Die(AShooterCharacter* shooter_character, float KillingDamage, FDamageEvent* DamageEvent, AController* Killer, AActor* DamageCauser)
{
	FString playername = shooter_character->PlayerNameField();

	Log::GetLog()->info("Jugador: {}, ¡Muere!", playername.ToString());

	return AShooterCharacter_Die_original(shooter_character, KillingDamage, DamageEvent, Killer, DamageCauser);
}

/**
 * @brief Configura o elimina los ganchos del plugin.
 * 
 * Esta función se encarga de registrar o desregistrar los ganchos
 * utilizados por el plugin. Los ganchos permiten interceptar y
 * modificar el comportamiento de eventos del juego.
 * 
 * @param addHooks Bandera que indica si se deben agregar (true) o eliminar (false) los ganchos.
 */
void SetHooks(bool addHooks = true)
{
	if (addHooks)
	{
		ArkApi::GetHooks().SetHook("AShooterCharacter.Die(float,FDamageEvent&,AController*,AActor*)", &Hook_AShooterCharacter_Die, &AShooterCharacter_Die_original);
	}
	else
	{
		ArkApi::GetHooks().DisableHook("AShooterCharacter.Die(float,FDamageEvent&,AController*,AActor*)", &Hook_AShooterCharacter_Die);
	}
}