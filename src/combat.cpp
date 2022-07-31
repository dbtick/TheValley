// combat routines - character & monster.

/*
  The Valley......

  Based on the game from computing today,
  originaly ported for pet's , trs-80's and mz80k's,

  Ported to Gnu C, By Dave T. 2011.

 */
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"
#include "header.h"

int
combat_get(int a)
{
	int c;
	if (a != 0) {
		halfdelay((a*10));
		c=getch();
		cbreak();
		if (c==ERR) return 0; else return c;
	} else {
		return getch();
	}
}

void
combat(void)
{
	foe.d1=0;                        // surprise attack/retreat store,
	gotoxy(45,24);
	player.e=39*log(player.ex)/3.14;     // calculate experience factor

	if (vRandom(100)>60)         // 40% chance of surprising monster,
	{
		printw("You have surprise! You may");
		gotoxy(45,26);
		printw("<a>ttack or <r>etreat?");
		foe.d1=combat_get(3);
		if (foe.d1=='r')                        // if player surprises & bottles!
		{
			gotoxy(45,28);
			printw("Cowardice from a %s!.",player.type);
			combat_get(2);
			player.cf=0;foe.hf=0;
			return;
		}
		if (foe.d1==0)                        // if player took too long to decide,
		{                                // monster gets 1st hit!
			gotoxy(45,28);
			printw("**** too slow... too slow ****");
			combat_get(1);
		}
	}

	if (foe.cs==0) foe.mt=1; else foe.mt=0;        // 1 = psi monster else 0 = physical

	while (player.cf==1)                    // loop until combat complete (cf=0)
	{
		update_char();
		if (foe.d1==0)                    // if monsters surprise
		{
			monster_hit();            // do the monsters attack
			update_char();
			if (player.cf==0) break;            // if monster exhausted itself,
		}
		foe.d1=0;
		character_hit();            // let the player fight back!
	}
	update_char();                // update stats.
}

// then the routine for the monsters attack,
void
monster_hit(void)
{
	gotoxy(45,26);
	printw("The creature attacks...");
	combat_get(1);

	// if its a psi monster, or both and is physically suffering, spell attack
	if (foe.cs==0 || (foe.cs < foe.ps && foe.ps > 6 && vRandom(2)==0)) monster_spell();

	else monster_physical();    // otherwise it will physicaly attack.
}

// the monsters magikal attack.
void
monster_spell(void)
{
	gotoxy(45,28);
	printw("..Hurling a lightning bolt at you");
	combat_get(1);

	// damage calculation,
	int d=(int)(((180*foe.ps*vRandom(100))/100-((player.ps+player.e)/100))/100);
	foe.ps-=5;                        // spelling costs psi energy
	if (d>9) foe.ps-=int(d/5);        // if V.powerful, costs more

	if (foe.cs+foe.ps<=0)                // does it knacker itself?
	{
		gotoxy(45,30);
		printw("Using its last energy...");
		player.cf=0;                                // end combat flag,
		combat_get(1);
		player.ex+=int((foe.u+1)/2);                   // only 1/2 experience for this
		return;                                // end combat
	}
	combat_get(2);
	if (d<=0)                // if no damage done, blame it on psi shield!
	{
		d=0;
		gotoxy(45,30);
		printw("Your psi shield protects you.");
		combat_get(2);
		return;
	}
	if (vRandom(100)<25)                // 25% chance of missing!
	{
		gotoxy(45,30);
		printw("...Missed you! (phew!)");
		combat_get(2);
		return;
	}
	player.c-=d;

	if (d>9) player.ps=int(float(player.ps)-d/5);        // large damage also effects psi!

	gotoxy(45,30);
	printw("You take %d damage!",d);
	combat_get(2);
	if (player.cs<=0 || player.c<=0) death();            // if dead, tuff!
	return;
}

// then the monsters physical attack routine,
void
monster_physical(void)
{
	int mta;                   // monster target area,
	gotoxy(45,28);
	foe.cs--;
	if (foe.cs<=0)                // does it knacker itself?
	{
		printw("Using its last energy...");
		player.cf=0;
		player.ex+=int((foe.u+1)/2);        // only 1/2 experience award,
		combat_get(1);
		return;
	}

	int z=0;                        // 0 if not a hit,otherwise damage multiplier
	mta=vRandom(10);                 // vRandom result of attack
	switch(mta)
	{
	case 0:                                        // miss!
		printw("It swings... and misses");
		break;
	case 1:                                     // players luck!
		printw("Your Blade deflects the blow");
		break;
	case 2:                                        // bottling monster
		printw("...but hesitates, unsure...");
		break;
	case 3:
		z=3;                                    // major hit!
		printw("It strikes your head!!!");
		break;
	case 4:
	case 5:                                        // average hit,
		z=1.5;
		printw("a glancing blow to the chest");
		break;
	case 6:
	case 7:                                        // average - low hit
		z=1;
		printw("A strike to your swordarm,");
		break;
	case 8:                                        // average hit
		z=1.3;
		printw("A blow to your body...");
		break;
	case 9:
		z=0.5;                                    // crappy hit
		printw("It catches your legs..");
		break;
	}
	if (z==0)                                        // if no damage,
	{                                                // wait then return
		combat_get(1);
		return;
	}

	// calculate the damage done,
	int d=(int)(((foe.cs*75*vRandom(100))/100 -(10*player.cs)-player.e)/100)*z;

	if (d<=0)                                        // if none,
	{
		d=0;
		gotoxy(45,30);
		printw("Shaken... but no damage!");
		combat_get(1);
		return;                                        // and return
	}

	player.c-=int(d);                                // else take off damage
	if (d>9) player.cs-=int(d/5);              // if hard hit, also effects strength

	gotoxy(45,30);                            // then tell player how bad
	printw("doing %d damage!.",d);
	combat_get(2);
	if (player.cs<=0 || player.c<=0) death();                // if dead, har har!

}

// Then I suppose we should have the players attack (poor monster!)
void
character_hit(void)
{
	int k;                                    // holds key hit,
	gotoxy(45,26);
	printw("*** Strike Quickly... *** <h>ead, <b>ody or <l>egs");    // tell them to hurruy up
	k=combat_get(2);                        // get a hit (or a miss!)
	if (k==0)
	{                                       // took to long,
		gotoxy(45,28);                        // monsters go!
		printw("**** too slow... too slow ****");
		combat_get(1);
		foe.hf=0;
		return;
	}

	if (k=='s')                            // if spell casting
	{
		character_spell();                // do it...
		return;
	}
	if (foe.cs==0 && foe.mt==1)                    // if strength 0, & psi only,
	{
		gotoxy(45,28);                    // no point in using sword!
		printw("  Your sword is of no use");
		gotoxy(45,30);
		printw("Against a foe such as this!.");
		combat_get(2);
		foe.d1=0;
		return;
	}
	player.c--;                // decreace stamina, its hard work fighting, try it!

	if (player.c<0)                // if exhausted, die!!!!
	{
		gotoxy(45,28);
		printw("You fataly exhaust yourself!");
		death();                                // oh dear....
	}

	int rf=vRandom(10);                // vRandomness chance of hitting...

	int z=0;						// z = damage factor
	if (k=='h' &&(rf<5 || player.cs>foe.cs*4))         // combined with head/body/limbs
	{                                        // gives a chance of to hit...
		z=2;                                // major hit (z=damage factor)
	}
	else if (k=='b' &&(rf<7 || player.cs>foe.cs*4))
	{
		z=1;                                // average hit,
	}
	else if (k=='l' && (rf<9 || player.cs>foe.cs*4))
	{
		z=0.3;                                // sewing needle attack!
	}
	else
	{
		gotoxy(45,28);                        // otherwise a miss!
		printw("You missed it!");            // tell them (gloat !!!!)
		combat_get(1);
		foe.d1=0;
		foe.hf=0;
		return;
	}

	int d=0;
	if (foe.hf==1)                                // if stunned,
	{                                       // hit will kill it,
		d=foe.cs+vRandom(9);
		foe.hf=0;                                // damage and reset stunned flag,
	}
	else                                    // otherwise calculate damage
	{
		d=(int)(((((float)player.cs*50*((float)vRandom(100)/100))-(float)(10*foe.cs)+player.e)/100)*z);
		if (d<0) d=0;
		if (player.cs>(foe.cs-d)*4) foe.hf=1;                // was it a stunner of a hit?
	}

	foe.cs-=d;                                // deduct the damage...
	gotoxy(45,28);
	printw("You hit it!...");                // and tell player a hit!

	combat_get(1);
	if (d==0)                                // if it didn't hurt it,
	{
		printw("but no damage!");            // tell user,
		foe.hf=0;
		combat_get(1);
		return;
	}
	printw("%d damage!",d);

	if (foe.cs<=0 && foe.mt == 0)                    // if the monster is dead!,
	{
		gotoxy(50,30);
		printw("...Killing the monster!!!");
		foe.hf=0;
		player.cf=0;                               // end the fight and
		player.ex=player.ex+foe.u;                            // give experience,

		combat_get(2);
		update_char();                    // clear fight info off screen
		return;                             // and return to caller
	}

	if (foe.hf==1)                                // if it was a stunner of a hit
	{
		gotoxy(45,30);
		printw("The %s is stunned!",foe.name);    // say so,
		foe.d1=1;                                // and let player hit again!
		combat_get(1);
		update_char();
		return;
	}
	combat_get(1);
}

// then the characters spell option,
void
character_spell(void)
{
	int d=0;
	update_char();
	gotoxy(45,26);
	printw("Which Spell Seek Yea(1-3)?");             // ask which spell to cast,
	int s=combat_get(2);
	if (s==0)
	{
		gotoxy(45,28);                    // to indecisive, monster hits again
		printw("**** too slow... too slow ****");
		combat_get(1);
		foe.hf=0;
		foe.d1=0;
		return;
	}

	if (s<'1' || s > '3')                        // if non existent spell,
	{
		gotoxy(45,28);
		printw("There be no such spell!");        // say so
		combat_get(1);
		return;
	}
	switch (s)
	{
	case '1':                        // sleepit spell.
		player.c-=5;
		if (player.c<0)                    // does the player over do it?
		{                           // yes! dieeeee.....
			gotoxy(45,28);
			printw("The spell saps all your strength!");
			death();
		}
		gotoxy(45,28);
		printw("Sleep you miserable fiend so I may");
		gotoxy(45,30);
		printw("escape and fight another day......");

		combat_get(2);
		update_char();

		gotoxy(45,26);
		printw("The Creature staggers...");
		gotoxy(45,28);
		combat_get(2);

		if (vRandom(100)<50)            // 50% chance of success (quite good!)
		{
			printw("But recovers with a snarl...");
			combat_get(1);
			foe.d1=0;
			foe.hf=0;                    // if it recovers then fight on...
			update_char();
			return;
		}
		else                            // otherwise it falls asleep.
		{
			printw("and collapses stunned...");
			combat_get(2);
			player.ex+=foe.u/2;        // but only worth 1/2 experience
			player.cf=0;
			foe.hf=0;
			return;
		}

	case '2':                            // psi-lance...
		if (foe.ps==0)                      // only effects mosters with psi.
		{
			gotoxy(45,28);
			printw("This spell is of no use here...");
			combat_get(1);
			foe.d1=0;
			return;
		}

		// the player must be quite advanced to be able to do this spell
		if ((foe.cs > player.c || player.ps <49 || player.ex<1000) && player.god==false)
		{
			gotoxy(45,28);
			printw("This spell is beyond you...");
			combat_get(1);
			foe.d1=0;
			return;
		}

		player.c-=10;                    // otherwise take off spell energy,
		if (player.c<0)                // and kill player of overdone!
		{
			gotoxy(45,28);
			printw("The spell saps all your strength!");
			death();
		}

		gotoxy(45,28);                // if castable, cast it...
		printw("With my mind,");
		gotoxy(50,30);
		printw("I battle thee for my life...");
		combat_get(2);

		gotoxy(45,32);
		if (vRandom((int)(player.ps+foe.ps))>player.ps)        // if monsters psi check succeeds,
		{
			printw("The monsters Psi shields it!");
			combat_get(1);
			foe.d1=0;
			return;
		}
		// calculate the damage done,

		d=(int)(((((player.cs*50*(vRandom(100)))/100)-5*(foe.ps+foe.cs)+player.e/50)*2)/100);

		if (d<=0)        // if none, say so!
		{
			printw("The spell has no visible effect.");
			combat_get(1);
			foe.d1=0;
			return;
		}

		update_char();
		gotoxy(45,28);

		// otherwise say how much damage it caused.
		printw("The psi lance does %d damage",(int)(d*2));
		gotoxy(45,30);

		foe.ps-=2*d;                    // reduse monsters psi,
		if (foe.ps<0) foe.ps=0;                // make 0 if so,
		foe.cs-=d;                        // decrease strength
		if (foe.cs<0) foe.cs=0;                // if <0 =0
		if (foe.cs+foe.ps>0)                // if still alive, continue fight
		{
			combat_get(2);
			return;
		}

		gotoxy(45,32);                            // otherwise its dead!
		printw("... killing the creature.");
		combat_get(2);
		player.cf=0;
		foe.hf=0;                                    // end combat,
		player.ex+=foe.u;
		return;

	case '3':                                        // crispit...
		if ((player.ps <77 || player.ex<5000) && player.god==false)
		{
			gotoxy(45,28);                         // player needs power!
			printw("This spell is beyond you...");
			combat_get(2);
			foe.d1=0;
			return;
		}
		player.c-=20;                    // take of energy required,
		if (player.c<0)                // and if overdone, die!
		{
			gotoxy(45,28);
			printw("The spell saps all your strength!");
			death();
		}

		update_char();        // otherwise cast the fancy spell.
		gotoxy(45,26);
		printw("With my sword I smight thee,");
		gotoxy(45,28);
		printw("With this spell I curse thee");
		gotoxy(45,30);
		printw("Burn yea spawn of hell and suffer.");
		combat_get(3);

		update_char();
		gotoxy(45,28);
		printw("A bolt of energy lashes out...");

		combat_get(1);
		gotoxy(45,30);

		if (player.ps/player.pl < vRandom(50))                // chance of missing,
		{
			printw("but misses...");
			combat_get(2);
			foe.d1=0;
			return;
		}

		// calculate the damage caused - quite severe in some cases!
		d=(int)((player.cs+player.ps*((float)vRandom(100)/100)-((10*foe.ps*vRandom(100))/100)));

		if (d<0)                    // if no damage, say it dodged!
		{
			printw("But is dodged craftily...");
			combat_get(1);
			foe.d1=0;
			return;
		}

		if (foe.cs==0) foe.ps=foe.ps-d;           // if psi monster, take of psi not str
		else
		{
			foe.cs-=d;
			if (d>10) foe.ps-=(d/3);
		}
		// then inform player of damage caused.
		printw("It hits causing %d damage.",(int)d);

		if (foe.cs+foe.ps<=0)                // if it kills the creature,
		{
			gotoxy(45,32);
			printw("the beast dies screaming horribly!");
			player.cf=0;
			foe.hf=0;                    // end combat and return.
			foe.d1=0;
			player.ex+=foe.u;
		}
		combat_get(2);
		return;
	}
}
