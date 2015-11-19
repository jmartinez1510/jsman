
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

using namespace std;

extern "C" {    //adding in Text for wording on the blocks
#include "fonts.h"
}




extern double timeDiff(struct timespec *, struct timespec *);
struct timespec timeHit;

#define MAX_PARTICLES 4000
#define GRAVITY 0.1

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;

void renderBG(void); 
void renderJS(void); 
void renderPlat(void);

void renderMovePlat(void);

void renderTitle(void);

void enemyMaker(void);
void enemy(void);

void bulletRender(void);


int initOpenal(void);
void playSound(void);
void cleanupOpenal(void);

//////////////////danbi add

Game game;
bool lGo = false;

/*const double oobillion = 1.0 / 1e9;
struct timespec timeStart, timeCurrent;
struct timespec timePause;*/

bool collision(Character *c, Shape *s);
bool walkercollision(WalkEnemy *wE, Shape *s);

bool touchPlatform = 0;
int lives = 3;
int beginX = 0;
int endX= WINDOW_WIDTH;
int beginY = 0;
int endY = WINDOW_HEIGHT;
int keys[65536]; //just added
//float gravity = 0.5f;

int v =0;

int speedJump = 17;
bool titleClick = 0;
int title = 1;
int pauseG = 0;

bool EnemyTouchPlatform;

bool play_game=false;
bool exit_game=false;
bool done = false;

int gotHit = 0;

/*ALuint alBuffer;
ALuint alSource;
ALuint alJump;
ALuint alBGM;
ALuint alMenu;
ALuint alDeath;
*/

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e, Game *game);
int check_keys(XEvent *e, Game *game);
void movement(Game *game);
void gravity(Game *game); //just added
void platformMaker(Game *game);
void characterPlatTopCollision(Game *game, Character *c);
void characterPlatLRBCollision(Game *game, Character *c);
void crush(Game *game);
void crushGravity(Game *game);
void crushCollision(Game *game);
void crushSprite(void);
void renderCrush(void);

void right(Game *game);
void left(Game *game);
void characterJump(Game *game);
void EnemyGravity(Game *game);
void bulletMovement(Game *game, WalkEnemy *w);
void bulletPress(Game *game, Character *c);
void gravity(Game *game);

void charEnemyColl(Game *game);
void drawHealth(float, int);
void gotHitAlready(Game *game);

void cameraControl();

void soundDeath(void);
void soundBGM(void);
void soundMenu(void);
void soundBullet(void);
void soundJump(void);
void stage2(Game *game);

//void cutsc(Game *game); //cutscene function prototype

void createMovePlat(Game *game);
void movingPlatform(Game *game);
void MPStop(Game *game);
void movingPlatformCollision(Game *game, Character *c);
void movingPlatTopCollision(Game *game, Character *c);

void movePlatform(void);

void characterMaker(Game *game); //EXPERIMENTAL!!!!!!!!!!!!1

void maker_miniBoss(Game *game);
void delete_miniBoss(Game *g, miniBoss *node);
void render_miniBoss(void); 

void render(Game *game);

//int cutscene[360000];  //cutscene variables
//int cutsceneT = 0;

int main(void)
{

	int done=0;
	srand(time(NULL));
	initXWindows();
	init_opengl();
	//declare game object
	game.n=0;
	
	initButton();
	menu();

	
	//character
	//game.character.width = 22;
	//game.character.height = 15; //50, 25
	//game.character.center.x = 400;
	//game.character.center.y = 200;
	characterMaker(&game);
	
	game.walkers[0].width = 22;
	game.walkers[0].height = 14;
	game.walkers[0].center.x = 700;
	game.walkers[0].center.y = 275;

	game.walkers[1].width = 28.5;
	game.walkers[1].height = 22;
	game.walkers[1].center.x = 750;
	game.walkers[1].center.y = 275;

	platformMaker(&game);
	createMovePlat(&game);
	stage2(&game);
	

	game.crushJ.width = 80;
	game.crushJ.height = 100;
	game.crushJ.center.x = -2000;
	game.crushJ.center.y = -2000;

	//titleS();

	/*initOpenal();	
	for (int i=0; i<10; i++) {
                alSourcePlay(alSource);
                usleep(250000);
        }*/

	initImageToLeft();
	initImageToRight();
	
	platform();
	movePlatform();
	 //Don't put it in init open gl, lags everything up
	enemy();
	crushSprite();
	maker_miniBoss(&game);

	background();
	soundBGM();

	//start animation
	while(!done) {
	      		while(XPending(dpy)) {
			XEvent e;
			XNextEvent(dpy, &e);
			
			checkMouse(&e);
			check_mouse(&e, &game);
			done = check_keys(&e, &game);
			
		}
		if(title == 0) {
			if(pauseG == 0) {
			gravity(&game);
			movingPlatform(&game);
			//cutsc(&game);   //cutscene enabler
			EnemyGravity(&game);
			crushGravity(&game);
			//if(cutsceneT ==0) {  //cutscene enabler
			movement(&game); }
			init_opengl();
			
			//}   //cutscene enabler
       		}
       		else{
			if(exit_game==true){
				return 0;
			}
		}
		render(&game); 
		
		//if(game.character.health <= 0)
		//    lives--;
		//if lives reach zero, exit game
		if(lives == 0)
		{
		    return 0;
		    cleanupOpenal();
		}
		glXSwapBuffers(dpy, win);    
	}
	cleanupXWindows();
	cleanupOpenal();
	return 0;
}




void set_title(void)
{
    //Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "Jump Shoot Man");
}

void cleanupXWindows(void) {
	//do not change
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

void initXWindows(void) {
	//do not change
	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	int w=WINDOW_WIDTH, h=WINDOW_HEIGHT;
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
		std::cout << "\n\tcannot connect to X server\n" << std::endl;
		exit(EXIT_FAILURE);
	}
	Window root = DefaultRootWindow(dpy);
	XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	if(vi == NULL) {
		std::cout << "\n\tno appropriate visual found\n" << std::endl;
		exit(EXIT_FAILURE);
	} 
	Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	XSetWindowAttributes swa;
	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
							ButtonPress | ButtonReleaseMask |
							PointerMotionMask |
							StructureNotifyMask | SubstructureNotifyMask;
	win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
					InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
	set_title();
	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
}



/*void cutsc(Game *game) {   cutscene timer and manipulate sprites
	if(cutscene[600] == 0)
	if(game->character.center.x > 500) {
		cutsceneT = 1;
		cutscene[v] = 1;
		v++;
		cout << cutscene[20] << endl;
		cout << v ;
		if(v >= 300 && v <=400 ) {
		game->character.velocity.x = 1;
		game->character.center.x = game->character.center.x + game->character.velocity.x;
		beginX += 1;
		endX += 1;
		}
	}
	//cutscene[20] = 1;
	if(cutscene[600] == 1)
	cutsceneT = 0;
	//cout << cutsceneT << endl;
}*/

void init_opengl(void) //void
{
		
	//OpenGL initialization
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//Set 2D mode (no perspective)
	cameraControl();
	//Set the screen background color
	glClearColor(0.1, 0.1, 0.1, 1.0);

	glEnable(GL_TEXTURE_2D);  //adding this to enable fonts for blocks
	initialize_fonts();
}

#define rnd() (float)rand() / (float)RAND_MAX

unsigned char *buildAlphaData(Ppmimage *img)
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

void check_mouse(XEvent *e, Game *game)
{
	static int savex = 0;
	static int savey = 0;
	//static int n = 0;

	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			soundMenu();
			//Left button was pressed
			titleClick = 1;
			if(titleClick == 1)
			{
			    soundMenu();
			}
			return;
		}
		if (e->xbutton.button==3) {
			//Right button was pressed
			return;
		}
	}
	//Did the mouse move?
	if (savex != e->xbutton.x || savey != e->xbutton.y) {
		savex = e->xbutton.x;
		savey = e->xbutton.y;
		int y = WINDOW_HEIGHT - e->xbutton.y;
		//for(int i = 0; i<10; i++)
		    //makeParticle(game, e->xbutton.x, y);

		//if (++n < 10)
		//	return;
		game->lastMousex = e->xbutton.x; 
		game->lastMousey = y;
	}
}

int check_keys(XEvent *e, Game *game)
{
	//int shift;
	//Was there input from the keyboard?
	int key = XLookupKeysym(&e->xkey, 0);
	//Log("key: %i\n", key);
	if (e->type == KeyRelease) {
		keys[key]=0;
		if (key == XK_Shift_L || key == XK_Shift_R) {
			//shift=0;
		}
		return 0;
	}
	if (e->type == KeyPress) {
		//int key = XLookupKeysym(&e->xkey, 0);
		keys[key] = 1;
		if (key == XK_Shift_L || key == XK_Shift_R) {
			//shift=1;
			return 0;
		}
		
		if (key == XK_Escape) {
			return 1;
		}
		if (key == XK_p) {
			if(pauseG == 0)
			pauseG = 1;
			//if(pauseG == 1)
			//pauseG = 0;
		}
		if (key == XK_u) {
			if(pauseG == 1)
			pauseG = 0;
		}
		if (key == XK_c) 
		crush(game);
		
	}
	
		//You may check other keys here.

	
	return 0;
}




void movement(Game *game)
{
	
	Character *c;
	touchPlatform = 0;
	
	if(game->character.center.y < 0 || game->character.health <= 0){        //RESET if fell to pitfall, decrement lives count
		game->character.center.x = 400;
		game->character.center.y = 200;
		beginX = 0;
		endX = WINDOW_WIDTH;
		lives -= 1;
		game->character.health = 100;
		soundDeath();
		game->character.velocity.y = 0;
	//Bullet *b = bhead;  //Only deletes lastest bullet on screen, if no bullets, seg fault when hitting pitfall
	//	deleteBullet(b);
	}
	if (keys[XK_Right]) {
		right(game);
	}
	if (keys[XK_Left]) {
		left(game);
	}
	
	 
	c = &game->character;
	characterPlatTopCollision(game,c);

	characterPlatLRBCollision(game,c);
	movingPlatformCollision(game, c);
	movingPlatTopCollision(game, c);
	WalkEnemy *w;
	w = &game->walkers[0];
	for(int i = 0; i<PLATFORMS; i++) { 	
		
		Shape *e = &game->platform[i];
		if(walkercollision(w,e) == true) { 
			EnemyTouchPlatform = 1;
				
		}
	
	}

	for(int i = 0; i<PLATFORMS; i++) { 	
		Shape *e = &game->platform[i];
		if(walkercollision(w,e) == true) {
			if(w->center.x  < e->center.x - e->width) //for left side of platform
			if(w->center.x + w->width > e->center.x - e->width &&
			w->center.y - w->height <= e->center.y + e->height-5){
			w->center.x = e->center.x - e->width - c->width - 1;
				/*if(game->character.center.x >= (WINDOW_WIDTH/2)) { //stop camera movement if left collision or near beginning
					beginX -= 3;
					endX -= 3;
				}*/
			EnemyTouchPlatform = 0; 
			}
			/*if(w->center.x  > e->center.x + e->width)
				if(w->center.x - w->width < e->center.x + e->width &&
				w->center.y - w->height <= e->center.y + e->height-5) {
					w->center.x = e->center.x + e->width + w->width ;
					if(game->character.center.x >= 0) {
						beginX += 3;
						endX += 3;
					}
				EnemyTouchPlatform = 0;
				}
			if(w->center.y < e->center.y)
				if(w->center.y + w->height >= e->center.y - e->height &&
				w->center.x - w->width <= e->center.x + e->width &&
				w->center.x + w->width >= e->center.x - e->width) {
						w->center.y = e->center.y - e->height - w->height;
						touchPlatform = 0;
				}*/
		}
	}

	characterJump(game);	

	//bullet
	bulletMovement(game,w);

	if (keys[XK_space]) {
		bulletPress(game,c);
	}
	

	charEnemyColl(game);
	crushCollision(game);
	MPStop(game);	
}

void gotHitAlready(Game *game) {
	if(gotHit == 1){
		struct timespec bt;
		
		clock_gettime(CLOCK_REALTIME, &bt);
		double ts = timeDiff(&timeHit, &bt);
		if (ts > 120) {
			//game->character.health = game->character.health +0.9;
			gotHit = 0;
		}
	}
}
	

void charEnemyColl(Game *game) 
{
	if((game->character.center.x + game->character.width > game->walkers[0].center.x-15) && 
		(game->character.center.x- game->character.width < game->walkers[0].center.x +10) && 
		(game->character.center.y - 10 < game->walkers[0].center.y + 10) &&
		(game->character.center.y + 10 > game->walkers[0].center.y - 10)) 
	{
		game->character.health -= 20;
		//soundHit();
		if(game->character.center.x-10 < game->walkers[0].center.x+10) 
		{
			game->character.center.x -=35;
			beginX -= 35;
			endX -= 35;
		}
		if(game->character.center.x+10 > game->walkers[0].center.x-10) 
		{
			game->character.center.x +=35;
			beginX += 35;
			endX += 35;
		}
	}
}

void render(Game *game)
{

        //Rect r;    //Want text? Uncomment Rect r;
	//float w, h;
	glClear(GL_COLOR_BUFFER_BIT);

	//Draw shapes...
	
	renderBG();
	
	//draw platform
	
	Shape *e;
	glColor3ub(20,150,150);
	for(int i=0; i<5;i++) {
	e = &game->platform[i];
	glPushMatrix();
	glTranslatef(e->center.x, e->center.y, e->center.z);
	renderPlat();
	glPopMatrix();
	}
	MPlatform *zz;
	glColor3ub(20,150,150);
	for(int i=0; i<3;i++) {
	zz = &game->movePlatform[i];
	glPushMatrix();
	glTranslatef(zz->center.x, zz->center.y, zz->center.z);
	renderMovePlat();
	glPopMatrix();
	}

	//draw character
	
	Character *d;
	d = &game->character;
	glPushMatrix();
	//glColor3ub(100,100,100);
	glTranslatef(d->center.x, d->center.y, d->center.z);

		
	
	if(((keys[XK_Right])||(keys[XK_Left]))) {
		if((keys[XK_Right])){
			lGo = false;
			if(((!keys[XK_Up])&&!(keys[XK_space]))){
				walkAnime(lGo);
			}
			else if(((keys[XK_Up])&&!(keys[XK_space]))){		//jump to right
				jumpAnime(lGo);
			}
			else if((!(keys[XK_Up])&&keys[XK_space])){		//jump w/ shoot
				shootAnime(lGo, 'j');
			}				
			else if((keys[XK_space])){		//walk and shoot
				shootAnime(lGo, 'w');
			}
		}
		else{
			lGo = true;
			if(((!keys[XK_Up])&&!(keys[XK_space]))){
				walkAnime(lGo);
			}
			else if(((keys[XK_Up])&&!(keys[XK_space]))){		//jump to left
				jumpAnime(lGo);
			}
			else if((!(keys[XK_Up])&&keys[XK_space])){		//jump w/ shoot
				shootAnime(lGo, 'j');
			}				
			else if((keys[XK_space])){		//walk and shoot
				shootAnime(lGo, 'w');
			}
			
		}
	}
	else if(((keys[XK_Up])&&!(keys[XK_space]))){		//jump
		jumpAnime(lGo);
	}
	else if((!(keys[XK_Up])&&keys[XK_space])){		//jump w/ shoot
		shootAnime(lGo, 'j');
	}	
	else if((keys[XK_space])){		//stay and shoot
		shootAnime(lGo, 's');
	}
	else{
		stayAnime(lGo);
	}


	glPopMatrix();
	drawHealth(d->center.x, game->character.health);
	bulletRender();

	WalkEnemy *z;
	z = &game->walkers[0];
	glPushMatrix();
	glColor3ub(155,155,155);
	glTranslatef(z->center.x, z->center.y, z->center.z);
	enemyMaker();
	
	//draw mini-boss
	miniBoss *m;
	m = &game->mb;
	glPushMatrix();
	//glColor3ub(90,140,90);
	glTranslatef(m->center.x, m->center.y, m->center.z);
	render_miniBoss();
	glPopMatrix();

	Shape *cc;
	cc = &game->crushJ;
	glPushMatrix();
	glColor3ub(155,155,155);
	glTranslatef(cc->center.x, cc->center.y, cc->center.z);
	renderCrush();
	glPopMatrix();

	if(title)
		menuRender();
}

