#include <stdio.h>

#include "prog3.h"
#include "node2.h"


int connectcosts2[4] = { 3,  1,  0, 2 };

struct distance_table dt2;

/* students to write the following two routines, and maybe some others */

void rtinit2() {
    for (int i = 0; i < 4; i++) {
        dt2.costs[i][i] = connectcosts2[i];
    }

    printdt2(&dt2);
}


void rtupdate2(struct rtpkt *rcvdpkt) {
    printf("rtupdate2 srcid: %i\n", rcvdpkt->sourceid);
    printf("rtupdate2 destid: %i\n", rcvdpkt->destid);
    printf("rtupdate2 mincost[0]: %i\n", rcvdpkt->mincost[0]);
}


void printdt2(struct distance_table *dtptr) {
    printf("------------------------\n");
    printf("                via     \n");
    printf("   D2 |    0     1    3 \n");
    printf("------|-----------------\n");
    printf("     0|  %3d   %3d   %3d\n", dtptr->costs[0][0], dtptr->costs[0][1], dtptr->costs[0][3]);
    printf("dest 1|  %3d   %3d   %3d\n", dtptr->costs[1][0], dtptr->costs[1][1], dtptr->costs[1][3]);
    printf("     3|  %3d   %3d   %3d\n", dtptr->costs[3][0], dtptr->costs[3][1], dtptr->costs[3][3]);
    printf("------------------------\n");
}
