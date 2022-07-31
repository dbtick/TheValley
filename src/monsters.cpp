// this contains the monster data for valley.

#include "header.h"

// then the monsters, 1st letter = location code,
// A = Valley, Woods, Swamps and Black tower,
// B = Vounims layer and Temple of Y'Nagioth,
// C = Black tower, Vounims layer and Temple of Y'Nagioth,
// E = All places,
// F = Woods and swamps,
// G = Valley and Black Tower,
// H = Valley and Woods.
// L = Watery areas, - lakes & swamps mainly.

// then the monster name, the physical strength , then magical strength.

const int number_of_monsters=19;
const int land_mon_count=17;
const int land_mon_nasty=9;
const int water_mon_start=17;
const int water_mon_count=2;

struct
monster creatures[]=
{
    {"Awolfen",            9,    0},
    {"Ahob-goblin",        8,    0},
    {"Aorc",            8,    0},
    {"Efire-imp",        7,    3},
    {"Grock-troll",        19,    0},
    {"Eharpy",            10,    12},
    {"Aogre",            23,    0},
    {"Bbarrow-wight",    0,    25},
    {"Hcentaur",        18,    14},
    {"Efire-giant",        26,    20},
    {"Vthunder-lizzard",50,    0},
    {"Cminatour",        35,    25},
    {"Cwraith",            0,    30},
    {"Fwyvern",            36,    12},
    {"Bdragon",            50,    20},
    {"Cring-wraith",    0,    45},
    {"Abalrog",            50,    50},
    {"Lwater-imp",        25,    15},
    {"Lkraken",            50,    0}
};

