#ifndef _DEFS_H
#define _DEFS_H

#include <ctime>


#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600
#define PLATFORMS 200



//Structures

struct Vec {
	float x, y, z;
};

struct Shape {
	float width, height;
	float radius;
	Vec center;
};

struct Particle {
	Shape s;
	Vec velocity;
};

struct Character {
	Shape s;
	Vec velocity;
	int health;
	float width, height;
	Vec center;  //position?
	Vec pos;
	
	bool walkClk;
	bool walk;
	bool shoot;
	bool right;
	bool left;
	
	clock_t beginShoot;
	clock_t beginWalk;
    clock_t beginBlock;
    float shootTimeDif;
    float walkTimeDif;
    float blockTimeDif;
	
};

struct WalkEnemy {
	Shape s;
	Vec velocity;
	float width, height;
	Vec center;
	struct WalkEnemy *prev;
	struct WalkEnemy *next;
};

struct MPlatform {
	Shape s;
	Vec velocity;
	float width, height;
	float radius;
	Vec center;
};
	

struct miniBoss {
	Shape s;
        Vec velocity;
        float width, height;
        Vec center;
        Vec pos;
    
        struct miniBoss *prev;
        struct miniBoss *next;
        miniBoss() {
               prev = NULL;
               next = NULL;
        }
};

struct Game {
	Shape box[6];
	Shape circle;
	Character character;
	miniBoss mb;
	Shape platform[200];
	WalkEnemy walkers[10];
	MPlatform movePlatform[5];
	Shape crushJ;

	
	Particle particle[4000];
	int n;
	int nminboss;
	int lastMousex, lastMousey;
	
};





#endif
