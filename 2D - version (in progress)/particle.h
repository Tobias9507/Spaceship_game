#ifndef particle_h
#define particle_h

//==========================PARTICLE=======================//

float NR_OFF	=	20/10;
float ACC 		=	10;

struct particle{
	v2 pos;
	v2 vel;
	v2 acc;
	float zpos;
	float zvel;
	float r;
	float life;
	float delay;
	int red;
	int green;
	int blue;
	float alpha;
};

Array<particle> particles;

particle createParticle(v2 pos, v2 vel, v2 acc, 
						float zpos, float zvel,
						float r, float life, float delay, 
						int red, int green, int blue, float alpha){
	particle par;
	par.pos = pos;
	par.vel = vel;
	par.acc = acc;
	par.zpos = zpos;
	par.zvel = zvel;
	par.r = r;
	par.life = life;
	par.delay = delay;
	par.red = red;
	par.green = green;
	par.blue = blue;
	if(alpha > 1)
		par.alpha = 1;
	else if(alpha < 0)
		par.alpha = 1;
	else
		par.alpha = alpha;	
	return par;
}

float particleXPos(particle* par){
	return par->pos.x;
}

float particleYPos(particle* par){
	return par->pos.y;
}

float particleZPos(particle* par){
	return par->zpos;
}

float particleR(particle* par){
	return par->r;
}

float particleAlpha(particle* par){
	if(par->life > 0.5)
		return par->alpha;
	return par->life*2.0f * par->alpha;
}

int particleDelay(particle* par){
	if(par->delay > 0)
		return 1;
	return 0;
}

int particleRed(particle* par){
	return par->red;
}

int particleGreen(particle* par){
	return par->green;
}

int particleBlue(particle* par){
	return par->blue;
}

void singleParticle(v2 pos, v2 vel, v2 acc,
					float zpos, float zvel,
					float r, float life, float delay,
					int red, int green, int blue, float alpha){
	particles.add(
				createParticle(
					pos, vel, acc,
					zpos, zvel,
					r, life, delay,
					red, green, blue, alpha
					));
}

void flashRainbow(int amount, float life, float intensity){
	for(int i = 0; i < amount; i++){
		particles.add(
		createParticle(
			{randf(1.0f, 79.0f), randf(1.0f, 39.0f)}, {0, 0}, {0, 0},
			11.0f, 1.0f,
			randf(1.0f, 2.0f), life/randf(1.0f, 2.0f), randf(0, life),
			randi(0, 255), randi(0, 255), randi(0, 255), intensity
			));
	}
}

void flashPurple(int amount, float life, float intensity){
	for(int i = 0; i < amount; i++){
		particles.add(
		createParticle(
			{randf(1.0f, 79.0f), randf(1.0f, 39.0f)}, {0, 0}, {0, 0},
			11.0f, 1.0f,
			randf(0.3f, 1.2f), life/randf(1.0f, 2.0f), randf(0, life),
			randi(100, 140), 50, randi(190, 240), intensity
			));
	}
}

void flashRed(int amount, float life, float intensity){
	for(int i = 0; i < amount; i++){
		particles.add(
		createParticle(
			{randf(1.0f, 79.0f), randf(1.0f, 39.0f)}, {0, 0}, {0, 0},
			11.0f, 1.0f,
			randf(0.5f, 1.0f), life/randf(1.0f, 2.0f), randf(0, life),
			randi(200, 255), 0, randi(0, 50), intensity
			));
	}
}

void systemGlitch(){
	for(int i = 0; i < 40; i++){
		particles.add(
			createParticle(
				{randf(5.0f, 75.0f), randf(5.0f, 35.0f)}, {0, 0}, {0, 0},
				10.0f, 1.0f,
				randf(0.3f, 0.7f), randf(0.1f, 0.4f), randf(0.0f, 0.2f),
				255, 255, 255, randf(0.4f, 0.6f)
				));
	}
}

void starfall(float x, float y){
	for(int i = 0; i < 5; i++){
		particles.add(
			createParticle(
				{x+0.5f, y+0.5f}, {randf(-5.0f, 40.0f), randf(-25.0f, 25.0f)}, {0, 0},
				0.3f, 0.0f,
				randf(0.05f, 0.1f), randf(0.3f, 0.6f), randf(0.0f, 0.01f),
				randi(150, 255), randi(150, 255), randi(150, 255), randf(0.4f, 0.7f)
				));
	}
}

void restartAnimation(float life, float intensity){
	if(life > 1.2f)
		life = 1.2f;
	else if(life < 0.5f)
		life = 0.5f;
	if(intensity > 1.0f)
		intensity = 1.0f;
	else if(intensity < 0.1f)
		intensity = 0.1f;

	particles.add(
		createParticle(
			{40, 20}, {0, 0}, {0, 0},
			10.0f, 1.0f,
			40.0f, life, 0.0f,
			255, 255, 255, intensity
			));
	for(int i = 0; i < 10; i++){
		particles.add(
		createParticle(
			{randf(5.0f, 75.0f), randf(5.0f, 35.0f)}, {0, 0}, {0, 0},
			10.0f, 1.0f,
			randf(5.0f, 15.0f), life/randf(1.0f, 2.0f), randf(0.0f, life),
			255, 255, 255, intensity
			));
	}
	for(int i = 0; i < 20; i++){
		particles.add(
		createParticle(
			{randf(5.0f, 75.0f), randf(5.0f, 35.0f)}, {0, 0}, {0, 0},
			10.0f, 1.0f,
			randf(2.0f, 8.0f), life/randf(1.0f, 2.0f), randf(life/2.0f, life*2),
			randi(150, 200), randi(150, 200), randi(150, 200), intensity*0.8f
			));
	}
	for(int i = 0; i < 30; i++){
		particles.add(
		createParticle(
			{randf(1.0f, 79.0f), randf(1.0f, 39.0f)}, {0, 0}, {0, 0},
			11.0f, 1.0f,
			randf(1.0f, 2.0f), life/randf(1.0f, 2.0f), randf(life/2.0f, life*2),
			randi(100, 140), 50, randi(190, 240), intensity*0.5f
			));
	}
	for(int i = 0; i < 40; i++){
		particles.add(
		createParticle(
			{randf(1.0f, 79.0f), randf(1.0f, 39.0f)}, {0, 0}, {0, 0},
			11.0f, 1.0f,
			randf(0.5f, 1.0f), life/randf(1.0f, 3.0f), randf(life*1.5f, life*3.0f),
			randi(200, 255), 0, randi(0, 50), intensity*0.5f
			));
	}
}

void thrust(v2 pos, float zpos, float zvel,
			float r, float life, float delay){
	for(int i = 0; i < 5; i++){
		particles.add(
				createParticle(
					pos, {randf(-50.0f, -15.0f), randf(-10.0f, 10.0f)}, {0, 0},
					zpos, zvel,
					r, life, delay,
					randi(200, 255), 0, randi(0, 50), 0.5f
					));
	}
}

//120, 50, 210,

//randi(150, 255), 0, randi(150, 255)

void splitBlock(int x, int y){
	particles.add(
				createParticle(
					{(float)x+0.5f, (float)y+0.5f}, {((float)randi(0,100)-50.0f)/100.0f, ((float)randi(0,100)-50)/10.0f}, {0, 0},
					0.1f, randf(5.0f, 10.0f),
					randf(0.4f, 0.5f), randf(0.4f, 1.2f), 0,
					randi(100, 140), 50, randi(190, 240), randf(0.5f, 0.7f)
					));
	for(int xp = 0; xp < NR_OFF; xp++){
		for(int yp = 0; yp < NR_OFF; yp++){
			float posx = ((float)x)+xp/NR_OFF;
			float posy = ((float)y)+yp/NR_OFF;
			float accx = ((((float)xp)-NR_OFF/2.0f)*ACC)*((float)randi(250,400))/100.0f;
			float accy = ((((float)yp)-NR_OFF/2.0f)*ACC)*((float)randi(250,400))/100.0f;

			particles.add(
				createParticle(
					{posx, posy}, {0, 0}, {accx, accy},
					-1.0f, ((float)randi(50,2000)/100.0f)-5.0f,
					randf(0.1f, 0.25f), randf(0.3f, 1.3f), 0,
					randi(80, 160), 50, randi(180, 250), randf(0.2f, 0.6f)
					));
		}
	}
}

void collectEffect(float x, float y, int red, int green, int blue){
	particles.add(
		createParticle(
			{x, y}, {0, 0}, {0, 0},
			0.5f, 2.0f,
			0.6f, 0.5f, 0.0f,
			red, green, blue, 0.5
			));
	particles.add(
		createParticle(
			{x-0.5f, y-0.5f}, {-2, -2}, {0, 0},
			0.5f, 0.0f,
			0.3f, 0.4f, 0.2f,
			red, green, blue, 0.5
			));
	particles.add(
		createParticle(
			{x-0.5f, y+0.5f}, {-2, 2}, {0, 0},
			0.5f, 0.0f,
			0.3f, 0.4f, 0.2f,
			red, green, blue, 0.5
			));
	particles.add(
		createParticle(
			{x+0.5f, y-0.5f}, {2, -2}, {0, 0},
			0.5f, 0.0f,
			0.3f, 0.4f, 0.2f,
			red, green, blue, 0.5
			));
	particles.add(
		createParticle(
			{x+0.5f, y+0.5f}, {2, 2}, {0, 0},
			0.5f, 0.0f,
			0.3f, 0.4f, 0.2f,
			red, green, blue, 0.5
			));
}

void updateParticles(float t, float cOffset){
	for(int i = 0; i < particles.len(); i++){
		particle* par = particles.get(i);
		par->pos.x -= cOffset;
		if(par->delay <= 0){
			par->life -= t;
			if(par->life > 0){
				par->pos += par->vel * t;
				par->vel += par->acc * t;
				par->acc *= 0.8f;
				par->zpos += par->zvel * t;
				par->zvel *= 0.9f;
			} else 
					particles.rem(i--);
		} else {
			par->delay -= t;
			if(par->delay < 0)
				par->life += par->delay;
		}
	}
}


//==========================PARTICLE END===================//

#endif