// scenario design and definition.

/*
  The Valley......

  Based on the game from computing today,
  originaly ported for pet's , trs-80's and mz80k's,

  Ported to Borland c++, By Dave T. June 1992.

 */

#include <stdlib.h>
#include <string.h>

#include "header.h"
#include "utils.h"

void lake(int,int,int,int,char *);
void add_rooms(char *,int);
void add_specials(char *);

// array of semi-random numbers for generation of temple/lair/tower.
int ca[]={4,13,7,9,7,8,4,5,11,6,13,5,7,6,8,6,8};
int nca=17;


// the function to draw the valley picture on the screen.
void
draw_valley(void)
{
	// first design a valley ( not if already done!)
	if (player.s==1) design_valley(valley_data[1]);

	// title information for screen map,
	static const char title[][30]={"","The Valley","Swamplands","Woodlands","Black Tower", \
			"Temple of Y'Nagioth","Vounims Lair"};

	immedok(mainscr,FALSE);
	clrscr();

	for (int x=1; x<=42; x++)        // draw a border for the map,
	{
		mvaddch(2,x,'_');
		mvaddch(43,x,'_');
	}
	for (int y=3;y<=43;y++)
	{
		mvaddch(y,1,'|');
		mvaddch(y,42,'|');
	}
	mvaddch(2,1,'+');
	mvaddch(43,1,'+');
	mvaddch(2,42,'+');
	mvaddch(43,42,'+');


	mvprintw(1,15,"%s",title[player.s]);

	// if in the tower, show floor number
	if (player.s==4) printw(" Floor : %f",(player.fl-2));

	if (debug) printw(" - Debug Mode.");    // if debugging, say so!

	for (int x=0;x<1600;x++)                    // then print the map for the
	{                                        // present scene (s)
		gotoxy((x%40)+2,(x/40)+3);
		addch((char)valley_data[player.s][x]);
	}
	refresh();
	immedok(mainscr,TRUE);

}


// checks to see if the player is trying to leave too soon (chicken!)
void
scene_exit(void)
{
	if (player.tn-player.tf<(vRandom(6)+2))       // 2+rnd(6) turns must elapse before allowed.
	{
		gotoxy(45,32);                    // sorry guv, no go yet!
		printw("The way is barred!");    // tell them so...
		player.w=player.m;                            // restore position and
		pk=cBlank;                        // character under player
		combat_get(1);                    // delay to allow message read,
		update_char();                // then update the character,
		return;                            // and return to caller
	}
	pk=cBlank;
	new_scene();                        // otherwise allow player to leave.
}

// new scene decides the info for a scene change, and does design calling.
void
new_scene(void)
{
	if (q1==cCastle && q==cWater)        // tried to enter lair/temple from water?
	{
		gotoxy(45,26);                            // sorry, not allowed
		printw("Cannot enter from the water!");
		combat_get(1);
		update_char();
		player.w=player.m;
		pk=q;
		q1=q;
		return;
	}

	if (player.s==1) player.mp=player.m;                // store position, (if in valley)
	player.tf=player.tn;                        // turn count holder to stop immediate exit
	player.c-=10;                        // decreace stamina from entering.

	if (q1==cSwamp ) { player.s=2;player.fl=2; }                // valley       -> swamp
	if (q1==cWoods ) { player.s=3;player.fl=3; }                // valley       -> woods
	if (q1==cTower ) { player.s=4; player.fl=2; }            // valley       -> tower
	if (q1==cBorder) { player.s=1; player.fl=1;}               // woods/swamps -> valley
	if (q1==cDoor && player.s==4) {player.s=1;player.fl=1;}        // tower        -> valley
	else if (q1==cDoor) {player.s-=3;player.fl-=4;player.m=player.mw; }    // temple/lair  -> swamp/wood
	if (q1==cCastle ) { player.s+=3;player.fl+=4;player.mw=player.m;}       // swamp/wood   -> temple/lair

	pk=cBlank;

	switch (player.s)            // what do do in each case
	{
	case 1:
		strcpy(fs,"VAEGH");             // sets monster select string
		player.m=player.mp;                            // restores leaving position
		design_valley(valley_data[player.s]);
		break;
	case 2:
		strcpy(fs,"AFL");
		if (q1==cDoor) player.m=player.mw;                // if leaving castle, get old pos.
		else
		{
			player.mp=player.m;                        // else save valley pos
			player.m=1540;                     // and puts player at bottom.
		}
		design_swamp(valley_data[player.s]);    // then designs the swamp
		break;
	case 3:
		strcpy(fs,"FAEHL");
		if (q1==cDoor) player.m=player.mw;            // if leaving castle, get old pos.
		else
		{
			player.mp=player.m;                    // else save valley pos
			player.m=1540;
		}
		design_woods(valley_data[player.s]);
		break;
	case 4:
		if (q1!=cStairs)                // if on stairs just move floor
		{
			strcpy(fs,"CAGE");      // else save info & design
			player.mp=player.m;
			player.m=1540;
		}
		design_tower(valley_data[player.s]);    // design floor.
		break;
	case 5:
		strcpy(fs,"CBE");
		player.mw=player.m;                            // stores position in 2ndary store
		player.m=1540;
		design_temple(valley_data[player.s]);
		break;
	case 6:
		strcpy(fs,"CBE");
		player.mw=player.m;
		player.m=1540;
		design_lair(valley_data[player.s]);
		break;
	}

	draw_valley();                            // draws the new scenario
	update_char();                        // updates character,
}                                            // and returns to caller

// then the scenario design routines, each only designes once.

// the design valley function,
void
design_valley(char *dat)
{
	strcpy(fs,"VAEGH"); player.fl=1; player.s=1;           // set up strings for monster choice
	static int done=0;                    // flag to stop multiple designing.
	if (done) return;                    // return if done,
	done=1;                                // otherwise set done, & do it...

	int c1=vRandom(10)+15;          // the left castle position.(y)
	player.m=c1*40;                    // calculate map position
	player.w=player.m;                        // make old position it as well,

	dat[c1*40]=cExit;                // put a castle there,
	// then design the path.

	int xp=1,yp=c1;                // start at 1,c1 ( right 1 of castle 1)

	while (1)                    // infinite loop, breaks on completion
	{
		if (xp==39 || yp==39)                // if finished,
		{
			dat[xp+yp*40]=cExit;    // give end a castle,
			break;                // and continue with other parts
		}

		switch (vRandom(5)) {    // choice of path direction,
		case 0:
		case 1:                // upwards path (North East)
			if (yp<39)
			{
				dat[yp*40+xp]='/';
				xp++;
				yp--;
				break;
			}
			break;
		case 2:
		case 3:
			if (yp>0)        // downwards path (South East)
			{
				yp++;
				dat[yp*40+xp]='\\';
				xp++;
				break;
			}
			break;

		case 4:                // horizontal (East)
			dat[yp*40+xp]='_';
			xp++;
			break;
		}
	}

	// once the path has been drawn, we can now put in the tower,swamps & woods

	int p;                    // storage for positions,

	// d counts for the number of woods & swamps, e.g. 2 = 2 woods + 2 swamps

	for (int d=0;d<2;d++)
	{
            do { p=vRandom(400)*4; } while (dat[p]!=cBlank || dat[p+1]!=cBlank);
            dat[p]=cSwamp; dat[p+1]=cSwamp;            // put swamp chars,
            
            do { p=vRandom(400)*4; } while (dat[p]!=cBlank || dat[p+1]!=cBlank);
            dat[p]=cWoods; dat[p+1]=cWoods;            // put swamp chars,
	}                 // end of loop for number of woods & swamps

        do { p=vRandom(400)*4; } while (dat[p] != cBlank);
	dat[p]=cTower;            // put the tower

}                                // design complete, return to caller

void
design_swamp(char *dat)
{
	static int done=false;
	pk=cBlank;                        // put space under player,
	if (done) return;
	done=true;
	for (int c=0; c<400; c++) dat[vRandom(1600)]=cMarsh; // add marsh,

	for (int x=0;x<40;x++)            // build borders
	{
		dat[x]=cBorder;
		dat[1599-x]=cBorder;
		dat[x*40]=cBorder;
		dat[x*40+39]=cBorder;
	}
	add_lake(dat);                    // then add the lake + temple
}

void
design_woods(char *dat)
{
	static int done=false;
	pk=cBlank;                        // put space under player,
	if (done) return;
	done=true;
	for (int c=0; c<400; c++) dat[vRandom(1600)]=cTree; // add trees,
	for (int x=0;x<40;x++)            // build borders
	{
		dat[x]=cBorder;
		dat[1599-x]=cBorder;
		dat[x*40]=cBorder;
		dat[x*40+39]=cBorder;
	}
	add_lake(dat);                // add the lake & lair/temple.
}

void
design_tower(char *dat)
{
	int sp=0;                            // stair position
	static int done=false;
	pk=cBlank;                        // put space under player,
	if (done) memset(dat,cBlank,1600);
	done=true;
	for (int x=0;x<40;x++)            // build borders
	{
		dat[x]=cWall;
		dat[1599-x]=cWall;
		dat[x*40]=cWall;
		dat[x*40+39]=cWall;
	}
	add_rooms(dat,player.fl+1);
	if (player.fl==2) dat[1580]=cDoor;        // put in exit. (if ground floor!)

	while (1)                        // loop to ensure stairs not under player.
	{
		switch (vRandom(4))                // choose a corner for the stairs,
		{                                // but not the players corner.
		case 1:                        // (can't be too easy!)
			sp=1521;
			break;
		case 2:
			sp=1558;
			break;
		case 3:
			sp=41;
			break;
		case 0:
			sp=78;
			break;
		}
		if (sp!=player.m) { dat[sp]=cStairs; break; }    // if player not on stairs now, end
	}
	add_specials(dat);
}


void
design_temple(char *dat)
{
	static int done=false;
	pk=cBlank;                        // put space under player,
	if (done)
	{ // reset "specials"
		for (int x=0;x<1600;x++) dat[x]=(dat[x]==cSpecial)?cBlank:dat[x];
		add_specials(dat);
		return;
	}
	done=true;
	for (int x=0;x<40;x++)            // build borders
	{
		dat[x]=cWall;
		dat[1599-x]=cWall;
		dat[x*40]=cWall;
		dat[x*40+39]=cWall;
	}
	add_rooms(dat,vRandom(nca));
	dat[1580]=cDoor;                // put in exit.
	add_specials(dat);              // and add the specials
}

void
design_lair(char *dat)
{
	static int done=false;
	pk=cBlank;                        // put space under player,
	if (done)
	{
		for (int x=0;x<1600;x++) dat[x]=(dat[x]==cSpecial)?cBlank:dat[x];
		add_specials(dat);
		return;
	}
	done=true;
	for (int x=0;x<40;x++)            // build borders
	{
		dat[x]=cWall;
		dat[1599-x]=cWall;
		dat[x*40]=cWall;
		dat[x*40+39]=cWall;
	}

	add_rooms(dat,vRandom(nca));
	dat[1580]=cDoor;                // put in exit.
	add_specials(dat);
}

void                        // lake adder, calls recursive lake routine
add_lake(char *dat)
{
    int cx,cy,p;
    cx=vRandom(20)+10;        // random position x & y
    cy=vRandom(20)+10;
    lake(cx,cy,cx,cy,dat);    // then calls the lake design routine
    p=cy*40+cx;
    dat[p]=cCastle;                // add castle,
    dat[p+1]=cBlank;            // space to right,
    dat[p-40]=cBlank;            // space above,
    dat[p-39]=cBlank;            // space above left,
}


// recursive lake design algorithm,
// builds a lake around initial point cx,cy based on distance of x,y from
// cx & cy.

void
lake(int cx,int cy,int x, int y, char *dat)
{
    int d,p;                                // distance from centre holder
    p=y*40+x;
    if (dat[p]==cWater) return;
    dat[y*40+x]=cWater;                        // add lake pos,
    d=((cx-x)*(cx-x)+(cy-y)*(cy-y));        // calculate distance ^2;
    if (d!=0 && (((float)vRandom(100))/100 > ((4+vRandom(4))/d)))// if far enough,
            return;                                // return

    lake(cx,cy,x+1,y,dat);                    // right,
    lake(cx,cy,x-1,y,dat);                    // left,
    lake(cx,cy,x,y+1,dat);                    // down,
    lake(cx,cy,x,y-1,dat);                    // up.
    return;
}

// and then a function to add the rooms to a building scenario
void
add_rooms(char *dat,int p)
{
	int h=0,g=0,osx=0,dd;

	int sy=2,sx=2;                // store for y position.
	sx=ca[p];                // 1st room dimensions,

	while (1)            // while still in room dimensions.
	{
            if (sx>38){ sx=38; h=1;}

            p++;                    // move to next in array,
            p%=nca;                    // not past end...

            sy=ca[p];
            p++;
            p%=nca;

            dd=vRandom(33)+3;
            for(int t=1;t<39;t++)
            {
                if (sx<36) dat[(int)t*40+sx]=cWall;
                if (t==dd) dat[(int)t*40+sx]=cBlank;        // put hole
            }

            while (1)
            {
                if (sx>36) sx=39;
                if (sy>38) {sy=38 ; g=1; }
                dd=vRandom(3)+1;
                for (int t=osx+1;t<sx;t++)
                {
                    if (sy<38) dat[sy*40+t]=cWall;
                    if (t==(int)(((sx-osx)*dd)/4)+osx) dat[sy*40+t]=cBlank;    // put hole in wall
                }
                sy+=ca[p];
                p++;
                p%=nca;
                if (g) break;
                if (sx==39) sx--;
            }
            g=0;
            osx=sx;
            sx+=ca[p];
            p++;
            p%=nca;
            if (h) break;
	}
	dat[1540]=cBlank;
}

void
add_specials(char *dat)
{
	if (player.fl<4) return;            // no specials on floors <2.
	int c=vRandom(5)+2,p=0;        // number of specials to place,
	for (int i=0;i<=c;i++)
	{
            do { p=vRandom(1600); } while (dat[p]!=cBlank) ;
            dat[p]=cSpecial;
	}
}

