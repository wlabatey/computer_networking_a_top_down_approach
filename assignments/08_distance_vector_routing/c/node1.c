#include <stdio.h>

#include "prog3.h"
#include "node1.h"


int connectcosts1[4] = { 1,  0,  1, 999 };

struct distance_table dt1;


/* students to write the following two routines, and maybe some others */


void rtinit1() {
    printdt1(&dt1);
}


void rtupdate1(struct rtpkt *rcvdpkt) {
    printf("rtupdate1 srcid: %i\n", rcvdpkt->sourceid);
    printf("rtupdate1 destid: %i\n", rcvdpkt->destid);
    printf("rtupdate1 mincost[0]: %i\n", rcvdpkt->mincost[0]);
}


void printdt1(struct distance_table *dtptr) {
    printf("------------------------\n");
    printf("             via        \n");
    printf("   D1 |    0     2      \n");
    printf("  ----|-----------------\n");
    printf("     0|  %3d   %3d      \n", dtptr->costs[0][0], dtptr->costs[0][2]);
    printf("dest 2|  %3d   %3d      \n", dtptr->costs[2][0], dtptr->costs[2][2]);
    printf("     3|  %3d   %3d      \n", dtptr->costs[3][0], dtptr->costs[3][2]);
    printf("------------------------\n");

}


/* called when cost from 1 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
void linkhandler1(int linkid, int newcost) {
    printf("linkhandler1 linkid: %i\n", linkid);
    printf("linkhandler1 newcost: %i\n", newcost);
}
