#ifndef gameObject_h
#define gameObject_h

#include "particle.h"

#define gravityConst		-20
#define frictionConst		-50

float warp = 0.0f;

struct gameObject{
	v2 pos;
	v2 vel;
	v2 acc;
	v2 initialPos;
	bool gravityFlipped;
	bool active;
	float offset;
	int col;
	int coins;
	int grenades;
	int clustergrenades;
	int missiles;

	float starlife;
};

//================================ALLOC ACTIVE=================================//

gameObject* allocGameObject(float x, float y)
{
	gameObject* obj = (gameObject*)calloc(1, sizeof(gameObject));
	if (obj != NULL)
	{
		obj->initialPos = {x, y};
		obj->pos = obj->initialPos;
		obj->vel = {};
		obj->acc = {};
		obj->gravityFlipped = false;
		obj->active = false;
		obj->coins = 0;
		obj->grenades = 10;
		obj->clustergrenades = 0;
		obj->missiles = 0;
		obj->starlife = 10.0f;
	}
	return obj;
}

void activate(gameObject* obj){
	obj->active = true;
}

void deactivate(gameObject* obj){
	obj->active = false;
}

bool isActive(gameObject* obj){
	return obj->active;
}

void restartGameObject(gameObject* obj){
		obj->pos = obj->initialPos;
		obj->vel = {};
		obj->acc = {};
		obj->gravityFlipped = false;
		obj->coins = 0;
		obj->grenades = 10;
		obj->clustergrenades = 0;
		obj->missiles = 0;
		obj->starlife = 10.0f;
}

//================================ALLOC ACTIVE END=============================//

//================================POS VEL ACC==================================//

void setPos(gameObject* obj, float x, float y)
{
	obj->pos = {x, y};
}

float getXpos(gameObject* obj)
{
	return obj->pos.x;
}

float getYpos(gameObject* obj)
{
	return obj->pos.y;
}

void setVel(gameObject* obj, float x, float y)
{
	obj->vel = {x, y};
}

float getXvel(gameObject* obj)
{
	return obj->vel.x;
}

float getYvel(gameObject* obj)
{
	return obj->vel.y;
}

void setAcc(gameObject* obj, float x, float y)
{
	obj->acc = {x, y};
}

float getXacc(gameObject* obj)
{
	return obj->acc.x;
}

float getYacc(gameObject* obj)
{
	return obj->acc.y;
}

//================================POS VEL ACC END==============================//

//================================MOVE OPERATIONS==============================//

void move(gameObject* obj, float x, float y){
	obj->acc += {x, y};
}

void gravity(gameObject* obj){
	if (!obj->gravityFlipped)
	{
		if(!COLLISION(obj->col, Bot)){}
			//obj->acc.y += gravityConst;
	}
	else
	{
		if(!COLLISION(obj->col, Top)){}
			//obj->acc.y -= gravityConst;
	}
}

void friction(gameObject* obj){
	float friction = frictionConst;
	if(obj->starlife > 0.0f)
		friction *= 0.5f;
	if (COLLISION(obj->col, Top)||COLLISION(obj->col, Bot))
	{
		obj->acc.x += friction * obj->vel.x;
	}
	if (COLLISION(obj->col, Left)||COLLISION(obj->col, Right))
	{
		obj->acc.y += friction * obj->vel.y;
	}
}

void bounce(gameObject* obj){
	bool bounced = false;
	if(obj->starlife == 0.0f) {
		if(COLLISION(obj->col, Bot)){
			obj->vel.y = -obj->vel.y/10;
			bounced = true;
			obj->pos.y += 0.1;
		}
		else if(COLLISION(obj->col, Top)){
			obj->vel.y = -obj->vel.y/10;
			bounced = true;
			obj->pos.y -= 0.1;
		}
		if(COLLISION(obj->col, Right)){
			obj->vel.x = -obj->vel.x/10;
			bounced = true;
			obj->pos.x -= 0.1;
		}
		else if(COLLISION(obj->col, Left)){
			obj->vel.x = -obj->vel.x/10;
			bounced = true;
			obj->pos.x += 0.1;
		}
		if(bounced){
			systemGlitch();
			if(abs(obj->vel.x) > 15 &&(COLLISION(obj->col, Right)||COLLISION(obj->col, Left))||
				abs(obj->vel.y) > 15 &&(COLLISION(obj->col, Top)||COLLISION(obj->col, Bot)))
				blast3((int)obj->pos.x, (int)obj->pos.y);
			else
				blast1((int)obj->pos.x, (int)obj->pos.y);
		}
	}
}

void flipGravity(gameObject* obj){
	obj->gravityFlipped = !obj->gravityFlipped;
}

int shootGrenade(gameObject* obj){
	if(obj->grenades > 0)
		return obj->grenades--;
	return 0;
}

int grenadesLeft(gameObject* obj){
	return obj->grenades;
}

int shootClusterGrenade(gameObject* obj){
	if(obj->clustergrenades > 0)
		return obj->clustergrenades--;
	return 0;
}

int clusterGrenadesLeft(gameObject* obj){
	return obj->clustergrenades;
}

int shootMissile(gameObject* obj){
	if(obj->missiles > 0)
		return obj->missiles--;
	return 0;
}

int missilesLeft(gameObject* obj){
	return obj->missiles;
}

//================================MOVE OPERATIONS END==========================//

void updateObject(gameObject* obj, float t, float cOffset){
	obj->pos.x -= cOffset;
	obj->pos += obj->vel * t;
	obj->vel += obj->acc * t;
	obj->acc = {};

	if(obj->starlife > 0.0f){
		obj->starlife -= t;
		warp += t;
		while(warp > 0.5f){
			blast1((int)obj->pos.x, (int)obj->pos.y);
			if(obj->starlife > 2.0f)
				collectEffect(obj->pos.x+0.05f, obj->pos.y+0.05f, randi(150, 255), randi(150, 255), randi(150, 255));
			else
				collectEffect(obj->pos.x+0.05f, obj->pos.y+0.05f, 255,  0, 50);
			warp -= 0.05f;
		}
		if(obj->pos.y < 2.0f){
			obj->vel.y = 15.0f;
			obj->pos.y += 0.3;
		}
		else if(obj->pos.y > 38.0f){
			obj->vel.y = -15.0f;
			obj->pos.y -= 0.3;
		}
	}


	obj->col = tilemap_get_collision(&map, obj->pos, 0.5, cOffset);
	
	//gravity(obj);
	friction(obj);
	bounce(obj);

	if(obj->starlife < 0.0f){
		obj->starlife = 0.0f;
		warp = 0.0f;
	}
}

/*void updateShot(gameObject* obj, float t, int cOffset){
	obj->pos.x -= cOffset;
	obj->offset += (float)cOffset;
	if (obj->offset >= 50)
		obj->offset -= 50;

	obj->pos += obj->vel * t;
	obj->vel += obj->acc * t;
	obj->acc = {};
	
	if (hitUp(obj) ||
		hitDown(obj) ||
		hitRight(obj) ||
		obj->pos.x > 1000)
	{ 
		//deactivate(obj); 
	}
}*/

#endif