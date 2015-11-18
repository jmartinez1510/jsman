//Jeremy Martinez
//
// This file adds all sound to every aspect of the game, first it generates a source for each
// sound and then sets the volume for each sound. There is currently only one sound that loops
// which is used for the menu and background music of the game. Each sound is then placed in 
// their respectable positions in order to play at the right time, such as at button press or
// collision. The other action this file takes care of is the character jump, which happens when
// the space button is pressed. The characters y velocity is incremented until it reaches a limit,
// then the character falls back down until it reaches a platform.
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <time.h>
#include "ppm.h"
#include "chrisR.h"
#include "danbiY.h"
#include </usr/include/AL/alut.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "struct.h"

ALuint alBuffer;
ALuint alSource;
ALuint alJump;
ALuint alBGM;
ALuint alMenu;
ALuint alDeath;
ALuint alHit;
ALuint alKill;

extern bool touchPlatform;
extern int keys[];

void playSound(ALuint source) 
{  
    alSourcePlay(alSource); 
}

void cleanupOpenal() 
{
	//Cleanup.
	//First delete the source.
	alDeleteSources(1, &alSource);
	//Delete the buffer.
	alDeleteBuffers(1, &alBuffer);
	//Close out OpenAL itself.
	//Get active context.
	ALCcontext *Context = alcGetCurrentContext();
	//Get device for active context.
	ALCdevice *Device = alcGetContextsDevice(Context);
	//Disable context.
	alcMakeContextCurrent(NULL);
	//Release context(s).
	alcDestroyContext(Context);
	//Close device.
	alcCloseDevice(Device);
	
}

int initOpenal() {
	//Get started right here.
	alutInit(0, NULL);
	//Clear error state.
	alGetError();
	//
	//Setup the listener.
	//Forward and up vectors are used.
	float vec[6] = {0.0f,0.0f,1.0f, 0.0f,1.0f,0.0f};
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListenerfv(AL_ORIENTATION, vec);
	alListenerf(AL_GAIN, 1.0f);
	//
	//Buffer holds the sound information.
	//ALuint alBuffer;
	alBuffer = alutCreateBufferFromFile("./shoot1.wav");
	//Source refers to the sound.
	//ALuint alSource;
	//Generate a source, and store it in a buffer.
	alGenSources(1, &alSource);
	alSourcei(alSource, AL_BUFFER, alBuffer);
	//Set volume and pitch to normal, no looping of sound.
	alSourcef(alSource, AL_GAIN, 2.0f);
	alSourcef(alSource, AL_PITCH, 1.0f);
	alSourcei(alSource, AL_LOOPING, AL_FALSE);
	//BGM
	alBuffer = alutCreateBufferFromFile("./sample2.wav");
	//Generate source
	alGenSources(1, &alBGM);
	alSourcei(alBGM, AL_BUFFER, alBuffer);
	//Set volume
	alSourcef(alBGM, AL_GAIN, 0.4f);
	alSourcef(alBGM, AL_PITCH, 1.0f);
	alSourcei(alBGM, AL_LOOPING, AL_TRUE);
	//jump
	alBuffer = alutCreateBufferFromFile("./jump.wav");
	//generate source
	alGenSources(1, &alJump);
	alSourcei(alJump, AL_BUFFER, alBuffer);
	//set volume
	alSourcef(alJump, AL_GAIN, 0.7f);
	alSourcef(alJump, AL_PITCH, 1.0f);
	alSourcei(alJump, AL_LOOPING, AL_FALSE);
	//Menu
	alBuffer = alutCreateBufferFromFile("./Menu.wav");
	//Generate source 
	alGenSources(1, &alMenu);
	alSourcei(alMenu, AL_BUFFER, alBuffer);
	//set volume
	alSourcef(alMenu, AL_GAIN, 1.2f);
	alSourcef(alMenu, AL_PITCH, 1.0f);
	alSourcei(alMenu, AL_LOOPING, AL_FALSE);
	//death
	alBuffer = alutCreateBufferFromFile("./death.wav");
	//Generate Source
	alGenSources(1, &alDeath);
	alSourcei(alDeath, AL_BUFFER, alBuffer);
	//set volume
	alSourcef(alDeath, AL_GAIN, 2.0f);
	alSourcef(alDeath, AL_PITCH, 1.0f);
	alSourcei(alDeath, AL_LOOPING, AL_FALSE);
	//Hit confirm
	alBuffer = alutCreateBufferFromFile("./hit.wav");
	//Generate Source
	alGenSources(1, &alHit);
	alSourcei(alHit, AL_BUFFER, alBuffer);
	//set volume
	alSourcef(alHit, AL_GAIN, 2.0f);
	alSourcef(alHit, AL_PITCH, 1.0f);
	alSourcei(alHit, AL_LOOPING, AL_FALSE);
	//enemy death
	alBuffer = alutCreateBufferFromFile("./kill.wav");
	//Generate Source
	alGenSources(1, &alKill);
	alSourcei(alKill, AL_BUFFER, alBuffer);
	//set volume
	alSourcef(alKill, AL_GAIN, 2.0f);
	alSourcef(alKill, AL_PITCH, 1.0f);
	alSourcei(alKill, AL_LOOPING, AL_FALSE);

	return 1;
}

void soundMenu()
{
	initOpenal();
	alSourcePlay(alMenu);
}

void soundHit()
{
	initOpenal();
        alSourcePlay(alHit);
	usleep(2000);	
}

void soundKill()
{
	initOpenal();
	alSourcePlay(alKill);
	usleep(2000);
}

void soundDeath()
{
	initOpenal();
	alSourcePlay(alDeath);
	usleep(2000);
}

void soundBGM()
{
	initOpenal();
	alSourcePlay(alBGM);
	usleep(20000);
}

void soundJump()
{
	initOpenal();
	alSourcePlay(alJump);
	usleep(2000);   
}

void soundBullet()
{
	initOpenal();
	alSourcePlay(alSource);
	usleep(20000);
}

void gravity(Game *game) {
	if(touchPlatform == 0) {
	game->character.velocity.y -= 1;
	game->character.center.y = game->character.center.y + game->character.velocity.y;
	}
	if(touchPlatform == 1)
	game->character.velocity.y = 0;
}

void characterJump(Game *game) 
{
	if (keys[XK_Up] ) 
	{
		if(touchPlatform == 1) 
		{
			while(game->character.velocity.y < 17)
				game->character.velocity.y = game->character.velocity.y + 1; 
			if(touchPlatform > 0)
			{
				soundJump();
			}
			touchPlatform = 0;	
		}
	} 
}

void drawHealth(float x, int health)
{
        int y = 550;
	x = x+240;
	//draw black box
	glColor3ub(0,0,0);
	glBegin(GL_QUADS);
	glVertex2i(x-105,y-10);
	glVertex2i(x-105,y+10);
	glVertex2i(x+105,y+10);
	glVertex2i(x+105,y-10);
	glEnd();
	glColor3ub(200,0,0);
	glBegin(GL_QUADS);
	glVertex2i(x-100,y-7);
	glVertex2i(x-100,y+7);
	glVertex2i(x+100,y+7);
	glVertex2i(x+100,y-7);
	glEnd();
	glColor3ub(0,200,0);
	glBegin(GL_QUADS);
	glVertex2i(x-100,y-7);
	glVertex2i(x-100,y+7);
	glVertex2i(x-100+(health * 2),y+7);
	glVertex2i(x-100+(health * 2),y-7);
	glEnd();
}

void stage2(Game *game) 
{
	/*game->platform[4].width = 252; 
	game->platform[4].height = 24; 
	game->platform[4].center.x = 2650; 
	game->platform[4].center.y = 250;
	game->platform[5].width = 252; 
	game->platform[5].height = 24; 
	game->platform[5].center.x = 3200;
	game->platform[5].center.y = 280;
	game->platform[6].width = 252; 
	game->platform[6].height = 24; 
	game->platform[6].center.x = 3800;
	game->platform[6].center.y = 150;*/
		game->platform[4].width = 252; 
	game->platform[4].height = 24; 
	game->platform[4].center.x = 3310; 
	game->platform[4].center.y = 160;
	/*game->platform[5].width = 252; 
	game->platform[5].height = 24; 
	game->platform[5].center.x = 3200;
	game->platform[5].center.y = 280;
	game->platform[6].width = 252; 
	game->platform[6].height = 24; 
	game->platform[6].center.x = 3800;
	game->platform[6].center.y = 150;*/
	
}
