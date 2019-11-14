#include <stdio.h>
#include <string.h>

#include "prog3.h"
#include "node0.h"

#define NODE_ID 0

int connectcosts0[4] = { 0, 1, 3, 7 };

struct distance_table dt0;


/* students to write the following two routines, and maybe some others */
void rtinit0() {

    // Steps to take:
    // - Update distance table
    // - Send distance table to all directly connected neighbours
    //
    //

    // Initialize distance table
    for (int i = 0; i < 4; i++) {
        dt0.costs[i][i] = connectcosts0[i];
    }

    struct rtpkt cost_pkt;
    cost_pkt.sourceid = NODE_ID;
    cost_pkt.destid = 0;
    memcpy(&cost_pkt.mincost, connectcosts0, 4 * sizeof(int));

    struct rtpkt *cost_pkt_ptr = &cost_pkt;

    // ignore self (link cost 0) and non directly connected nodes (999)
    // send direct neighbours our link costs
    for (int i = 0; i < 4; i++) {
        if (connectcosts0[i] != 0 && connectcosts0[i] != 999) {
            cost_pkt_ptr->destid = i;
            printf("cost_pkt.sourceid: %i\n", cost_pkt.sourceid);
            printf("cost_pkt.destid: %i\n", cost_pkt.destid);
            tolayer2(cost_pkt);
        }
    }

    printdt0(&dt0);
}


void rtupdate0(struct rtpkt *rcvdpkt) {
    printf("rtupdate0 srcid: %i\n", rcvdpkt->sourceid);
    printf("rtupdate0 destid: %i\n", rcvdpkt->destid);

    for (int i = 0; i < 4; i++) {
        printf("rtupdate0 mincost[%i]: %i\n", i, rcvdpkt->mincost[i]);
    }

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
