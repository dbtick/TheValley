// header file for the valley

/*
		The Valley......

 Based on the game from computing today,
 originaly ported for pet's , trs-80's and mz80k's,

 Ported to Borland c++, By Dave T. June 1992.

 */

void play(void);
void draw_valley(void);
void design_valley(char *);
void design_swamp(char *);
void design_woods(char *);
void design_tower(char *);
void design_temple(char *);
void design_lair(char *);
void add_lake(char *);
void update_char(void);
int  rnd(int);
void clear_message(void);
void movement(void);
int combat_get(int);
void leave_valley(void);
void new_scene(void);
void scene_exit(void);
void stairs(void);
void death(void);
void special_find(void);
void monster_find(void);
void combat(void);
void find_select(void);
void monster_hit(void);
void character_hit(void);
void character_spell(void);
void debug_info(void);
void monster_spell(void);
void monster_physical(void);

extern char *valley_data[10];		// store for the valley info.

extern char fs[10];					// foe selector (for scenario)

extern int debug,cf;

extern int pk,q,q1,p[8],n[8];

extern const int number_of_monsters;
extern const int land_mon_count;
extern const int land_mon_nasty;
extern const int water_mon_start;
extern const int water_mon_count;

struct monster
{
	char name[20];
	int phy;
	int mag;
} ;

class Player
{
public:
	char name[80];	// name
	char type[20];	// player class
	float ts; 		// treasure
	float ex;		// experience
	int tn;			// turns
	float cs;		// combat strength
	float ps;		// PSI power
	float c; 		// Stamina
	float e;		// experience based multiplier for combat.
	float cl;		// combat strength recovery factor
	float pl; 		// psi recovery factor
	int rt;			// player rating
	int god;		// 0 = godmode off, 1=on
	int cf;			// combat flag = 0=not, 1=fighting.
	int s;			// scene: 1=valley,2=swamp,3=woods,4=tower,5=temple,6=lair,
	float fl;		// "floor" - higher = more difficult
	int mw;			// stores location when entering castle (i.e. woods/swamp position)
	int mp;			// stores location when enering woods/swamp/tower (i.e. valley position)
	int m;			// current location.
	int w;			// "next" location when moving etc.
	int tf;			// turn "save" to stop immediate exit of tower/castle.
	int t[3];		// treasure [amulet,#stonesInAmulet,helm]
	Player(void) : ts(0), ex(5), tn(0), cs(0), ps(0), c(150), e(0),
			cl(0), pl(0), rt(0), god(0), cf(0), s(1), fl(1), mw(0),
			mp(0), m(0), w(0) {t[0]=0;t[1]=0;t[2]=0;}
} ;
extern Player player;

class Foe
{
public:
	char name[80];			// name
	float u;				// experience award for killing
	float cs;				// combat strength
	float ps;				// psi strength
	int hf;					// 1=foe stunned, 0=not.
	int d1;					// 1=foe surprised, 0=not
	int mt;					// Monster type, 1=psi only, 0=physical only
	Foe(void) : u(0),cs(0),ps(0),hf(0),d1(0) {}
};
extern Foe foe;

extern struct monster creatures[];
