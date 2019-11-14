#include <stdio.h>
#include <string.h>

#include "prog3.h"
#include "node2.h"

#define NODE_ID 2

int connectcosts2[4] = { 3,  1,  0, 2 };

struct distance_table dt2;


/* students to write the following two routines, and maybe some others */


void rtinit2() {
    for (int i = 0; i < 4; i++) {
        dt2.costs[i][i] = connectcosts2[i];
    }

    struct rtpkt cost_pkt;
    cost_pkt.sourceid = NODE_ID;
    cost_pkt.destid = 0;
    memcpy(&cost_pkt.mincost, connectcosts2, 4 * sizeof(int));

    struct rtpkt *cost_pkt_ptr = &cost_pkt;

    // ignore self (link cost 0) and non directly connected nodes (999)
    // send direct neighbours our link costs
    for (int i = 0; i < 4; i++) {
        if (connectcosts2[i] != 0 && connectcosts2[i] != 999) {
            cost_pkt_ptr->destid = i;
            printf("cost_pkt.sourceid: %i\n", cost_pkt.sourceid);
            printf("cost_pkt.destid: %i\n", cost_pkt.destid);
            tolayer2(cost_pkt);
        }
    }

    printdt2(&dt2);
}

void rtupdate2(struct rtpkt *rcvdpkt) {
    printf("rtupdate2 srcid: %i\n", rcvdpkt->sourceid);
    printf("rtupdate2 destid: %i\n", rcvdpkt->destid);

    for (int i = 0; i < 4; i++) {
        printf("rtupdate2 mincost[%i]: %i\n", i, rcvdpkt->mincost[i]);
    }
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
