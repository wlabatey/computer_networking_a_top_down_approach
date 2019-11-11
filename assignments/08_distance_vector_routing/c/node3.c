#include <stdio.h>

#include "prog3.h"
#include "node3.h"


int connectcosts3[4] = { 7,  999,  2, 0 };

struct distance_table dt3;


/* students to write the following two routines, and maybe some others */

void rtinit3() {
    for (int i = 0; i < 4; i++) {
        dt3.costs[i][i] = connectcosts3[i];
    }
    printdt3(&dt3);
}


void rtupdate3(struct rtpkt *rcvdpkt) {
    printf("rtupdate3 srcid: %i\n", rcvdpkt->sourceid);
    printf("rtupdate3 destid: %i\n", rcvdpkt->destid);
    printf("rtupdate3 mincost[0]: %i\n", rcvdpkt->mincost[0]);
}


void printdt3(struct distance_table *dtptr) {
    printf("------------------------\n");
    printf("             via        \n");
    printf("   D3 |    0     2      \n");
    printf("------|-----------------\n");
    printf("     0|  %3d   %3d      \n", dtptr->costs[0][0], dtptr->costs[0][2]);
    printf("dest 1|  %3d   %3d      \n", dtptr->costs[1][0], dtptr->costs[1][2]);
    printf("     2|  %3d   %3d      \n", dtptr->costs[2][0], dtptr->costs[2][2]);
    printf("------------------------\n");
}
