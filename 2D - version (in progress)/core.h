#ifndef core_h
#define core_h
#define ATS_TILEMAP
#include "ats/ats_tool.h"
#include "ats/bitmaps.h"
#include "gameState.h"
#include "gameData.h"
#include "gameObject.h"
#include "gameItem.h"
#include "particle.h"


Render_Window Window;
Timer timer;
float time;
float delay;
float mapWarp;
float speed;

int HIGH_SCORE = 2661082;
				//10045391;
				//4113240
				//930249
int BEST_SCORE;
int LAST_SCORE;
int SCORE;

gameState* STATE;
gameObject* player;
Array<gameItem> items;


void coreInit(int w, int h, const char* title){
	Window = window_create(w, h, title, 1);
	timer = timer_create();
	delay = 5.0f;
	mapWarp = 0;
	mapInit();
	particles.reserve(2048);

	//STATE = allocGameState();
	player = allocGameObject(5, 20);
	activate(player);
	BEST_SCORE = 0;
	LAST_SCORE = 0;
	SCORE = 0;
	STATE = allocGameState();
	setState(STATE, STARTUP); 
}

void restart(){
	delay = 5.0f;
	mapWarp = 0;
	mapInit();
	restartGameObject(player);
	items.clear();
	restartItems();
	particles.clear();
	restartAnimation(0.7, 1);
	LAST_SCORE = SCORE;
	if(SCORE > HIGH_SCORE){
		printf("HIGH ");
		HIGH_SCORE = SCORE;
	}
	if(SCORE > BEST_SCORE){
		BEST_SCORE = SCORE;
	}
	printf("SCORE : %d\n", SCORE);
	SCORE = 0;
	setState(STATE, STARTUP);
}

int coreIsOpen(){
	if(window_is_open(Window))
		return 1;
	return 0;
}
void coreDestroy(){ window_destroy(Window);}

void stateUpdate(){
	if(getXpos(player) < 0.7 ||
		getYpos(player) < 1.7 ||
		getYpos(player) > 38.3){restart();}
	else if(getXpos(player) < 20){
		speed = 6;
		if(randf(0.0f, 1.0f) > 0.98)
			flashRed(60, 0.5f, 0.5f);
	}
	else if(getXpos(player) < 50){
		if(getXpos(player) < 30){speed = 8;}
		else if(getXpos(player) < 40){speed = 10;}
		else {speed = 12;}
		if(randf(0.0f, 1.0f) > 0.98)
			flashPurple(30, 0.5f, 0.3f);
	}
	else if(getXpos(player) < 80){
		if(getXpos(player) < 60){speed = 14;}
		else if(getXpos(player) < 70){speed = 16;}
		else {speed = 18;}
		if(randf(0.0f, 1.0f) > 0.98)
			flashRainbow(30, 0.5f, 0.3f);
	} else {speed = 100; setVel(player, -15.0f, getYvel(player)); SCORE += 10000; flashRainbow(50, 0.5f, 0.8f);}
	SCORE += (int)((time*(float)(speed*speed))*10.0f) + getScore();
	if(SCORE < 10000)
		speed *= 0.7f;
	else if(SCORE < 50000)
		speed *= 0.9f;
	else if(SCORE < 100000)
		speed *= 1.0f;
	else if(SCORE < 500000)
		speed *= 1.25f;
	else if(SCORE < 1000000)
		speed *= 1.5f;
	else if(SCORE < 5000000)
		speed *= 1.65f;
	else if(SCORE < 10000000)
		speed *= 1.85f;
	else
		speed *= 2.0f;
}

void keyEvents(){
	if(is_key_pressed(Window, D))
		move(player, 100, 0);
	if(is_key_pressed(Window, A))
		move(player, -100, 0);
	if(is_key_pressed(Window, W))
		move(player, 0, 100);
	if(is_key_pressed(Window, S))
		move(player, 0, -100);
	if(is_key_pressed(Window, R)){
		setVel(player, 0, 0); 
		setPos(player, 40, 20);}
	for(int i = 0; i < key_events.len(); i++){
		Key_Event* event = key_events.get(i);
		/*if(is_key_event(event, W, PRESS)){
			flipGravity(player);
			itemFlipGravity();
		}*/
		if(is_key_event(event, SPACE, PRESS)){
			if(shootClusterGrenade(player))
				items.add(createGameItem(getXpos(player), getYpos(player), getXvel(player), 0.25, CLUSTERGRENADE));
			else if(shootMissile(player))
				items.add(createGameItem(getXpos(player), getYpos(player), getXvel(player), 0.25, MISSILE));
			else if(shootGrenade(player))
				items.add(createGameItem(getXpos(player), getYpos(player), getXvel(player), 0.25, GRENADE));
		}
	}
}

void mapUpdate(){
	while(mapWarp >= 1){
		mapWarp -= 1;
		updateMap();
	}
}

void renderMap(){
	for(int y = 1; y < ytiles - 1; y++){
		for(int x = 0; x < xtiles; x++){
			if(tileType(x, y) == BLOCK){
				render_cube	(x+0.05-mapWarp, y+0.05, 
							x+0.95-mapWarp, y+0.95, 
							0.9, -0.1, 
							120, 50, 210, 255);
			} else {
				render_rectangle(x+0.05-mapWarp, y+0.05, 
								x+0.95-mapWarp, y+0.95, 0.0, 
								150, 0, 255, 35);
				if(tileType(x, y) == ITEM){
					setBlock(x, y, NO_BLOCK);
					if(randf(0.0f, 1.0f) > 0.95 && getState(STATE) == GAME)// 0.95 <---CHANGE TO!
						items.add(randomPowerUp(x, y));
					else
						items.add(randomCollectable(x, y));
				}
			}
		}
	}
	render_rectangle(0, -2, 
					80, 1, 0, 
					205, 0, 50, 255);
	render_rectangle(0, 39, 
					80, 42, 0, 
					205, 0, 50, 255);
	
}

void renderPlayer(){	
	updateObject(player, time, time*speed);
	render_rectangle(getXpos(player)-0.3, getYpos(player)-0.55, 
					getXpos(player)+0.3, getYpos(player)-0.3, 0.2, 
					255, 0, 200, 255);
	render_rectangle(getXpos(player)-0.55, getYpos(player)-0.3, 
					getXpos(player)+0.55, getYpos(player)+0.3, 0.2, 
					255, 0, 200, 255);
	render_rectangle(getXpos(player)-0.3, getYpos(player)+0.3, 
					getXpos(player)+0.3, getYpos(player)+0.55, 0.2, 
					255, 0, 200, 255);
	if(randf(0.0f, 1.0f) > 0.6){
		singleParticle({getXpos(player), getYpos(player)}, 
						{0, 0}, 
						{((float)randi(0,100)/100.0f-0.5f)*10.0f, ((float)randi(0,100)/100.0f-0.5f)*20.0f},
						randf(0.1f, 0.15f), ((float)randi(0,100)/100.0f-0.5f)*10.0f,
						0.2f, 1.0f, 0.0f,
						255, 0, randi(50, 150), 0.3f);
	}
	if(randf(0.0f, 1.0f) > 0.97){
		singleParticle({getXpos(player), getYpos(player)}, {0, 0}, 
						{((float)randi(0,100)/100.0f-0.5f)*50.0f, ((float)randi(0,100)/100.0f-0.5f)*50.0f},
						-0.1f, -(float)randi(50, 100)/10.0f,
						1.45f, (float)randi(20,70)/100.0f, (float)randi(0,30)/100.0f,
						randi(200, 255), 0, randi(0, 50), (float)randi(20, 40)/100.0f
						);
	}
}

void renderItems(){
	for(int i = 0; i < items.len(); i++){
		gameItem* itm = items.get(i);
		if(itemIsActive(itm) && 
			itemYPos(itm) > -10 &&
			itemYPos(itm) < 50 &&
			itemXPos(itm) > -10 &&
			itemXPos(itm) < 90){
			updateGameItem(itm, time, time*speed, &items);

			float xdiff = getXpos(player) - itemXPos(itm);
			float ydiff = getYpos(player) - itemYPos(itm);
			if(xdiff > -0.3 && xdiff < 1.7 &&
				ydiff > -0.3 && ydiff < 1.7){
				collect(itm, player);
				items.rem(i); i--;
			}
			else if(itemType(itm) == GRENADE){
				render_rectangle(itemXPos(itm)-0.25, itemYPos(itm)-0.25,
								itemXPos(itm)+0.25, itemYPos(itm)+0.25, 0.3,
								255, 255, 100, 255);
			}
			else if(itemType(itm) == GRENADEPACK){
				render_rectangle(itemXPos(itm)+0.25, itemYPos(itm)+0.25,
								itemXPos(itm)+0.75, itemYPos(itm)+0.75, 0.3,
								255, 255, 100, 255);
			}
			else if(itemType(itm) == CLUSTERGRENADE ||
					itemType(itm) == CLUSTERCHILD){
				render_rectangle(itemXPos(itm)-0.25, itemYPos(itm)-0.25,
								itemXPos(itm)+0.25, itemYPos(itm)+0.25, 0.3,
								100, 255, 0, 255);
			}
			else if(itemType(itm) == CLUSTERGRENADEPACK){
				render_rectangle(itemXPos(itm)+0.25, itemYPos(itm)+0.25,
								itemXPos(itm)+0.75, itemYPos(itm)+0.75, 0.3,
								100, 255, 0, 255);
			}
			else if(itemType(itm) == MISSILE){
				render_rectangle(itemXPos(itm)-1.0, itemYPos(itm)-0.15,
								itemXPos(itm)+0.25, itemYPos(itm)+0.15, 0.3,
								255, 0, 0, 255);
				thrust({itemXPos(itm)-1.0f, itemYPos(itm)}, -0.1f, 10.0f,
						0.2f, 1.0f, 0.0f);
			}
			else if(itemType(itm) == MISSILEPACK){
				render_rectangle(itemXPos(itm)+0.25, itemYPos(itm)+0.25,
								itemXPos(itm)+0.75, itemYPos(itm)+0.75, 0.3,
								255, 0, 0, 255);
			}
			else if(itemType(itm) == STAR){
				render_rectangle(itemXPos(itm)+0.35, itemYPos(itm)+0.35,
								itemXPos(itm)+0.65, itemYPos(itm)+0.65, 0.3,
								255, 255, 255, 100);
			}
		}
		else {items.rem(i); i--;}
	}
}

void renderParticles(){
	updateParticles(time, time*speed);
	for(int i = 0; i < particles.len(); i++){
		particle* par = particles.get(i);
		if(!particleDelay(par)){
			render_rectangle(particleXPos(par)-particleR(par), particleYPos(par)-particleR(par),
							particleXPos(par)+particleR(par), particleYPos(par)+particleR(par), 
							particleZPos(par),
							particleRed(par), particleGreen(par), particleBlue(par), 
							(int)(255.0f*particleAlpha(par)));
		}
	}
}

void renderText(){
	char buffer [50];
	sprintf(buffer, "SCORE : %d LAST : %d BEST : %d", SCORE, LAST_SCORE, BEST_SCORE);
	render_string(buffer, 5, 41, 1, 0.15f, -0.15f, {255, 255, 255, 255});
	sprintf(buffer, "%d", grenadesLeft(player));
	render_string(buffer, 60, 41, 1, 0.15f, -0.15f, {255, 255, 100, 255});
	sprintf(buffer, "%d", missilesLeft(player));
	render_string(buffer, 65, 41, 1, 0.15f, -0.15f, {255, 0, 0, 255});
	sprintf(buffer, "%d", clusterGrenadesLeft(player));
	render_string(buffer, 70, 41, 1, 0.15f, -0.15f, {100, 255, 0, 255});
	render_rectangle(0.0f, 40.0f, 80.0f, 41.0f, 0.9f, 0, 0, 0, 255);

	bitmaps_render();
}

void coreUpdateAndRender(){
	stateUpdate();
	time = timer_restart(&timer);
	mapWarp += time*speed;
	window_update_view(Window, 
						40, 20, 38,
						40, 20, 0,
						0, 1, 0,
						60, 1, 300
						);/*
	window_update_view(Window, 
						0, 20, 4,
						60, 20, 0,
						0, 0, 1,
						60, 1, 300
						);*/
	window_clear(Window);

	if(getState(STATE) == STARTUP){
		delay -= time;
		if(delay <= 5.0f && delay > 2.0f){
			char buffer [50];
			sprintf(buffer, "READY IN %d", ((int)delay)-1);
			render_string(buffer, 25 - delay, 23 - delay/2, 20.0f - delay*4.0f, 0.4f, -0.4f, {255, 255, 255, 255});
			while(mapWarp >= 1)
				mapWarp -= 1;
		}
		if(delay < 2.0f && delay > 0.0f) {
			keyEvents();
			mapUpdate(); 
			Color c = color_lerp({255, 255, 255, 255}, {120, 50, 210, 100}, 1.0f - delay/2.0f);
			render_string("GO!", 30 - delay, 23 - delay/2, 20.0f - delay*4.0f, 0.6f, -0.6f, {c.r, c.g, c.b, c.a});
		}
		else {setVel(player, 5.0f, getYvel(player)); setAcc(player, 0.0f, getYacc(player)-gravityConst);}
		renderMap();
		renderPlayer();
		renderItems();
		renderParticles();
		renderText();
		if (delay < 0.0f){
			setState(STATE, GAME);
		}
	}
	else if(getState(STATE) == GAME){
		keyEvents();
		mapUpdate();
		renderMap();
		renderPlayer();
		renderItems();
		renderParticles();
		renderText();
	}
	if(is_key_pressed(Window, ESCAPE)){restart(); window_close(Window); printf("BEST SCORE : %d\n", BEST_SCORE);}
	window_update(Window);
}

#endif