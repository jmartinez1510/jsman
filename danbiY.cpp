/*
	Danbi Youn
	
	this file is for make start menu which include buttons for start game and exit.
	it also initialize the character image files and do the character animation when the buttons hit.

*/
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "ppm.h"
#include "log.h"
#include "danbiY.h"
#include "chrisR.h"
#include "struct.h"

extern "C" {
#include "fonts.h"
}


Vec bPos;
int bDown = 0;
extern int endX, endY;
extern int title, done;
extern bool play_game, exit_game;
extern unsigned char *buildAlphaData(Ppmimage *img);
static GLuint silhouetteTexture;
extern struct timespec timeStart, timeCurrent, timeCharacter;
extern Game game;

extern double timeDiff(struct timespec *, struct timespec *);
extern void timeCopy(struct timespec *, struct timespec *);


typedef struct t_button {
	Rect r;
	char text[32];
	int over;
	int down;
	int click;
	float color[3];
	float dcolor[3];
	Vec center;
	unsigned int text_color;
} Button;

typedef struct t_box{
	float width, height;
	Vec pos;
} box;

#define MAXBUTTONS 4

struct Global{
	Button button[MAXBUTTONS];
	int nbuttons;
	int gridDim;
	int boardDim;
} g;
box titleBox;
box playBox;
box exitBox;

Ppmimage *titleImage = NULL;
//int titlePrompt = 1;
GLuint titleTexture;

Ppmimage *leftImage[2];
GLuint leftTexture[2];

Ppmimage *leftWalkImage[3];
GLuint leftWalkTexture[3];

Ppmimage *leftJumpImage[2];
GLuint leftJumpTexture[2];

Ppmimage *leftShootImage[5];
GLuint leftShootTexture[5];

Ppmimage *rightImage[2];
GLuint rightTexture[2];

Ppmimage *rightWalkImage[3];
GLuint rightWalkTexture[3];

Ppmimage *rightJumpImage[2];
GLuint rightJumpTexture[2];

Ppmimage *rightShootImage[5];
GLuint rightShootTexture[5];



void menu(void){
		
	glClearColor(1.0,1.0,1.0,1.0);
	
	/*Initialize title screen image*/
    titleImage = ppm6GetImage("./images/title_background1.ppm");
    glGenTextures(1, &titleTexture);
    glBindTexture(GL_TEXTURE_2D, titleTexture);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, titleImage->width, titleImage->height,
			0, GL_RGB, GL_UNSIGNED_BYTE, titleImage->data);
}


void menuRender(void){
	
	Rect r;
	//glClearColor(1.0,1.0,1.0,1.0);
	//glClear(GL_COLOR_BUFFER_BIT);
	//glEnable(GL_TEXTURE_2D);
	glColor3f(1.0, 1.0, 1.0);
	
	glBindTexture(GL_TEXTURE_2D, titleTexture);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(0, endY);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(endX, endY);
	glTexCoord2f(1.0f, 1.0f); glVertex2i(endX, 0);
	glEnd();
	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
	
	//draw all buttons
	for (int i=0; i<g.nbuttons; i++) {
		if (g.button[i].over) {
			int w=2;
			glColor3f(1.0f, 1.0f, 0.0f);
			//draw a highlight around button
			glLineWidth(3);
			glBegin(GL_LINE_LOOP);
				glVertex2i(g.button[i].r.left-w,  g.button[i].r.bot-w);
				glVertex2i(g.button[i].r.left-w,  g.button[i].r.top+w);
				glVertex2i(g.button[i].r.right+w, g.button[i].r.top+w);
				glVertex2i(g.button[i].r.right+w, g.button[i].r.bot-w);
				glVertex2i(g.button[i].r.left-w,  g.button[i].r.bot-w);
			glEnd();
			glLineWidth(1);
		}
		if (g.button[i].down) {
			glColor3fv(g.button[i].dcolor);
		} else {
			glColor3fv(g.button[i].color);
		}
		glBegin(GL_QUADS);
			glVertex2i(g.button[i].r.left,  g.button[i].r.bot);
			glVertex2i(g.button[i].r.left,  g.button[i].r.top);
			glVertex2i(g.button[i].r.right, g.button[i].r.top);
			glVertex2i(g.button[i].r.right, g.button[i].r.bot);
		glEnd();

		r.left = g.button[i].r.centerx;
		r.bot  = g.button[i].r.centery-8;
		r.center = 1;
		if (g.button[i].down) {
			ggprint16(&r, 0, g.button[i].text_color, "Pressed!");
		} else {
			ggprint16(&r, 0, g.button[i].text_color, g.button[i].text);
		}

	}
}

void initButton(){
	g.boardDim = g.gridDim * 10;
	
	g.nbuttons=0;
	//play button
	g.button[g.nbuttons].r.width = 240;
	g.button[g.nbuttons].r.height = 45;
	g.button[g.nbuttons].r.left = 270;
	g.button[g.nbuttons].r.bot = 290;
	g.button[g.nbuttons].r.right =
	   g.button[g.nbuttons].r.left + g.button[g.nbuttons].r.width;
	g.button[g.nbuttons].r.top =
	   g.button[g.nbuttons].r.bot + g.button[g.nbuttons].r.height;
	g.button[g.nbuttons].r.centerx =
	   (g.button[g.nbuttons].r.left + g.button[g.nbuttons].r.right) / 2;
	g.button[g.nbuttons].r.centery =
	   (g.button[g.nbuttons].r.bot + g.button[g.nbuttons].r.top) / 2;
	strcpy(g.button[g.nbuttons].text, "Play now");
	g.button[g.nbuttons].down = 0;
	g.button[g.nbuttons].click = 0;
	g.button[g.nbuttons].color[0] = 0.4f;
	g.button[g.nbuttons].color[1] = 0.4f;
	g.button[g.nbuttons].color[2] = 0.7f;
	g.button[g.nbuttons].dcolor[0] = g.button[g.nbuttons].color[0] * 0.5f;
	g.button[g.nbuttons].dcolor[1] = g.button[g.nbuttons].color[1] * 0.5f;
	g.button[g.nbuttons].dcolor[2] = g.button[g.nbuttons].color[2] * 0.5f;
	g.button[g.nbuttons].text_color = 0x00ffffff;
	g.nbuttons++;
	
	//exit button
	g.button[g.nbuttons].r.width = 240;
	g.button[g.nbuttons].r.height = 45;
	g.button[g.nbuttons].r.left = 270;
	g.button[g.nbuttons].r.bot = 180;
	g.button[g.nbuttons].r.right =
	   g.button[g.nbuttons].r.left + g.button[g.nbuttons].r.width;
	g.button[g.nbuttons].r.top = g.button[g.nbuttons].r.bot +
	   g.button[g.nbuttons].r.height;
	g.button[g.nbuttons].r.centerx = (g.button[g.nbuttons].r.left +
	   g.button[g.nbuttons].r.right) / 2;
	g.button[g.nbuttons].r.centery = (g.button[g.nbuttons].r.bot +
	   g.button[g.nbuttons].r.top) / 2;
	strcpy(g.button[g.nbuttons].text, "Quit");
	g.button[g.nbuttons].down = 0;
	g.button[g.nbuttons].click = 0;
	g.button[g.nbuttons].color[0] = 0.4f;
	g.button[g.nbuttons].color[1] = 0.4f;
	g.button[g.nbuttons].color[2] = 0.7f;
	g.button[g.nbuttons].dcolor[0] = g.button[g.nbuttons].color[0] * 0.5f;
	g.button[g.nbuttons].dcolor[1] = g.button[g.nbuttons].color[1] * 0.5f;
	g.button[g.nbuttons].dcolor[2] = g.button[g.nbuttons].color[2] * 0.5f;
	g.button[g.nbuttons].text_color = 0x00ffffff;
	g.nbuttons++;
}

void checkMouse(XEvent *e){
	
	static int savex = 0;
	static int savey = 0;
	int i,x,y;
	int lbutton=0;
	int rbutton=0;
	//
	if (e->type == ButtonRelease)
		return;
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button is down
			lbutton=1;
		}
		if (e->xbutton.button==3) {
			//Right button is down
			rbutton=1;
			if (rbutton){}
		}
	}
	x = e->xbutton.x;
	y = e->xbutton.y;
	y = endY - y;
	if (savex != e->xbutton.x || savey != e->xbutton.y) {
		//Mouse moved
		savex = e->xbutton.x;
		savey = e->xbutton.y;
	}
	for (i=0; i<g.nbuttons; i++) {
		g.button[i].over=0;
		if (x >= g.button[i].r.left &&
			x <= g.button[i].r.right &&
			y >= g.button[i].r.bot &&
			y <= g.button[i].r.top) {
			g.button[i].over=1;
			if (g.button[i].over) {
				if (lbutton) {
					switch(i) {
						case 0:
							title=0;
							break;
						case 1:
							done=true;
							break;
					}
				}
			}
		}
	}
	return;
}



void initImageToLeft(){
	
	leftImage[0] = ppm6GetImage("./images/stayL1.ppm");
	leftImage[1] = ppm6GetImage("./images/stayL2.ppm");
	leftWalkImage[0] = ppm6GetImage("./images/walkL1.ppm");
	leftWalkImage[1] = ppm6GetImage("./images/walkL2.ppm");
	leftWalkImage[2] = ppm6GetImage("./images/walkL3.ppm");
	leftJumpImage[0] = ppm6GetImage("./images/jumpL1.ppm");
	leftJumpImage[1] = ppm6GetImage("./images/jumpL2.ppm");
	leftShootImage[0] = ppm6GetImage("./images/shootL1.ppm");
	leftShootImage[1] = ppm6GetImage("./images/shootL2.ppm");
	leftShootImage[2] = ppm6GetImage("./images/shootL3.ppm");
	leftShootImage[3] = ppm6GetImage("./images/shootL4.ppm");
	leftShootImage[4] = ppm6GetImage("./images/shootL5.ppm");
	
	//init stay Image
	for(int i=0; i<2; i++){
		
		glGenTextures(1, &leftTexture[i]);
		glGenTextures(1, &silhouetteTexture);
		
		int w = leftImage[i]->width;
        int h = leftImage[i]->height;
        
		glBindTexture(GL_TEXTURE_2D, leftTexture[i]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 3 ,w ,h , 0, 
                GL_RGB, GL_UNSIGNED_BYTE, leftImage[i]->data);
		
		unsigned char *silhouetteData = buildAlphaData(leftImage[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h,
					0, GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);
		free(silhouetteData);
	}
	
	//init walk Image
	for(int i=0; i<3; i++){
		
		glGenTextures(1, &leftWalkTexture[i]);
		glGenTextures(1, &silhouetteTexture);
		
		int w = leftWalkImage[i]->width;
        int h = leftWalkImage[i]->height;
        
		glBindTexture(GL_TEXTURE_2D, leftWalkTexture[i]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 3 ,w ,h , 0, 
                GL_RGB, GL_UNSIGNED_BYTE, leftWalkImage[i]->data);
		
		unsigned char *silhouetteData = buildAlphaData(leftWalkImage[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h,
					0, GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);
		free(silhouetteData);
	}
	
	//init jump Image
	for(int i=0; i<2; i++){
		
		glGenTextures(1, &leftJumpTexture[i]);
		glGenTextures(1, &silhouetteTexture);
		
		int w = leftJumpImage[i]->width;
        int h = leftJumpImage[i]->height;
        
		glBindTexture(GL_TEXTURE_2D, leftJumpTexture[i]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 3 ,w ,h , 0, 
                GL_RGB, GL_UNSIGNED_BYTE, leftJumpImage[i]->data);
		
		unsigned char *silhouetteData = buildAlphaData(leftJumpImage[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h,
					0, GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);
		free(silhouetteData);
	}
	
	//init shoot Image
	for(int i=0; i<5; i++){
		
		glGenTextures(1, &leftShootTexture[i]);
		glGenTextures(1, &silhouetteTexture);
		
		int w = leftShootImage[i]->width;
        int h = leftShootImage[i]->height;
        
		glBindTexture(GL_TEXTURE_2D, leftShootTexture[i]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 3 ,w ,h , 0, 
                GL_RGB, GL_UNSIGNED_BYTE, leftShootImage[i]->data);
		
		unsigned char *silhouetteData = buildAlphaData(leftShootImage[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h,
					0, GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);
		free(silhouetteData);
	}
}


void initImageToRight(){
	
	rightImage[0] = ppm6GetImage("./images/stayR1.ppm");
	rightImage[1] = ppm6GetImage("./images/stayR2.ppm");
	rightWalkImage[0] = ppm6GetImage("./images/walkR1.ppm");
	rightWalkImage[1] = ppm6GetImage("./images/walkR2.ppm");
	rightWalkImage[2] = ppm6GetImage("./images/walkR3.ppm");
	rightJumpImage[0] = ppm6GetImage("./images/jumpR1.ppm");
	rightJumpImage[1] = ppm6GetImage("./images/jumpR2.ppm");
	rightShootImage[0] = ppm6GetImage("./images/shootR1.ppm");
	rightShootImage[1] = ppm6GetImage("./images/shootR2.ppm");
	rightShootImage[2] = ppm6GetImage("./images/shootR3.ppm");
	rightShootImage[3] = ppm6GetImage("./images/shootR4.ppm");
	rightShootImage[4] = ppm6GetImage("./images/shootR5.ppm");
	
	//init stay Image
	for(int i=0; i<2; i++){
		
		glGenTextures(1, &rightTexture[i]);
		glGenTextures(1, &silhouetteTexture);
		
		int w = rightImage[i]->width;
        int h = rightImage[i]->height;
        
		glBindTexture(GL_TEXTURE_2D, rightTexture[i]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 3 ,w ,h , 0, 
                GL_RGB, GL_UNSIGNED_BYTE, rightImage[i]->data);
		
		unsigned char *silhouetteData = buildAlphaData(rightImage[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h,
					0, GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);
		free(silhouetteData);
	}
	
	//init walk Image
	for(int i=0; i<3; i++){
		
		glGenTextures(1, &rightWalkTexture[i]);
		glGenTextures(1, &silhouetteTexture);
		
		int w = rightWalkImage[i]->width;
        int h = rightWalkImage[i]->height;
        
		glBindTexture(GL_TEXTURE_2D, rightWalkTexture[i]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 3 ,w ,h , 0, 
                GL_RGB, GL_UNSIGNED_BYTE, rightWalkImage[i]->data);
		
		unsigned char *silhouetteData = buildAlphaData(rightWalkImage[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h,
					0, GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);
		free(silhouetteData);
	}
	
	//init jump Image
	for(int i=0; i<2; i++){
		
		glGenTextures(1, &rightJumpTexture[i]);
		glGenTextures(1, &silhouetteTexture);
		
		int w = rightJumpImage[i]->width;
        int h = rightJumpImage[i]->height;
        
		glBindTexture(GL_TEXTURE_2D, rightJumpTexture[i]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 3 ,w ,h , 0, 
                GL_RGB, GL_UNSIGNED_BYTE, rightJumpImage[i]->data);
		
		unsigned char *silhouetteData = buildAlphaData(rightJumpImage[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h,
					0, GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);
		free(silhouetteData);
	}
	
	//init shoot Image
	for(int i=0; i<5; i++){
		
		glGenTextures(1, &rightShootTexture[i]);
		glGenTextures(1, &silhouetteTexture);
		
		int w = rightShootImage[i]->width;
        int h = rightShootImage[i]->height;
        
		glBindTexture(GL_TEXTURE_2D, rightShootTexture[i]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 3 ,w ,h , 0, 
                GL_RGB, GL_UNSIGNED_BYTE, rightShootImage[i]->data);
		
		unsigned char *silhouetteData = buildAlphaData(rightShootImage[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h,
					0, GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);
		free(silhouetteData);
	}
}



void stayAnime(bool left, bool right){
	
	double curanim;
    int curanimtime;
    curanim = timeDiff(&timeCharacter, &timeCurrent);
    curanim *= 10;
    curanimtime = (int) curanim;
    curanimtime = curanimtime%2;
    glColor3ub(255,255,255);//set color to pure white to avoid blending 
    
    //draw a recangle at character's position using run sprite
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);
    
    
    if(!left && right){
		glBindTexture(GL_TEXTURE_2D, rightTexture[curanimtime]);    
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f,1.0f); 
			glVertex2i(-22, -25);
			glTexCoord2f(0.0f,0.0f); 
			glVertex2i(-22, 25);
			glTexCoord2f(1.0f,0.0f); 
			glVertex2i(22, 25);
			glTexCoord2f(1.0f,1.0f); 
			glVertex2i(22, -25);
		glEnd();
    }
    else if(left && !right){
		glBindTexture(GL_TEXTURE_2D, leftTexture[curanimtime]);
        glBegin(GL_QUADS);
			glTexCoord2f(0.0f,1.0f); 
			glVertex2i(-22, -25);
			glTexCoord2f(0.0f,0.0f); 
			glVertex2i(-22, 25);
			glTexCoord2f(1.0f,0.0f); 
			glVertex2i(22, 25);
			glTexCoord2f(1.0f,1.0f); 
			glVertex2i(22, -25);
		glEnd();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    //disable alpha blending to avoid conflict on other draw functions
    glDisable(GL_ALPHA_TEST);
    //glDisable(GL_TEXTURE_2D);
    glPopMatrix();

	
}



void walkAnime(bool left, bool right){
	
	double curanim;
    int curanimtime;
    curanim = timeDiff(&timeCharacter, &timeCurrent);
    curanim *= 10;
    curanimtime = (int) curanim;
    curanimtime = curanimtime%3;
    glColor3ub(255,255,255);//set color to pure white to avoid blending 
    
    //draw a rectangle at character's position using run sprite
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);
    
    
    if(!left && right){
		glBindTexture(GL_TEXTURE_2D, rightWalkTexture[curanimtime]);    
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f,1.0f); 
			glVertex2i(-22, -25);
			glTexCoord2f(0.0f,0.0f); 
			glVertex2i(-22, 25);
			glTexCoord2f(1.0f,0.0f); 
			glVertex2i(22, 25);
			glTexCoord2f(1.0f,1.0f); 
			glVertex2i(22, -25);
		glEnd();
    }
    else if(left && !right){
		glBindTexture(GL_TEXTURE_2D, leftWalkTexture[curanimtime]);
        glBegin(GL_QUADS);
			glTexCoord2f(0.0f,1.0f); 
			glVertex2i(-22, -25);
			glTexCoord2f(0.0f,0.0f); 
			glVertex2i(-22, 25);
			glTexCoord2f(1.0f,0.0f); 
			glVertex2i(22, 25);
			glTexCoord2f(1.0f,1.0f); 
			glVertex2i(22, -25);
		glEnd();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    //disable alpha blending to avoid conflict on other draw functions
    glDisable(GL_ALPHA_TEST);
    //glDisable(GL_TEXTURE_2D);
    glPopMatrix();

	
}

void jumpAnime(bool left, bool right){
	
	double curanim;
    int curanimtime;
    curanim = timeDiff(&timeCharacter, &timeCurrent);
    curanim *= 10;
    curanimtime = (int) curanim;
    curanimtime = curanimtime%3;
    glColor3ub(255,255,255);//set color to pure white to avoid blending 
    
    //draw a rectangle at character's position using run sprite
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);
    
    
    if(!left && right){
	//	while(game.character.velocity!=0.0){
			glBindTexture(GL_TEXTURE_2D, rightJumpTexture[0]);    
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f,1.0f); 
				glVertex2i(-22, -25);
				glTexCoord2f(0.0f,0.0f); 
				glVertex2i(-22, 25);
				glTexCoord2f(1.0f,0.0f); 
				glVertex2i(22, 25);
				glTexCoord2f(1.0f,1.0f); 
				glVertex2i(22, -25);
			glEnd();
	//	}
		glBindTexture(GL_TEXTURE_2D, rightJumpTexture[1]);    
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f,1.0f); 
			glVertex2i(-22, -25);
			glTexCoord2f(0.0f,0.0f); 
			glVertex2i(-22, 25);
			glTexCoord2f(1.0f,0.0f); 
			glVertex2i(22, 25);
			glTexCoord2f(1.0f,1.0f); 
			glVertex2i(22, -25);
		glEnd();
    }
    else if(left && !right){
		glBindTexture(GL_TEXTURE_2D, leftJumpTexture[curanimtime]);
        	glBegin(GL_QUADS);
			glTexCoord2f(0.0f,1.0f); 
			glVertex2i(-22, -25);
			glTexCoord2f(0.0f,0.0f); 
			glVertex2i(-22, 25);
			glTexCoord2f(1.0f,0.0f); 
			glVertex2i(22, 25);
			glTexCoord2f(1.0f,1.0f); 
			glVertex2i(22, -25);
		glEnd();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    //disable alpha blending to avoid conflict on other draw functions
    glDisable(GL_ALPHA_TEST);
    //glDisable(GL_TEXTURE_2D);
    glPopMatrix();

	
}

void shootAnime(bool left, bool right, char a){
	
	double curanim;
    int curanimtime;
    curanim = timeDiff(&timeCharacter, &timeCurrent);
    curanim *= 10;
    curanimtime = (int) curanim;
	
	
	if(a == 'a')	//stay and shoot
		curanimtime = 0;
	else if(a == 'r')	//run and shoot
		curanimtime = curanimtime%3 + 1;
	else if(a == 'j')	//jump and shoot
		curanimtime = 4;
		
    glColor3ub(255,255,255);//set color to pure white to avoid blending 
    
    //draw a rectangle at character's position using run sprite
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);
    
    
    if(!left && right){
		glBindTexture(GL_TEXTURE_2D, rightShootTexture[curanimtime]);    
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f,1.0f); 
			glVertex2i(-22, -25);
			glTexCoord2f(0.0f,0.0f); 
			glVertex2i(-22, 25);
			glTexCoord2f(1.0f,0.0f); 
			glVertex2i(22, 25);
			glTexCoord2f(1.0f,1.0f); 
			glVertex2i(22, -25);
		glEnd();
    }
    else if(left && !right){
		glBindTexture(GL_TEXTURE_2D, leftShootTexture[curanimtime]);
        glBegin(GL_QUADS);
			glTexCoord2f(0.0f,1.0f); 
			glVertex2i(-22, -25);
			glTexCoord2f(0.0f,0.0f); 
			glVertex2i(-22, 25);
			glTexCoord2f(1.0f,0.0f); 
			glVertex2i(22, 25);
			glTexCoord2f(1.0f,1.0f); 
			glVertex2i(22, -25);
		glEnd();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    //disable alpha blending to avoid conflict on other draw functions
    glDisable(GL_ALPHA_TEST);
    //glDisable(GL_TEXTURE_2D);
    glPopMatrix();

	
}
