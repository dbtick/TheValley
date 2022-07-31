/* Some utils for Valley on GCC 
 *
 */
 /* Version history 
  * 0.1 First draft
  */

#include <ncurses.h>
#include <unistd.h>
extern WINDOW *mainscr;

int vScrInit();
int vScrTerm();
int gotoxy(int,int);

#define gotoxy(x,y) move(y,x)
#define clrscr() erase()
#define vRandom(x) (rand()%x)

const char cTree    ='^';   // ^
const char cWall    ='X'; 	// \262
const char cBorder  ='%'; // \261
const char cCastle  ='&'; // \236
const char cWater   ='.';  // \260
const char cDoor    ='=';   // = 
const char cStairs  ='>';  // \?
const char cSpecial ='*'; // *
const char cPlayer  ='@';  // @
const char cWoods   ='w';   // W
const char cSwamp   ='s';   // S
const char cExit    ='#';    // #
const char cTower   ='T';   // \333
const char cBlank   =' ';   // ' ' and '\0'
const char cMarsh   ='~';   // ~ - marsh ground in swamp (not water)
