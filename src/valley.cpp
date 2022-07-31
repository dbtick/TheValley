/*
  The Valley......

  Based on the game from computing today,
  originaly ported for pet's , trs-80's and mz80k's,

  Ported to Gnu C++ By Dave T. June 2011.

 */

#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include "header.h"
#include <getopt.h>

char *valley_data[10];        // store for the valley info.

int debug=false;
int pk=0;
int q=0,q1=0;

char fs[10];					// foe selector

Player player;
Foe foe;

void save(void);
void load(void);
void end_prog(void);

// the main function, starts program here, and interprets command line options
// and then allows the user to choose a character+name.
int
main(int argc, char **argv)
{
	player.name[0]='\0';
	atexit(end_prog);

	char *a=(char *)malloc(sizeof(char)*16000);// reserve memory for valley data
	memset(a,cBlank,16000);            // clear valley data memory
	for (int aa=0;aa<10;aa++)        // seperate into 10 scenes
		valley_data[aa]=&a[1600*aa];

	int pt=0;            // for holding player type if option

	strcpy(fs,"VAEGH");    // set up initial area data + monsters.

	srand(time(0));
	vScrInit();

	opterr=false;
	mvprintw(17,35,"The Valley");            // set up a pretty title

	mvprintw(18,35,"----------");
	mvprintw(20,25,"Ported to gcc by Dave T. 2011\n");

	if (argc != 1)        // if there are any command line options,
	{
		int opt;                // parse them
		// end of options
		while ( (opt=getopt(argc,argv,"dn:g:c:")) !=EOF )
		{
			switch(opt)    // otherwise switch on command
			{
			case 'n':    // player name option,
				mvprintw(22,35,"Hello %s.",optarg);
				strcpy(player.name,optarg);// get name of player
				break;

			case 'g':// god option,
				if (strcmp(optarg,"iddqd")!=0) exit(-1);
				mvprintw(24,26,"God Powers given to player");
				player.god=true;
				break;

			case 'd':// debugging mode,
				mvprintw(26,27,"Debugging mode initiated.");
				debug=true;
				break;

			case 'c':// player type option (1-5)
				pt=(int)optarg[0];
				break;

			case '?':// error on command line
				mvprintw(47,0,"Invalid option '%c' for Valley,\nTry one of n,g,c or d\n\nPress any key to quit.", opt);
				combat_get(0);
				exit (-1);
			}
		}
	}

	FILE *fp;
	if ((fp=fopen("valley.sav","r"))!=NULL)
	{
		fclose(fp);
		//        load();
	} else {

		if (player.name[0]=='\0')
		{    // get players name if not on command line
			mvprintw(28,33,"Enter Name: ");
			char buf[40];                // buffer to store it,
			getnstr(buf,20);    // get it,
			strcpy(player.name,buf);    // and put it into playername
		}                    // then get player type.

		if (pt==0)            // if player type on command line,
		{                        // dont ask.
			mvprintw(30,2,"What are you ? (1:Wizzard, 2:Thinker, 3:Barbarian, 4:Warrior or 5:Cleric)?");
			pt = combat_get(0);
		}

		switch(pt)
		{                // switch on player type for variables
		case '1':
			strcpy(player.type,"Wizzard"); player.pl=2; player.cl=0.5; player.cs=22; player.ps=28; break;

		case '2':
			strcpy(player.type,"Thinker"); player.pl=1.5; player.cl=0.75; player.cs=24; player.ps=26; break;

		case '3':
			strcpy(player.type,"Barbarian"); player.pl=0.5; player.cl=2; player.cs=28; player.ps=22; break;

		case '4':
			strcpy(player.type,"Warrior"); player.pl=1;player.cl=1.25;player.cs=26; player.ps=24;break;

		case '5':
			strcpy(player.type,"Cleric"); player.pl=1.25;player.cl=1;player.cs=25; player.ps=25;break;

		default:
			strcpy(player.type,"Dolt"); player.pl=1;player.cl=1;player.cs=20; player.ps=20;break;
		}
//		player.ex=5;player.c=150;        // set up initial stamina & experience

		// then wish the player good luck, and wait for any key.

		mvprintw(32,27,"Good luck %s the %s." ,player.name,player.type);

		mvprintw(34,32,"Press Any key.");
		if (!debug) combat_get(0);            // don't wait if debugging!
	}
	// then begin the adventure...

	play();
	// if ended, say bye & leave
	clrscr();
	refresh();
	mvprintw(1,45,"Bye for now...\n");
	combat_get(5);
	return 0;
}

// the initial function play is called to begin the game.
void
play(void)
{
	draw_valley();         // draw the valley area,
	update_char();    	   // update the screen information on the character.
	movement();            // do the movement...
}

// this function puts up the players stats, and those of the monster if the
// player is in mortal combat!
void
update_char(void)
{
	if (player.cs>77-int(2*pow(player.pl,2.5)))        // limit strength
		player.cs=77-int(2*pow(player.pl,2.5));

	if (player.ps < 7)
		player.ps=7;                    // don't allow psi < 7

	if (player.ps > int(42*pow((player.pl+1),log(pow(player.pl,3.7)))+75))        // limit psi,
		player.ps = int(42*pow((player.pl+1),log(pow(player.pl,3.7)))+75);

	if (player.c>125+player.rt*5-(int(player.pl)*12.5)) player.c=125+player.rt*5-int(int(player.pl)*12.5);        // limit stamina
	immedok(mainscr,false);
	mvprintw(3,50, "%s the %s    ", player.name, player.type);
	mvprintw(6,50, "Treasure    = %.0f    ",player.ts);
	mvprintw(8,50, "Experience  = %.0f    ",player.ex);
	mvprintw(10,50,"Turns       = %d      ", player.tn);
	mvprintw(12,50,"Combat Str. = %.0f    ",player.cs);
	mvprintw(14,50,"Psi Power   = %.0f    ",player.ps);
	mvprintw(16,50,"Stamina     = %.0f    ",player.c);

	mvprintw(18,44,"----------------------------------");

	// calculate the players rating (ongoing, as it effects stamina!)
	player.rt=(0.067*(pow((player.ex+player.ts/3.0),0.5)+log((float)player.ex/pow(player.tn+1.0,1.5))));
	if (player.rt>28) player.rt=28;
	if (player.rt<0) player.rt=0;

	mvaddch(player.m/40+3,player.m%40+2,cPlayer);

	clear_message();        // clear junk off screen
	immedok(mainscr,true);
	refresh();

	if (player.cf==false) return;         // if not fighting, return,

	mvprintw(20,45,"Fighting %s",foe.name);
	mvprintw(22,45,"Strength = %d psi: %d",(int)foe.cs,(int)foe.ps);
}

// clears screen below character description.
void
clear_message(void)
{
	move(20,45); clrtoeol();
	move(22,45); clrtoeol();
	move(24,45); clrtoeol();
	move(26,45); clrtoeol();
	move(28,45); clrtoeol();
	move(30,45); clrtoeol();
	move(32,45); clrtoeol();

}


// Death!............................
void
death(void)
{

	clear_message();

	mvprintw(24,45,"You have been killed!");
	combat_get(2);

	mvprintw(26,45,"Thy spirit has been taken,");
	combat_get(2);

	mvprintw(28,45,"To be guarded until reincarnated");
	combat_get(2);

	mvprintw(30,45,"When a new character may enter");
	combat_get(2);

	mvprintw(32,45,".......");
	combat_get(2);

	printw("The valley........");
	combat_get(5);

	// if ended, say bye & leave
	mvprintw(45,1,"Bye for now...\n");
	exit(0);
}

// player lands on safe castle
void
leave_valley(void)
{
	int k;
	clear_message();
	mvprintw(24,45,"Thou art safe in castle,");
	mvprintw(26,45,"Will thou leave the valley?(Y/N)?");
	k=combat_get(0);
	if (k=='Y' || k=='y')
	{
		mvprintw(28,45,"May the gods be your guard!");
		combat_get(2);
		save();
		player.c=0;player.cs=0;
	}
	else
	{
		mvprintw(28,45,"Thy wounds are healed,");
		mvprintw(30,45,"Thy sword be sharp,");
		mvprintw(32,45,"leave, do what must be done.");
		combat_get(2);
		player.c+=200;
	}
	return;
}
#if 0

// displays the program constants.
void
debug_info(void)
{
	char title[7][30]={"","The Valley","Swamplands","Woodlands","Black Tower",\
			"Temple of Y'Nagioth","Vounims Lair"};
	int a;

	static int save_screen[4000];
	gettext(1,1,80,50,save_screen);

	while (1) {
		clrscr();
		cout << "         debugging info - variable values:\n";
		cout << "         ---------------------------------\n";
		cout << "\nScenario         \t" << s << "(" << title[s] << ")\nFL (floor)       \t" << fl << "\n";
		cout << "\nCombat str       \t" << cs << "\tPsi        " << ps << "\n";
		cout << "\nCs factor        \t" << cl << "\tPsi factor " << pl << "\n";
		cout << "\nTreasure         \t" << ts << "\tExperience " << ex << "\n";
		cout << "\nExperience Factor\t" << e << "\n\n";

		cout << "\nPosition present \t\t" << m << "\t character under '" << (char)pk << "'\n";
		cout << "\nPos outside primary scene \t" << mp << "\t Last character under '" << (char)q << "'\n";
		cout << "\nPos outside secondary scene \t" << mw << "\n";

		if (combat_flag) cout << "\nCombat ENABLED\n";
		else cout << "\nCombat DISABLED\n";

		cout << "\n\n 'c' toggle combat enable/disable(stops wandering monsters)\n";
		cout << "\n 'f' to increace floor number\n";
		cout << "\n 'r' return to game\n";

		a=combat_get(0);
		switch (a)
		{
		case 'c':
			combat_flag=(!combat_flag);
			break;
		case 'r':
			puttext(1,1,80,50,save_screen);
			return;
		case 'f':
			fl++;
			break;
		}

	}

}
#endif
void
save(void)
{
	FILE *fh=fopen("valley.sav","w");

	fprintf(fh,"%s,%s,%f,%f,%d,%f,%f,%d,%d,%d,%f,%f,%f,%d",player.name,\
			player.type,player.ts,player.ex,player.tn,player.cs,player.ps,\
			player.t[0],player.t[1],player.t[2],player.cl,player.pl,player.e,player.rt);

	fclose(fh);
}

void
load(void)
{
	FILE *fh=fopen("valley.sav","r");
	int rv=fscanf(fh,"%s,%s,%f,%f,%d,%f,%f,%d,%d,%d,%f,%f,%f,%d",&(player.name[0]), \
			&(player.type[0]),&player.ts,&player.ex,&player.tn,&player.cs,&player.ps,\
			&player.t[0],&player.t[1],&player.t[2],&player.cl,&player.pl,&player.e,&player.rt);
        if (rv == EOF) {
            printf("Failed to load from savefile. ABORTING.");
            exit(-1);
        }

        
	fclose(fh);
	player.c=150;
}

void
end_prog(void)
{
	vScrTerm();
}






