/* Some utils for Valley on GCC 
 */
 /* Version history 
  * 0.1 First draft
  */


#include <locale.h>
#include "utils.h"

WINDOW *mainscr=(WINDOW *)0;

int vScrTerm()
{
  endwin();
  return 0;
}


int vScrInit()
{
  setlocale(LC_ALL,"");
  if (!mainscr) {
    mainscr=initscr();
    immedok(mainscr,TRUE);
  }
  return 0;
}
