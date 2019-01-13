#ifndef gameState_h
#define gameState_h

enum State{
	STARTUP,
	MAIN_MENU,
	GAME,
	PAUSE,
	GAME_OVER
};

enum activeWeapon{
	//GRENADE,
	//MISSILE,
	NA
};


struct gameState{
	State state;
	activeWeapon weapon;
};

gameState* allocGameState()
{
	gameState* sta = (gameState*)calloc(1, sizeof(gameState));
	if (sta != NULL)
	{
		sta->state = STARTUP;
		sta->weapon = NA;
	}
	return sta;
}

void setState(gameState* sta, State state){
	sta->state = state;
}

State getState(gameState* sta){
	return sta->state;
}

#endif