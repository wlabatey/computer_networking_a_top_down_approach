#include <stdio.h>
#include <string.h>
#include <stdbool.h>

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
            tolayer2(cost_pkt);
        }
    }

    printdt0(&dt0);
}


void rtupdate0(struct rtpkt *rcvdpkt) {
    printf("----------\n");
    printf("rtupdate0 srcid: %i\n", rcvdpkt->sourceid);
    printf("rtupdate0 destid: %i\n", rcvdpkt->destid);
    printf("rtupdate0 mincosts: ");
    for (int i = 0; i < 4; i++) {
        printf("%i ", rcvdpkt->mincost[i]);
    }
    printf("\n");

    // Steps to take:
    // - Check srcid (use as index for 2d distance table)
    // - Iterate through mincosts array (index is destination node)
    // - If connectcosts0[sourceid] + rcvdpkt->mincosts[i] < connectcosts0[sourceid] + dt0.costs[sourceid][i] then update dt0.costs[sourceid][i]
    // - If update made in last step, then resend dt0 to directly connected nodes

    bool table_updated = false;

    printf("----------\n");

    for (int i = 0; i < 4; i++) {
        // Don't update the distance table with another node's link cost to itself (which will always be 0)
        if (i == rcvdpkt->sourceid) {
            printf("i %i matches sourceid %i. Skipping.\n", i, rcvdpkt->sourceid);
            continue;
        }

        // Don't update the distance table with another node's link cost back to the current node (which we already have from mincosts[])
        if (i == NODE_ID) {
            printf("i %i matches NODE_ID %i. Skipping.\n", i, NODE_ID);
            continue;
        }

        // Ignore link costs of 999, which mean a node has not yet established a path to another node.
        if (rcvdpkt->mincost[i] == 999) {
            printf("node %i has not yet established a path to node %i. Skipping.\n", rcvdpkt->sourceid, i);
            continue;
        }

        printf("dt0.costs[i][srcid]: %i\n", dt0.costs[i][rcvdpkt->sourceid]);
        printf("connectcosts0[srcid]: %i\n", connectcosts0[rcvdpkt->sourceid]);
        printf("connectcost0[srcid] + rcvdpkt->mincost[i]: %i\n", connectcosts0[rcvdpkt->sourceid] + rcvdpkt->mincost[i]);
        if (dt0.costs[i][rcvdpkt->sourceid] == 0 || (connectcosts0[rcvdpkt->sourceid] + rcvdpkt->mincost[i]) < dt0.costs[i][rcvdpkt->sourceid]) {
            dt0.costs[i][rcvdpkt->sourceid] = connectcosts0[rcvdpkt->sourceid] + rcvdpkt->mincost[i];
            table_updated = true;
        }
    }

    printf("----------\n");

    if (table_updated == true) {
        printf("dt0 was updated. New table below.\n\n");
        printdt0(&dt0);

        // TODO: Send out updated distance tables to directly connected nodes.
    }
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
