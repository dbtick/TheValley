// movement section.

/*
  The Valley......

  Based on the game from computing today,
  originaly ported for pet's , trs-80's and mz80k's,

  Ported to Borland c++, By Dave T. June 1992.

 */

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "header.h"
#include "utils.h"

void show_stats();

const char *rat[]={"Pathetic!","Monster food","Peasant","Cadet","Cannon fodder",\
		"Path Walker","Novice Adventurer","Survivor","Adventurer",\
		"Assasin","Apprentice Hero","Giant Killer","Hero",\
		"Master of Sword","Champion","Necromancer","Loremaster",\
		"Paladin","Superhero","Dragon slayer","Knight of the valley",\
		"Master of Combat","Dominator","Prince of Valley","Guardian",\
		"War Lord","Demon Killer","Lord of Valley","Master of Destiny!"};


void prompt_player_for_move(void)
{
	if (pk =='/' || pk == '\\' || pk=='_')    // if on path,
	{
		mvprintw(30,46,"Safe on the Path,which way now? ");
	}
	else
	{
		mvprintw(30,46,"Which way now? "); // otherwise not so friendly
	}
}

void recover_some_health(void)
{
	player.c+=10;                // 10 per round recovery rate,
	if (player.rt>10) player.cs++;        // rating >10 ,cs +1 each round
}

void undo_recover_some_health(void)
{
	player.c-=10;                        // cancel stamina heal,
	if (player.rt>10) player.cs--;        // rating >10 ,cs +1 each round
}

// This waits for the players input, checks it is valid, and then moves the player.
int get_players_move(void)
{
	int i=0;
	// we want to save the cursor position,
	// so that when an invalid input is given, we don't move the cursor on.
	int xs=0,ys=0;

	recover_some_health();

	prompt_player_for_move();

	getyx(mainscr,ys,xs);

	while (true) {
		if (player.god) { player.c=9999; player.cs=9999; player.ex=99999; player.ps=9999; player.ts=99999;}
		gotoxy(xs,ys);
		i=combat_get(0);            // get the players command
		gotoxy(xs,ys);

		// check the input is valid
		char b[2];
		sprintf(b,"%c",i);
		if (strstr("d+s123456789",b)==NULL) continue;

		if (i=='d' && debug)        // debugging screen,
		{
			//        debug_info();
			continue;
		}
		if (i=='+' && debug)        // boost stamina, str and psi.
		{ player.ps+=10; player.cs+=10; player.c+=10; continue; }

		if (i=='s')                    // score players achievement,
		{ show_stats(); continue; }

		// then check to see if trying to move past the edge of the map,
		if ((i=='1' || i=='4' || i=='7')&& player.m%40==0) continue;
		if ((i=='3' || i=='6' || i=='9')&& player.m%40==39) continue;

		i=(int)i - 0x30;                    // calculate the position change,
		int t1=(i>6)?-40:(i>3)?0:+40;
		t1+=(((i-1)%3)-1);
		player.w=player.m+t1;                            // w = new position

		if (player.w>1599 || player.w<0 ) { player.w=player.m; continue; }        // if off map, disallow.

		player.tn++;                            // increace turn count
		q=q1;                            // what am I on...
		q1=valley_data[player.s][player.w];            // what am I to move to...

		if (q1==cWall || q1 == cTree)        // if hit wall or tree...
		{
			player.tn--;                        // cancel turn,
			undo_recover_some_health();
			continue;                    // return to get command
		}

		// otherwise, a valid move etc...
		return i;
	}
}

void
movement(void)
{
	int i,rf,safe;
	player.m=player.w;
	pk=valley_data[player.s][player.w];    // save the present object, and then draw the
	mvaddch(player.w/40+3,player.w%40+2,cPlayer);       // draw the character.

	/*******************************************************************/
	/*    the main command loop is here.....                           */

	while (1) {
		i=get_players_move();

		if (i==0x1b && debug) return ;    // quick <ESC> exit for debugging

		gotoxy(player.m%40+2,player.m/40+3);            // otherwise goto previous player location
		addch(((pk==0)?cBlank:pk));        // replace with old content
		pk=valley_data[player.s][player.w];            // then look at what the player has now stepped on

		safe=0;                         // assume all locations are unsafe,

		switch (pk) {
		case '/':                       // safe on the path,
		case '\\':
		case '_':
			safe=1;
			break;

		case cWoods:
		case cSwamp:
		case cCastle:
		case cTower:
			if (pk==cCastle) player.mw=player.m;        // lair/temple? save position entered from :mw
			else player.mp=player.m;                    // otherwise save position enterd from :mp
			new_scene();                  // generate the new scene...
			continue;                     // and continue with the command fetching

		case cExit:
			leave_valley();        // leave & save option etc...
			if (player.cs==0) return;
			continue;

		case cDoor:
		case cBorder:
			scene_exit();        // check to see if left too soon, else leave.
			update_char();       // restore screen
			continue;            // continue with commands

		case cWater:
			undo_recover_some_health();
			player.c-=10;  		// health penalty for being in water
			if (player.c<0) death();
			break;     //goto wmcheck;

		case cStairs: // if stairs, call routine to deal with
			stairs();
			continue;

		case cSpecial:
			special_find();
			continue;
		}

		player.m=player.w;                    // otherwise move to new pos,
		update_char();        // update screen.

		gotoxy(player.w%40+2,player.w/40+3);
		addch(cPlayer);                    // put characters shape,

		// if nothing, now check for wandering monsters, or a find.
		rf=vRandom(100);                // percentile random number

		if (!safe && rf<(20+2*player.fl))    // 25% base + 2*floor.
		{
			monster_find();                // if so, go find a monster
			continue;                    // if survived, continue.
		}

		else if (!safe && rf > 75) find_select();    // 25% chance of random find,
		//    else                                // otherwise very boring.
		//      {
		//        mvprintw(32,50,"Nothing of interest...");
		//      }
		//    combat_get(1);

		update_char();                    // update screen.
	}
}


// Dealing with stairs in the tower,
void
stairs(void)
{
	mvprintw(30,45,"Stairs! <U>p or <D>own?");
	int d=combat_get(0);
	switch (d)
	{
	case 'd':
		if (player.fl==2)
		{
			mvprintw(32,45,"These stairs are blocked!");
			combat_get(1);
			return;
		}
		player.fl--;
		player.m=player.w;
		new_scene();
		return;
	case 'u':
		if (player.fl==8)
		{
			mvprintw(32,45,"These stairs are blocked!");
			combat_get(1);
			return;
		}
		player.m=player.w;
		player.fl++;
		new_scene();
		return;
	default:
		return;
	}
}

void
show_stats(void)
{
	clear_message();
	mvprintw(26,45,"Player Stats:");
	mvprintw(28,45,"Rating : ");

	printw("%s",rat[player.rt]);
	gotoxy(45,30);
	if (player.t[2]==1)
	{
		printw("You have the Helm of Evanna");
		gotoxy(45,32);
	}
	if (player.t[0]==1)
	{
		printw("and an amulet with %d stones",player.t[1]);
	}
	combat_get(2);
	clear_message();
	return;
}


