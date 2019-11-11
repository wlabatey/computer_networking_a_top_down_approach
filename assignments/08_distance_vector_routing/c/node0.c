#include <stdio.h>

#include "prog3.h"
#include "node0.h"


int connectcosts0[4] = { 0, 1, 3, 7 };

struct distance_table dt0;


/* students to write the following two routines, and maybe some others */
void rtinit0() {

    // Steps to take:
    // - Update distance table
    // - Send distance table to all directly connected neighbours
    for (int i = 0; i < 4; i++) {
        dt0.costs[i][i] = connectcosts0[i];
    }

    printdt0(&dt0);
}


void rtupdate0(struct rtpkt *rcvdpkt) {
    printf("rtupdate0 srcid: %i\n", rcvdpkt->sourceid);
    printf("rtupdate0 destid: %i\n", rcvdpkt->destid);
    printf("rtupdate0 mincost[0]: %i\n", rcvdpkt->mincost[0]);

    // Steps to take:
    // - Update distance table based on distance vector information from neighbours
    // - If distance table is updated, send new distance table again to neighbours
}


void printdt0(struct distance_table *dtptr) {
    printf("------------------------\n");
    printf("                via     \n");
    printf("   D0 |    1     2    3 \n");
    printf("------|-----------------\n");
    printf("     1|  %3d   %3d   %3d\n", dtptr->costs[1][1], dtptr->costs[1][2], dtptr->costs[1][3]);
    printf("dest 2|  %3d   %3d   %3d\n", dtptr->costs[2][1], dtptr->costs[2][2], dtptr->costs[2][3]);
    printf("     3|  %3d   %3d   %3d\n", dtptr->costs[3][1], dtptr->costs[3][2], dtptr->costs[3][3]);
    printf("------------------------\n");
}


/* called when cost from 0 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
void linkhandler0(int linkid, int newcost) {
    printf("linkhandler0 linkid: %i\n", linkid);
    printf("linkhandler0 newcost: %i\n", newcost);
}
