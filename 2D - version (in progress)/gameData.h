#ifndef gameData_h
#define gameData_h

#include "particle.h"

//==========================GAME DATA=======================//

#define xtiles 		80
#define ytiles 		40
#define NO_BLOCK	0
#define BLOCK 		1
#define LAVA		2
#define ITEM		3


int counter;
int score;

Tilemap map;

void mapInit(){
	tilemap_init(&map, xtiles, ytiles);

	//CLEAR MAP
	for(int y = 1; y < ytiles-1; y++){
		for(int x = 0; x < xtiles; x++)
			if(x > 20 && randf(0.0f, 1.0f) > 0.975) {map.set(x, y, ITEM);}
			else {map.set(x, y, NO_BLOCK);}
	}

	for(int x = 0; x < xtiles; x++){
		map.set(x, 1, BLOCK);
		map.set(x, ytiles - 2, BLOCK);
		map.set(x, 0, LAVA);
		map.set(x, ytiles - 1, LAVA);
	}

	counter = randi(0, 100000);
	score = 0;
}

int tileType(int x, int y){
	return map.get(x, y);
}

void setBlock(int x, int y, int type){
	map.set(x, y, type);
}

int getScore(){
	int temp = score;
	score = 0;
	return temp;
}

void deleteBlock(int x, int y){
	if(x >= 0 && x < xtiles - 1 &&
		y >= 1 && y < ytiles - 1){
		if(map.get(x, y) == BLOCK){
			splitBlock(x, y);
			map.set(x, y, NO_BLOCK);
			score += 100;
		}
		if(randf(0.0f, 1.0f) > 0.8){
			singleParticle(
					{(float)x+0.5f, (float)y+0.5f}, {0, 0}, 
					{randf(-15.0f, 15.0f), randf(-15.0f, 15.0f)},
					-0.1f, randf(-10.0f, -5.0f),
					2.45f, randf(0.0f, 1.0f), randf(0.0f, 0.5f),
					randi(200, 255), 0, randi(0, 50), randf(0.1f, 0.3f)
					);
		}
	}
}

void blast1(int x, int y){
	deleteBlock(x-1, y-1);
	deleteBlock(x-1, y);
	deleteBlock(x-1, y+1);

	deleteBlock(x, y-1);
	deleteBlock(x, y);
	deleteBlock(x, y+1);

	deleteBlock(x+1, y-1);
	deleteBlock(x+1, y);
	deleteBlock(x+1, y+1);
}

void blast2(int x, int y){
	blast1(x, y);

	deleteBlock(x-2, y-1);
	deleteBlock(x-2, y);
	deleteBlock(x-2, y+1);

	deleteBlock(x-1, y-2);
	deleteBlock(x-1, y+2);

	deleteBlock(x, y-2);
	deleteBlock(x, y+2);

	deleteBlock(x+1, y-2);
	deleteBlock(x+1, y+2);

	deleteBlock(x+2, y-1);
	deleteBlock(x+2, y);
	deleteBlock(x+2, y+1);
}

void blast3(int x, int y){
	blast2(x, y);

	deleteBlock(x-3, y-1);
	deleteBlock(x-3, y);
	deleteBlock(x-3, y+1);

	deleteBlock(x-2, y-2);
	deleteBlock(x-2, y+2);

	deleteBlock(x-1, y-3);
	deleteBlock(x-1, y+3);

	deleteBlock(x, y-3);
	deleteBlock(x, y+3);

	deleteBlock(x+1, y-3);
	deleteBlock(x+1, y+3);

	deleteBlock(x+2, y-2);
	deleteBlock(x+2, y+2);

	deleteBlock(x+3, y-1);
	deleteBlock(x+3, y);
	deleteBlock(x+3, y+1);
}

void blast4(int x, int y){
	blast3(x, y);

	deleteBlock(x-4, y-1);
	deleteBlock(x-4, y);
	deleteBlock(x-4, y+1);

	deleteBlock(x-3, y-3);
	deleteBlock(x-3, y-2);
	deleteBlock(x-3, y+2);
	deleteBlock(x-3, y+3);

	deleteBlock(x-2, y-3);
	deleteBlock(x-2, y+3);

	deleteBlock(x-1, y-4);
	deleteBlock(x-1, y+4);

	deleteBlock(x, y-4);
	deleteBlock(x, y+4);

	deleteBlock(x+1, y-4);
	deleteBlock(x+1, y+4);

	deleteBlock(x+2, y-3);
	deleteBlock(x+2, y+3);

	deleteBlock(x+3, y-3);
	deleteBlock(x+3, y-2);
	deleteBlock(x+3, y+2);
	deleteBlock(x+3, y+3);

	deleteBlock(x+4, y-1);
	deleteBlock(x+4, y);
	deleteBlock(x+4, y+1);
}

void updateMap(){	
	counter++;
	for(int y = 0; y < ytiles; y++){
		for(int x = 0; x < xtiles - 1; x++){
			if (map.get(x+1, y) == ITEM)
				map.set(x, y, NO_BLOCK);
			else
				map.set(x, y, map.get(x+1, y));
		}
	}
	for(int y = 1; y < ytiles -1; y++){
		float r = stb_perlin_noise3((counter)*0.1, y*0.1, 0, 0, 0, 0);
		if (r <= 0 || r > 0.5){
			if (randf(0.0f, 1.0f) < 0.99)
				map.set(xtiles-1, y, NO_BLOCK);
			else{
				map.set(xtiles-1, y, ITEM);
			}
		}
		else
			map.set(xtiles-1, y, BLOCK);
	}
	map.set(xtiles-1, 0, LAVA);
	map.set(xtiles-1, ytiles-1, LAVA);
}

//==========================GAME DATA END===================//
#endif