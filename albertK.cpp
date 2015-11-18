//Albert Khan

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


#define PLATFORMS 200

void soundBullet(); //Jeremy

extern int endX;

extern bool EnemyTouchPlatform;



Ppmimage *Eimage = NULL;
GLuint eTexture;
GLuint ilhouetteTexture;
Ppmimage *Eimage2 = NULL;
GLuint eTexture2;


typedef struct t_bullet {
	Vec pos; //perhaps later write "center"?
	Vec vel;
	float color[3];
	struct timespec time;
	struct t_bullet *prev;
	struct t_bullet *next;
}Bullet; 
Bullet *bhead=NULL;
WalkEnemy *eWalkers = NULL;
struct timespec bulletTimer;

const double physicsRate = 1.0 / 60.0;
const double oobillion = 1.0 / 1e9;
struct timespec timeStart, timeCurrent;
struct timespec timePause;
double physicsCountdown=0.0;
double timeSpan=0.0;
//unsigned int upause=0;
double timeDiff(struct timespec *start, struct timespec *end) {
	return (double)(end->tv_sec - start->tv_sec ) +
			(double)(end->tv_nsec - start->tv_nsec) * oobillion;
}

void timeCopy(struct timespec *dest, struct timespec *source) {
	memcpy(dest, source, sizeof(struct timespec));
}

bool walkerShot(Bullet *b, WalkEnemy *w) {
	if((b->pos.y) <= (w->center.y + w->height) &&  
		(b->pos.y) > (w->center.y - w->height)&&
		(b->pos.x) < (w->center.x + w->width) &&
		(b->pos.x) > (w->center.x - w->width) )
		//c->velocity.y *= -0.25;
		//c->center.y += -3;
		//c->center.y += c->velocity.y;
		return true;
	else 
		return false;
}

void deleteEnemy(WalkEnemy *node)
{
	//remove a node from linked list
	if (node->prev == NULL) {
		if (node->next == NULL) {
			//Log("only 1 item in list.\n");
			eWalkers = NULL;
		} else {
			//Log("at beginning of list.\n");
			node->next->prev = NULL;
			eWalkers = node->next;
		}
	} else {
		if (node->next == NULL) {
			//Log("at end of list.\n");
			node->prev->next = NULL;
		} else {
			//Log("in middle of list.\n");
			node->prev->next = node->next;
			node->next->prev = node->prev;
		}
	}
	free(node);
	node = NULL;
}

bool collisionBullet(Bullet *b, Shape *s) {
	if((b->pos.y) <= (s->center.y + s->height) &&  
		(b->pos.y) > (s->center.y - s->height)&&
		(b->pos.x) < (s->center.x + s->width) &&
		(b->pos.x) > (s->center.x - s->width) )
		//c->velocity.y *= -0.25;
		//c->center.y += -3;
		//c->center.y += c->velocity.y;
		return true;
	else 
		return false;
}

unsigned char *buildAlphData(Ppmimage *img)
{
	//add 4th component to RGB stream...
	int i;
	int a,b,c;
	unsigned char *newdata, *ptr;
	unsigned char *data = (unsigned char *)img->data;
	newdata = (unsigned char *)malloc(img->width * img->height * 4);
	ptr = newdata;
	for (i=0; i<img->width * img->height * 3; i+=3) {
		a = *(data+0);
		b = *(data+1);
		c = *(data+2);
		*(ptr+0) = a;
		*(ptr+1) = b;
		*(ptr+2) = c;
		//get largest color component...
		//*(ptr+3) = (unsigned char)((
		//		(int)*(ptr+0) +
		//		(int)*(ptr+1) +
		//		(int)*(ptr+2)) / 3);
		//d = a;
		//if (b >= a && b >= c) d = b;
		//if (c >= a && c >= b) d = c;
		//*(ptr+3) = d;
		*(ptr+3) = (a|b|c);
		ptr += 4;
		data += 3;
	}
	return newdata;
}


bool walkercollision(WalkEnemy *w, Shape *s) {
	if((w->center.y - w->height) <= (s->center.y + s->height) &&  
		(w->center.y + w->height) > (s->center.y - s->height)&&
		(w->center.x - w->width) < (s->center.x + s->width) &&
		(w->center.x + w->width) > (s->center.x - s->width) )
		//c->velocity.y *= -0.25;
		//c->center.y += -3;
		//c->center.y += c->velocity.y;
		return true;
	else 
		return false;
}

void EnemyGravity(Game *game) {
	if(EnemyTouchPlatform == 0) {
	game->walkers[0].velocity.y -= 1;
	game->walkers[0].center.y = game->walkers[0].center.y + game->walkers[0].velocity.y;
	}
	if(EnemyTouchPlatform == 1)
	game->walkers[0].velocity.y = 0;
}

void enemy() {
	Eimage = ppm6GetImage("./images/enemy1.ppm");
	glGenTextures(1, &eTexture);
	glGenTextures(1, &ilhouetteTexture);

	glBindTexture(GL_TEXTURE_2D, eTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	unsigned char *ilhouetteData = buildAlphData(Eimage);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Eimage->width, Eimage->height, 0,
							GL_RGBA, GL_UNSIGNED_BYTE, ilhouetteData);
	free(ilhouetteData);
}

void enemyMaker() {
	
	//renderEnemy;
	glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.0f);
	glBindTexture(GL_TEXTURE_2D, eTexture);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(-22, -17.5);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(-22, 17.5);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(22, 17.5);
	glTexCoord2f(1.0f, 1.0f); glVertex2i(22, -17.5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_ALPHA_TEST);

	glPopMatrix();
}

void bulletRender() {
	
		Bullet *b = bhead;
		while(b) {
			//Log("draw bullet...\n");
			glColor3f(0.0, 255.0, 0.0);
			glBegin(GL_POINTS);
				glVertex2f(b->pos.x,      b->pos.y);
				glVertex2f(b->pos.x-1.0f, b->pos.y);
				glVertex2f(b->pos.x+1.0f, b->pos.y);
				glVertex2f(b->pos.x,      b->pos.y-1.0f);
				glVertex2f(b->pos.x,      b->pos.y+1.0f);
				glColor3f(0.8, 0.8, 0.8);
				glVertex2f(b->pos.x-1.0f, b->pos.y-1.0f);
				glVertex2f(b->pos.x-1.0f, b->pos.y+1.0f);
				glVertex2f(b->pos.x+1.0f, b->pos.y-1.0f);
				glVertex2f(b->pos.x+1.0f, b->pos.y+1.0f);
			glEnd();
			b = b->next;
		}
}


void bulletPress(Game *game, Character *c) {
	
	struct timespec bt;
		clock_gettime(CLOCK_REALTIME, &bt);
		double ts = timeDiff(&bulletTimer, &bt);
		if (ts > 1) {
			timeCopy(&bulletTimer, &bt);
		//shoot a bullet...
			Bullet *b = (Bullet *)malloc(sizeof(Bullet));
			timeCopy(&b->time, &bt);
			b->pos.x = c->center.x;
			b->pos.y = c->center.y;
			b->vel.x = c->velocity.x;
			b->vel.y = c->velocity.x;
		
			b->prev = NULL;
			b->next = bhead;
			if (bhead != NULL)
				bhead->prev = b;
			bhead = b;
			soundBullet(); //Jeremy
		}
}

void deleteBullet(Bullet *node)
{
	//remove a node from linked list
	if (node->prev == NULL) {
		if (node->next == NULL) {
			//Log("only 1 item in list.\n");
			bhead = NULL;
		} else {
			//Log("at beginning of list.\n");
			node->next->prev = NULL;
			bhead = node->next;
		}
	} else {
		if (node->next == NULL) {
			//Log("at end of list.\n");
			node->prev->next = NULL;
		} else {
			//Log("in middle of list.\n");
			node->prev->next = node->next;
			node->next->prev = node->prev;
		}
	}
	free(node);
	node = NULL;
}

void bulletMovement(Game *game, WalkEnemy *w) {
	struct timespec bt;
	clock_gettime(CLOCK_REALTIME, &bt);
	Bullet *b = bhead;
	while(b) {
		double ts = timeDiff(&b->time, &bt);
		if (ts > 60) {
			Bullet *saveb = b->next;
			deleteBullet(b);
			b = saveb;
			continue;
		}
		b->pos.x += b->vel.x + 5;
		b->pos.y += b->vel.y;
	//Check for collision with window edges
		if (b->pos.x >= endX) {
			deleteBullet(b);
		}

		if(game->character.center.y < 50) {
			deleteBullet(b);
		}

		for(int i = 0; i<PLATFORMS; i++) { 	
			Shape *e = &game->platform[i];
			if(collisionBullet(b,e) == true) {
				if(b->pos.x  > e->center.x - e->width) //for left side of platform
				deleteBullet(b);

			}
		}

		if(walkerShot(b,w) == true) {
			if(b->pos.x > w->center.x - w->width)
				deleteEnemy(w);
		}

		b = b->next;
	}
}


void enemy2() 
{
	Eimage2 = ppm6GetImage("./images/stationaryenemy.ppm");
	glGenTextures(1, &eTexture2);
	glGenTextures(1, &ilhouetteTexture);

	glBindTexture(GL_TEXTURE_2D, eTexture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	unsigned char *ilhouetteData = buildAlphData(Eimage2);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Eimage2->width, Eimage2->height, 0,
							GL_RGBA, GL_UNSIGNED_BYTE, ilhouetteData);
	free(ilhouetteData);
}

void enemyMaker2() 
{
	
	//renderEnemy;
	glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.0f);
	glBindTexture(GL_TEXTURE_2D, eTexture2);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(-28.5, -21);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(-28.5, 21);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(28.5, 21);
	glTexCoord2f(1.0f, 1.0f); glVertex2i(28.5, -21);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_ALPHA_TEST);

	glPopMatrix();
}
