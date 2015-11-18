//Christopher Rann
//
//My file contains control of camera, basic left/right x-axis control of character, character-to-platform collisions, and making 1st part of 
//stage
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "ppm.h"
#include "struct.h"

using namespace std;

Ppmimage *backgroundImage = NULL;
GLuint backgroundTexture;
GLuint silhouetteTexture;
int silhouette = 1;
int bg = 1;
Ppmimage *jsImage = NULL;
GLuint jsTexture;
int js = 1; 
Ppmimage *platformImage = NULL;
GLuint platformTexture;
int plat = 1;
Ppmimage *platformMoveImage = NULL;
GLuint platformMoveTexture;
Ppmimage *wallImage = NULL;
GLuint wallTexture;
Ppmimage *crushImage = NULL;
GLuint crushTexture;

extern bool touchPlatform;
extern int keys[];
extern int beginX;
extern int endX;
extern int beginY;
extern int endY;
extern unsigned char *buildAlphaData(Ppmimage *img);

bool platRight = 1;
int touchMovePlatRight = 2;
bool platUp = 0;
int touchMPUp = 2;
int onlyOneCrush = 0;
int crushExist = 0;

bool collision(Character *c, Shape *s) {
	if((c->center.y - c->height) <= (s->center.y + s->height) &&  
		(c->center.y + c->height) > (s->center.y - s->height)&&
		(c->center.x - c->width) < (s->center.x + s->width) &&
		(c->center.x + c->width) > (s->center.x - s->width) )
		//c->velocity.y *= -0.25;
		//c->center.y += -3;
		//c->center.y += c->velocity.y;
		return true;
	else 
		return false;
}

bool moveCollision(Character *c, MPlatform *s) {
	if((c->center.y - c->height) <= (s->center.y + s->height) &&  
		(c->center.y + c->height) > (s->center.y - s->height)&&
		(c->center.x - c->width) < (s->center.x + s->width) &&
		(c->center.x + c->width) > (s->center.x - s->width) )
		//c->velocity.y *= -0.25;
		//c->center.y += -3;
		//c->center.y += c->velocity.y;
		return true;
	else 
		return false;
}

void cameraControl() {
	glOrtho(beginX, endX, beginY, endY, -1, 1); 
}

void characterMaker(Game *game) {
game->character.width = 22;
	game->character.height = 15; //50, 25
	game->character.center.x = 400;
	game->character.center.y = 200;
	game->character.health = 100;
}

void platformMaker(Game *game) { 
	game->platform[0].width = 252; 
	game->platform[0].height = 24; 
	game->platform[0].center.x = 350; 
	game->platform[0].center.y = 150;
	game->platform[1].width = 252; 
	game->platform[1].height = 24; 
	game->platform[1].center.x = 890;
	game->platform[1].center.y = 230;
	game->platform[2].width = 252; 
	game->platform[2].height = 24; 
	game->platform[2].center.x = 1900;
	game->platform[2].center.y = 230;
	game->platform[3].width = 252; 
	game->platform[3].height = 24; 
	game->platform[3].center.x = 2650; 
	game->platform[3].center.y = 250;
}

void left(Game *game) {
	game->character.center.x -= 3;
	if(game->character.center.x <= beginX)
		game->character.center.x = 0;
	if(beginX >= 0) {			
		beginX -= 3;
		endX -= 3; 
	}
}

void right(Game *game) {
	game->character.center.x += 3;
 	if(game->character.center.x >= (WINDOW_WIDTH/2)) {
 		beginX += 3;
		endX += 3;
	}
}


void jsSprite(void) {
	//glClearColor(1.0,1.0,1.0,1.0);

	jsImage = ppm6GetImage("./images/mmstand.ppm");
	
	glGenTextures(1, &jsTexture);
	glGenTextures(1, &silhouetteTexture);

	

	glBindTexture(GL_TEXTURE_2D, jsTexture);
	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	/*glTexImage2D(GL_TEXTURE_2D, 0, 3,
							jsImage->width, jsImage->height,
							0, GL_RGB, GL_UNSIGNED_BYTE, jsImage->data); */

	/*glBindTexture(GL_TEXTURE_2D, silhouetteTexture);
	//
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	//
	//must build a new set of data... */
	unsigned char *silhouetteData = buildAlphaData(jsImage);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, jsImage->width, jsImage->height, 0,
							GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);
	free(silhouetteData); 

	//glEnable(GL_TEXTURE_2D);
}

void renderJS(void) {

   // if (js) {
	glColor3f(1.0, 1.0, 1.0);
	
	glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.0f);	

        glBindTexture(GL_TEXTURE_2D, jsTexture);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2i(-22, -25); //0,0
        glTexCoord2f(0.0f, 0.0f); glVertex2i(-22, 25); //0,50
        glTexCoord2f(1.0f, 0.0f); glVertex2i(22, 25); //44,50
        glTexCoord2f(1.0f, 1.0f); glVertex2i(22, -25); //44,0
        glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_ALPHA_TEST);
    //}


}

void background(void) {
	glClearColor(1.0,1.0,1.0,1.0);

	backgroundImage = ppm6GetImage("./images/background1.ppm");
	
	glGenTextures(1, &backgroundTexture);

	
	glBindTexture(GL_TEXTURE_2D, backgroundTexture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	
	
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
							backgroundImage->width, backgroundImage->height,
							0, GL_RGB, GL_UNSIGNED_BYTE, backgroundImage->data);
	
	//glEnable(GL_TEXTURE_2D);
    	//initialize_fonts();
}

void renderBG(void) {
	
	   glColor3f(1.0, 1.0, 1.0);
   // if (bg) {
	
	
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);

	
	//Multiple images placed from 0 to 5 times the original location for more map area
        glBegin(GL_QUADS);
	for(int i = 0; i < 7; i++) {
        //glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
        //glTexCoord2f(0.0f, 0.0f); glVertex2i(0, WINDOW_HEIGHT);
        //glTexCoord2f(1.0f, 0.0f); glVertex2i(WINDOW_WIDTH, WINDOW_HEIGHT);
        //glTexCoord2f(1.0f, 1.0f); glVertex2i(WINDOW_WIDTH, 0);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(WINDOW_WIDTH * i, 0);
        glTexCoord2f(0.0f, 0.0f); glVertex2i(WINDOW_WIDTH * i, WINDOW_HEIGHT);
        glTexCoord2f(1.0f, 0.0f); glVertex2i(WINDOW_WIDTH + (WINDOW_WIDTH*i), WINDOW_HEIGHT);
        glTexCoord2f(1.0f, 1.0f); glVertex2i(WINDOW_WIDTH + (WINDOW_WIDTH*i), 0);
	}
	
        glEnd();
	glDisable(GL_BLEND);

	
    //}


}


void platform(void) {
	glClearColor(1.0,1.0,1.0,1.0);

	platformImage = ppm6GetImage("./images/platform1.ppm");
	
	
	glGenTextures(1, &platformTexture);
	glGenTextures(1, &silhouetteTexture);

	glBindTexture(GL_TEXTURE_2D, platformTexture);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	
	unsigned char *silhouetteData = buildAlphaData(platformImage);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, platformImage->width, platformImage->height, 0,
							GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);
	free(silhouetteData); 
	
}

void movePlatform(void) {
	glClearColor(1.0,1.0,1.0,1.0);

	platformMoveImage = ppm6GetImage("./images/movingPlat1.ppm");
	
	
	glGenTextures(1, &platformMoveTexture);
	glGenTextures(1, &silhouetteTexture);

	glBindTexture(GL_TEXTURE_2D, platformMoveTexture);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	
	unsigned char *silhouetteData = buildAlphaData(platformMoveImage);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, platformMoveImage->width, platformMoveImage->height, 0,
							GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);
	free(silhouetteData); 
	
}



void renderPlat(void) {

	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.0f);
        glBindTexture(GL_TEXTURE_2D, platformTexture);	
	
        glBegin(GL_QUADS);
	for(int i = 0; i < 2; i++) {
	 glTexCoord2f(0.0f, 1.0f); glVertex2i((-252)+(252*i), -24.5); //(-252) + 252*i,0
        glTexCoord2f(0.0f, 0.0f); glVertex2i((-252)+(252*i), 24.5); //(-252) + 252*i,49
        glTexCoord2f(1.0f, 0.0f); glVertex2i(/*252+*/252*i, 24.5); //252*i, 49
        glTexCoord2f(1.0f, 1.0f); glVertex2i(/*252+*/252*i, -24.5); //252*i, 0
	}
	
        glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	//glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);

}

void characterPlatTopCollision(Game *game, Character *c) {
	for(int i = 0; i<PLATFORMS; i++) { 	
		Shape *e = &game->platform[i];
		if(collision(c,e) == true) { 
			touchPlatform = 1;
			game->character.center.y = e->center.y + e->height + game->character.height; //Added this to help from plat to plat
		}
	}
}

void characterPlatLRBCollision(Game *game, Character *c) {
	for(int i = 0; i<PLATFORMS; i++) { 	
		c = &game->character;
		Shape *e = &game->platform[i];
		if(collision(c,e) == true) {
			if(c->center.x  < e->center.x - e->width) //for left side of platform
			if(c->center.x + c->width > e->center.x - e->width &&
			c->center.y - c->height <= e->center.y + e->height-5) {
				c->center.x = e->center.x - e->width - c->width - 1;
				if(game->character.center.x >= (WINDOW_WIDTH/2)) { //stop camera movement if left collision or near beginning
					beginX -= 3;
					endX -= 3;
				}
			touchPlatform = 0; 
			}
			if(c->center.x  > e->center.x + e->width)
				if(c->center.x - c->width < e->center.x + e->width &&
				c->center.y - c->height <= e->center.y + e->height-5) {
					c->center.x = e->center.x + e->width + c->width ;
					if(game->character.center.x >= 0) {
						beginX += 3;
						endX += 3;
					}
				touchPlatform = 0;
				}
			if(c->center.y < e->center.y)
				if(c->center.y + c->height >= e->center.y - e->height &&
				c->center.x - c->width <= e->center.x + e->width &&
				c->center.x + c->width >= e->center.x - e->width) {
						c->center.y = e->center.y - e->height - c->height;
						touchPlatform = 0;
				}
		}
	}
}

void createMovePlat(Game *game) {
	game->movePlatform[0].width = 45; 
	game->movePlatform[0].height = 25; 
	game->movePlatform[0].center.x = 1350; 
	game->movePlatform[0].center.y = 180;

	game->movePlatform[1].width = 45; 
	game->movePlatform[1].height = 25; 
	game->movePlatform[1].center.x = 3800; 
	game->movePlatform[1].center.y = 140;	
	game->movePlatform[2].width = 45; 
	game->movePlatform[2].height = 25; 
	game->movePlatform[2].center.x = 2300; 
	game->movePlatform[2].center.y = 130;
}


void movingPlatform(Game *game) {
	
	if(platRight == 0) {
		game->movePlatform[0].velocity.x = -1;
		game->movePlatform[0].center.x = game->movePlatform[0].center.x + game->movePlatform[0].velocity.x;
		if(touchMovePlatRight == 0) {
			game->character.center.x = game->character.center.x - 1;
			beginX -= 1;
			endX -= 1;
		}
	}
	if(platRight == 1) {
		game->movePlatform[0].velocity.x = 1;
		game->movePlatform[0].center.x = game->movePlatform[0].center.x + game->movePlatform[0].velocity.x;
		if(touchMovePlatRight == 1) {
			game->character.center.x = game->character.center.x + 1;
			beginX += 1;
			endX += 1;
		}
	}

	
	
	if(platUp == 1) {
		game->movePlatform[2].velocity.y = 1;
		game->movePlatform[2].center.y = game->movePlatform[2].center.y + game->movePlatform[2].velocity.y;
		if(touchMPUp == 1) {
			game->character.center.y = game->character.center.y + 1;
		}
	}
	if(platUp == 0) {
		game->movePlatform[2].velocity.y = -1;
		game->movePlatform[2].center.y = game->movePlatform[2].center.y + game->movePlatform[2].velocity.y;
		if(touchMPUp == 0) {
			game->character.center.y = game->character.center.y - 1;
		}
	}
}

void MPStop(Game *game) {
	
	if(game->movePlatform[0].center.x < 1250)
	platRight = 1;
	if(game->movePlatform[0].center.x > 1550)
	platRight = 0;
	
	if(game->movePlatform[2].center.y < 120)
	platUp = 1;
	if(game->movePlatform[2].center.y > 200)
	platUp = 0;

}

void movingPlatTopCollision(Game *game, Character *c) {
	for(int i = 0; i<1; i++) { 	
		MPlatform *e = &game->movePlatform[i];
		if(moveCollision(c,e) == true) { 
			touchPlatform = 1;
			if(platRight == 0)
				touchMovePlatRight = 0;
			if(platRight == 1)
				touchMovePlatRight = 1;
			
			game->character.center.y = e->center.y + e->height + game->character.height; //Added this to help from plat to plat
		}
		else
			touchMovePlatRight = 2;
			
	}
	MPlatform *z = &game->movePlatform[2];
	if(moveCollision(c, z) == true ) {
		touchPlatform = 1;
		if(platUp == 1)
			touchMPUp = 1;
	}
	else
		touchMPUp = 2;
			
}

void movingPlatformCollision(Game *game, Character *c) {
	for(int i = 0; i<3; i++) { 	
		c = &game->character;
		MPlatform *e = &game->movePlatform[i];
		if(moveCollision(c,e) == true) {
			if(c->center.x  < e->center.x - e->width) //for left side of platform
			if(c->center.x + c->width > e->center.x - e->width &&
			c->center.y - c->height <= e->center.y + e->height-5) {
				c->center.x = e->center.x - e->width - c->width - 1;
				if(game->character.center.x >= (WINDOW_WIDTH/2)) { //stop camera movement if left collision or near beginning
					beginX -= 3;
					endX -= 3;
				}
			touchPlatform = 0; 
			}
			if(c->center.x  > e->center.x + e->width)
				if(c->center.x - c->width < e->center.x + e->width &&
				c->center.y - c->height <= e->center.y + e->height-5) {
					c->center.x = e->center.x + e->width + c->width ;
					if(game->character.center.x >= 0) {
						beginX += 3;
						endX += 3;
					}
				touchPlatform = 0;
				}
			if(c->center.y < e->center.y)
				if(c->center.y + c->height >= e->center.y - e->height &&
				c->center.x - c->width <= e->center.x + e->width &&
				c->center.x + c->width >= e->center.x - e->width) {
						c->center.y = e->center.y - e->height - c->height;
						touchPlatform = 0;
				}
		}
	}
}

void renderMovePlat(void) {

	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.0f);
        glBindTexture(GL_TEXTURE_2D, platformMoveTexture);	
	
        glBegin(GL_QUADS);
	for(int i = 0; i < 1; i++) {
	glTexCoord2f(0.0f, 1.0f); glVertex2i(-45, -24);
        glTexCoord2f(0.0f, 0.0f); glVertex2i(-45, 24); 
        glTexCoord2f(1.0f, 0.0f); glVertex2i(45, 24); 
        glTexCoord2f(1.0f, 1.0f); glVertex2i(45, -24); 
	}
	
        glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	//glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

}

void WallSprite(void) {

	wallImage = ppm6GetImage("./images/wall.ppm");
	
	glGenTextures(1, &wallTexture);
	glGenTextures(1, &silhouetteTexture);

	glBindTexture(GL_TEXTURE_2D, wallTexture);
	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	unsigned char *silhouetteData = buildAlphaData(wallImage);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wallImage->width, wallImage->height, 0,
							GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);
	free(silhouetteData); 
}

void renderWall(void) {

	//glColor3f(1.0, 1.0, 1.0);
	
	glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.0f);	

        glBindTexture(GL_TEXTURE_2D, wallTexture);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2i(-3, -175);
        glTexCoord2f(0.0f, 0.0f); glVertex2i(-3, 175);
        glTexCoord2f(1.0f, 0.0f); glVertex2i(3, 175); 
        glTexCoord2f(1.0f, 1.0f); glVertex2i(3, -175); 
        glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_ALPHA_TEST);

}

void crush(Game *game) {
	crushExist = 1;
	if(onlyOneCrush == 0) {
		if(crushExist == 1) {
			game->crushJ.center.x = game->character.center.x;
			game->crushJ.center.y = game->character.center.y + 1000;
			cout << "Giant Rock appear!!!\n";
		}	
	}

	crushExist =2;
	
	onlyOneCrush = 1;
}

void crushGravity(Game *game) {
	if(crushExist == 2) {
		game->crushJ.radius -= 0.5;
		game->crushJ.center.y = game->crushJ.center.y + game->crushJ.radius;
	}
}

void crushCollision(Game *game) {
	if(collision(&game->character, &game->crushJ) == true) {
		game->character.health = -100;
		crushExist = 3;
	}
	if(crushExist == 3) {
			game->crushJ.center.x = - 1000;
			game->crushJ.center.y = - 1000;
		}
}

void crushSprite(void) {

	crushImage = ppm6GetImage("./images/newWall.ppm");
	
	glGenTextures(1, &crushTexture);
	glGenTextures(1, &silhouetteTexture);

	glBindTexture(GL_TEXTURE_2D, crushTexture);
	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	unsigned char *silhouetteData = buildAlphaData(crushImage);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, crushImage->width, crushImage->height, 0,
							GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);
	free(silhouetteData); 
}

void renderCrush(void) {

	glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.0f);	

        glBindTexture(GL_TEXTURE_2D, crushTexture);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2i(-80, -100);
        glTexCoord2f(0.0f, 0.0f); glVertex2i(-80, 100);
        glTexCoord2f(1.0f, 0.0f); glVertex2i(80, 100); 
        glTexCoord2f(1.0f, 1.0f); glVertex2i(80, -100); 
        glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_ALPHA_TEST);

}
