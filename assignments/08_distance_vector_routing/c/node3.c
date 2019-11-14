#include <stdio.h>
#include <string.h>

#include "prog3.h"
#include "node3.h"

#define NODE_ID 3

int connectcosts3[4] = { 7,  999,  2, 0 };

struct distance_table dt3;


/* students to write the following two routines, and maybe some others */


void rtinit3() {
    for (int i = 0; i < 4; i++) {
        dt3.costs[i][i] = connectcosts3[i];
    }

    struct rtpkt cost_pkt;
    cost_pkt.sourceid = NODE_ID;
    cost_pkt.destid = 0;
    memcpy(&cost_pkt.mincost, connectcosts3, 4 * sizeof(int));

    struct rtpkt *cost_pkt_ptr = &cost_pkt;

    // ignore self (link cost 0) and non directly connected nodes (999)
    // send direct neighbours our link costs
    for (int i = 0; i < 4; i++) {
        if (connectcosts3[i] != 0 && connectcosts3[i] != 999) {
            cost_pkt_ptr->destid = i;
            printf("cost_pkt.sourceid: %i\n", cost_pkt.sourceid);
            printf("cost_pkt.destid: %i\n", cost_pkt.destid);
            tolayer2(cost_pkt);
        }
    }

    printdt3(&dt3);
}


void rtupdate3(struct rtpkt *rcvdpkt) {
    printf("rtupdate3 srcid: %i\n", rcvdpkt->sourceid);
    printf("rtupdate3 destid: %i\n", rcvdpkt->destid);

    for (int i = 0; i < 4; i++) {
        printf("rtupdate3 mincost[%i]: %i\n", i, rcvdpkt->mincost[i]);
    }
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
