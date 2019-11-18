#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "prog3.h"
#include "node0.h"

#define NODE_ID 0


// Link costs to directly connected neighbours used for initializing the distance table.
int connectcosts0[4] = { 0, 1, 3, 7 };

// Minimum costs to all nodes, initially set to same as connectcosts
int mincosts0[4] = { 0, 1, 3, 7 };

struct distance_table dt0;

void findmincosts0() {
    // Update mincosts if distance table was updated.
    for (int i = 0; i < 4; i++) {
        // Ignore link cost to ourself
        if (i == NODE_ID) {
            continue;
        }

        for (int j = 0; j < 4; j++) {
            // Ignore routes to another node via ourself, which we already have from connectcosts[]
            if (j == NODE_ID) {
                continue;
            }

            // Ignore uninitialized link costs
            if (dt0.costs[i][j] == 0) {
                continue;
            }

            // Update mincost[i] with lowest cost for node i from each of the options of dt0.costs[i][j]
            if (dt0.costs[i][j] < mincosts0[i]) {
                mincosts0[i] = dt0.costs[i][j];
            }
        }
    }
}

void sendcosts0() {
    struct rtpkt cost_pkt;
    cost_pkt.sourceid = NODE_ID;
    cost_pkt.destid = 0;
    memcpy(&cost_pkt.mincost, mincosts0, 4 * sizeof(int));

    struct rtpkt *cost_pkt_ptr = &cost_pkt;

    for (int i = 0; i < 4; i++) {
        // ignore self and non directly connected nodes (999)
        if (i == NODE_ID || connectcosts0[i] == 999) {
            continue;
        }

        cost_pkt_ptr->destid = i;
        tolayer2(cost_pkt);
    }
}

/* students to write the following two routines, and maybe some others */
void rtinit0() {
    // Initialize distance table
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (i == j) {
                dt0.costs[i][j] = connectcosts0[i];
            }
            else {
                dt0.costs[i][j] = 0;
            }
        }
    }

    sendcosts0();
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

        findmincosts0();

        printf("dt0update mincosts0: ");
        for (int i = 0; i < 4; i++) {
            printf("%i ", mincosts0[i]);
        }
        printf("\n");

        printf("dt0update sending out new min costs.\n");
        sendcosts0();
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

    // Reset all costs via linkid to 0.
    // Update the distance table with new cost
    dt0.costs[linkid][linkid] = newcost;

    for (int i = 0; i < 4; i ++) {
        // Direct route already updated, so skip it here.
        if (i == linkid) {
            continue;
        }

        // Reset all link costs via linkid
        dt0.costs[i][linkid] = 0;
    }

    // Update connectcosts0 with new cost 
    connectcosts0[linkid] = newcost;

    // Reset mincosts to connectcosts and then find new lowest cost path
    for (int i = 0; i < 4; i++) {
        mincosts0[i] = connectcosts0[i];
    }

    findmincosts0();

    printf("linkhandler0 new link costs: ");
    for (int i = 0; i < 4; i++) {
        printf("%i ", connectcosts0[i]);
    }
    printf("\n");

    printf("linkhandler0 new min costs: ");
    for (int i = 0; i < 4; i++) {
        printf("%i ", mincosts0[i]);
    }
    printf("\n");

    printf("linkhandler0 dt0 was updated. New table below.\n\n");
    printdt0(&dt0);

    sendcosts0();
}
