#ifndef _DANBIY_H_
#define _DANBIY_H_


extern void menu(void);
extern void menuRender(void);
extern void initButton(void);
extern void checkMouse(XEvent *);
extern void initImageToLeft();
extern void initImageToRight();
extern void stayAnime(bool, bool);
extern void walkAnime(bool, bool);
extern void jumpAnime(bool, bool);
extern void shootAnime(bool, bool, char);


#endif
