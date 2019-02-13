#ifndef gameItem_h
#define gameItem_h

#include "particle.h"

//==========================GAME ITEM=======================//

bool itemGravityFlipped = false;

enum itemType{
	STAR,
	GRENADE,
	GRENADEPACK,
	CLUSTERGRENADE,
	CLUSTERCHILD,
	CLUSTERGRENADEPACK,
	MISSILE,
	MISSILEPACK
};

struct gameItem{
	v2 pos;
	v2 vel;
	v2 acc;
	float initVel;
	float r;
	itemType type;
	bool active;
};

gameItem createGameItem(float x, float y, float initVel, float r, itemType type){
	gameItem itm;
	itm.pos = {x, y};
	itm.vel = {0, 0};
	itm.acc = {0, 0};
	itm.initVel = initVel;
	itm.r = r;
	itm.type = type;
	itm.active = true;
	return itm;
}

void restartItems(){
	itemGravityFlipped = false;
}

gameItem randomCollectable(float x, float y){
	gameItem itm;
	int chance = randi(0,100);
	if(chance < 70){
		itm = createGameItem(x, y, 0, 0.25, GRENADEPACK);
	}
	else if(chance < 90){
		itm = createGameItem(x, y, 0, 0.25, MISSILEPACK);
	}
	else {
		itm = createGameItem(x, y, 0, 0.25, CLUSTERGRENADEPACK);
	}
	return itm;
}

gameItem randomPowerUp(float x, float y){
	return createGameItem(x, y, 1, 0.25, STAR);
}

int itemType(gameItem* itm){
	return itm->type;
}

float itemXPos(gameItem* itm){
	return itm->pos.x;
}

float itemYPos(gameItem* itm){
	return itm->pos.y;
}

bool itemIsActive(gameItem* itm){
	return itm->active;
}

void itemFlipGravity(){
	itemGravityFlipped = !itemGravityFlipped;
}

//==========================UPDATE==========================//

void itemGravity(gameItem* itm, float t){
	if (itemGravityFlipped){itm->acc.y += 40;}
	else{itm->acc.y -= 40;}
	itm->vel.y = itm->acc.y * t;
	itm->pos.y += itm->vel.y * t;
}

void collect(gameItem* itm, gameObject* obj){
	if(itm->type == GRENADEPACK){
		obj->grenades += 5;
		collectEffect(itm->pos.x, itm->pos.y, 255, 255, 100);
	}
	if(itm->type == MISSILEPACK){
		obj->missiles += 5;
		collectEffect(itm->pos.x, itm->pos.y, 255, 0, 0);
	}
	if(itm->type == CLUSTERGRENADEPACK){
		obj->clustergrenades += 5;
		collectEffect(itm->pos.x, itm->pos.y, 0, 255, 100);
	}
	if(itm->type == STAR){
		obj->starlife = 10.0f;
		collectEffect(itm->pos.x, itm->pos.y, 255, 255, 255);
	}
}

void updateGrenade(gameItem* itm, float t, int col){
	//itemGravity(itm, t);
	if(!(COLLISION(col, Right) || 
		COLLISION(col, Top) || 
		COLLISION(col, Bot)) && 
		itm->pos.x < 210){
		if(itm->initVel < 0)
			itm->initVel = 0;
		itm->pos.x += (itm->initVel + 50) * t;
	} else {
		blast3((int)itm->pos.x, (int)itm->pos.y);
		itm->active = false;
	}
}

void updateClusterGrenade(gameItem* itm, float t, int col, Array<gameItem>* items){
	//itemGravity(itm, t);
	if(!(COLLISION(col, Right) || 
		COLLISION(col, Top) || 
		COLLISION(col, Bot)) && 
		itm->pos.x < 210){
		if(itm->initVel < 0)
			itm->initVel = 0;
		itm->pos.x += (itm->initVel + 50) * t;
	} else {
		blast4((int)itm->pos.x, (int)itm->pos.y);
		int m = 0;
		int n = 1;
		gameItem c1 = createGameItem(itm->pos.x, itm->pos.y, 0, 0.25f, CLUSTERCHILD);
			c1.vel = {15.0f, 15.0f};
		gameItem c2 = createGameItem(itm->pos.x, itm->pos.y, 0, 0.25f, CLUSTERCHILD);
			c2.vel = {25.0f, 15.0f};
		gameItem c3 = createGameItem(itm->pos.x, itm->pos.y, 0, 0.25f, CLUSTERCHILD);
			c3.vel = {40.0f, 0.0f};
		gameItem c4 = createGameItem(itm->pos.x, itm->pos.y, 0, 0.25f, CLUSTERCHILD);
			c4.vel = {25.0f, -15.0f};
		gameItem c5 = createGameItem(itm->pos.x, itm->pos.y, 0, 0.25f, CLUSTERCHILD);
			c5.vel = {15.0f, -15.0f};
		items->add(c1);
		items->add(c2);
		items->add(c3);
		items->add(c4);
		items->add(c5);
		itm->active = false;
	}
}

void updateClusterChild(gameItem* itm, float t, int col){
	if(!((COLLISION(col, Right) || 
		COLLISION(col, Top) || 
		COLLISION(col, Bot)))){
		itm->pos += itm->vel * t;
	} else {
		blast4((int)itm->pos.x, (int)itm->pos.y);
		itm->active = false;
	}
}

void updateMissile(gameItem* itm, float t, int col){
	itm->acc.x += 20.0f;
	itm->vel.x += itm->acc.x * t;
	if(!(COLLISION(col, Right) || 
		COLLISION(col, Top) || 
		COLLISION(col, Bot)) && 
		itm->pos.x < 210){
		if(itm->initVel < 0)
			itm->initVel = 0;
		itm->pos.x += (itm->initVel + 20 + itm->vel.x) * t;
	} else {
		blast4((int)itm->pos.x, (int)itm->pos.y);
		itm->active = false;
	}
}

void updateGameItem(gameItem* itm, float t, float cOffset, Array<gameItem>* items){
	itm->pos.x -= cOffset;
	int col = tilemap_get_collision(&map, itm->pos, itm->r*2, cOffset);
	if(itm->type == GRENADE){updateGrenade(itm, t, col);}
	else if(itm->type == CLUSTERGRENADE){updateClusterGrenade(itm, t, col, items);}
	else if(itm->type == CLUSTERCHILD){updateClusterChild(itm, t, col);}
	else if(itm->type == MISSILE){updateMissile(itm, t, col);}
	else if(itm->type == STAR){itm->pos.x -= 10*t; starfall(itm->pos.x, itm->pos.y);}	
}

//==========================UPDATE END======================//

//==========================GAME ITEM END===================//

#endif