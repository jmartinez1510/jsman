//Luis Arevalo
//CMPS 335
//luisA.cpp
//Making a min-boss: Creation, deletion, movement,
//and, collision.
//https://github.com/bethgrace5/game/tree/6f9b753b1490239e59ff9fcceafb3efdd071b918
#include <iostream>
#include <cstddef>
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

GLuint silhouetteTexture2;
int silhouette2 = 1;
Ppmimage *eImage2 = NULL;
GLuint eTexture3;
int e = 1; 
miniBoss *mbhead = NULL;

void maker_miniBoss(Game *game)
{
	glColor3ub(90,140,90);
	game->mb.width = 90;
	game->mb.height = 90; //50, 25
	game->mb.center.x = 450;
	game->mb.center.y = 200;
}

void miniBossShooter(Game *g, miniBoss *s)
{
}

void bullet_collision(miniBoss *mb)
{
}

void hitponts_miniBoss(Game *g)
{
  //collision when mini-boss is hit by bullet
  
}

void delete_miniBoss(Game *g,miniBoss *node)
{
    //remove a node from linked list
    if (node->prev == NULL) 
    {
	if (node->next == NULL) 
	{
		mbhead = NULL;
	}
	else
	{
		node->next->prev = NULL;
		mbhead = node->next;
	}
    } 
    else
    {
	if (node->next == NULL)
	{
		node->prev->next = NULL;
	}
	else
	{
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
    }
    
    delete node;
    node = NULL;
}

void render_miniBoss(void)
{
	//Draw mini-boss
	glColor3ub(90,140,90);
  	
  	//Eimage = ppm6GetImage("./images/enemy1.ppm");
	
	glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.0f);	

        glBindTexture(GL_TEXTURE_2D, eTexture3);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2i(-22, -25); //0,0
        glTexCoord2f(0.0f, 0.0f); glVertex2i(-22, 25); //0,50
        glTexCoord2f(1.0f, 0.0f); glVertex2i(22, 25); //44,50
        glTexCoord2f(1.0f, 1.0f); glVertex2i(22, -25); //44,0
        glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_ALPHA_TEST);
	
}

