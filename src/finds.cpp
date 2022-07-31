// the finds functions...

/*
        The Valley......

 Based on the game from computing today,
 originaly ported for pet's , trs-80's and mz80k's,

 Ported to Borland c++, By Dave T. June 1992.

 */
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "header.h"
#include "utils.h"

// if player finds something, this decides what it is,
void
find_select(void)
{
	gotoxy(45,32);
	switch (vRandom(6))        // 6 options,
	{
	case 1:                // 1 for circle of evil,
		printw("A circle of evil, leave in haste!");
		player.cs+=int((player.fl+1/2));        // takes off some str and psi.
		player.ps-=int((player.fl+1)/2);
		player.c=player.c-20;
		if (player.c<0) death();        // make sure not dead!
		break;
	case 2:                // 2 for gold,
	case 3:
		printw("A hoard of gold!");
		player.ts+=int(player.fl*vRandom(100)+100);
		break;
	case 4:
	case 5:                // 2 for aura,
		printw("You feel the aura of deep magic..");
		player.ps+=1+int(player.fl*player.pl);
		player.cs+=(int(0.5+player.fl*player.cl));
		player.c+=25;
		break;
	case 0:                // 1 for aincient power
		printw( "A place of ancient power..");
		player.ps+=2+int(player.fl*player.pl);
		player.cs+=(int(1+player.fl*player.cl));
		player.c+=25;
		break;
	}
	combat_get(1);
}

// A wandering monster has been found, so choose a monster!
void
monster_find(void)
{
	clear_message();
	mvprintw(24,45,"Beware!, Thou hast encountered");
	mvprintw(26,50,"An Evil ");
	foe.cs=0;
	foe.ps=0;
	player.cf=1;
	mf1:
	int rf=vRandom(land_mon_count);
	if ((rf > land_mon_nasty) && ( vRandom(100)<85)) goto mf1;

	// in water monster...
	if (pk==cWater) rf=water_mon_start+vRandom(water_mon_count);
	if (strchr(fs,creatures[rf].name[0])==NULL) goto mf1;

	// physical strength calculations
	if (creatures[rf].phy!=0) {
		foe.cs=int(((float)player.cs*0.3)+(float)(creatures[rf].phy)*(pow(player.fl,0.2))/((float)(vRandom(100)/100)+1));
	}

	// psi calculation for monster
	if (creatures[rf].mag!=0) {
		foe.ps=(int((float)creatures[rf].mag)*(pow(player.fl,0.2))/(((float)vRandom(100)/100)+1));
	}
	foe.u=int((rf*2)*(pow(player.fl,1.5)));            // experience award,
	printw("%s",&(creatures[rf].name[1]));
	strcpy(foe.name,&(creatures[rf].name[1]));
	combat_get(2);
	update_char();
	combat();                                // then begin the combat
}

// When in buildings, player might find jewel/amulet/helm/amulet stone/junk!
void
special_find(void)
{
	gotoxy(45,30);
	printw("You find an object...");
	combat_get(1);
	valley_data[player.s][player.w]=cBlank;        // remove the object once discovered,
	pk=cBlank;
	int rf=vRandom(100);
	if (player.s==6 && rf >95 && player.t[1]==6 && player.t[2]==0 && player.rt>25)
	{
		gotoxy(45,32);
		printw("The Helm of Evanna!");
		player.t[2]=1;
		player.ts=player.ts+100*(player.t[0]+player.t[1]+player.t[2]+player.fl);
		combat_get(2);
		return;
	}

	if (player.s==5 && rf >80 && player.t[0]==0)
	{
		player.t[0]=1;
		gotoxy(45,32);
		printw("Alarian's amulet... Empty!");
		combat_get(2);
		return;
	}

	if (player.s==4 && rf>70 && player.t[0]==1 && player.t[1]<6 && player.fl>player.t[1])
	{
		gotoxy(45,32);
		printw("An amulet stone..");
		combat_get(1);
		if (rf>85)
		{
			printw("doesn't fit!");
			combat_get(1);
			return;
		}
		printw("it fits!");
		player.t[1]++;
		return;
	}
	gotoxy(45,32);
	printw("a worthless bauble!");
	combat_get(1);
}
