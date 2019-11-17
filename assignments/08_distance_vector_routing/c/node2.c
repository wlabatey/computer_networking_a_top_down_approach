#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "prog3.h"
#include "node2.h"

#define NODE_ID 2


// Link costs to directly connected neighbours used for initializing the distance table.
int connectcosts2[4] = { 3, 1, 0, 2 };

// Minimum costs to all nodes, initially set to same as connectcosts
int mincosts2[4] = { 3, 1, 0, 2 };

struct distance_table dt2;


void sendcosts2() {
    struct rtpkt cost_pkt;
    cost_pkt.sourceid = NODE_ID;
    cost_pkt.destid = 0;
    memcpy(&cost_pkt.mincost, mincosts2, 4 * sizeof(int));

    struct rtpkt *cost_pkt_ptr = &cost_pkt;

    for (int i = 0; i < 4; i++) {
        // ignore self and non directly connected nodes (999)
        if (i == NODE_ID || connectcosts2[i] == 999) {
            continue;
        }

        cost_pkt_ptr->destid = i;
        tolayer2(cost_pkt);
    }
}

/* students to write the following two routines, and maybe some others */
void rtinit2() {
    // Initialize distance table
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (i == j) {
                dt2.costs[i][j] = connectcosts2[i];
            }
            else {
                dt2.costs[i][j] = 0;
            }
        }
    }

    sendcosts2();
    printdt2(&dt2);
}


void rtupdate2(struct rtpkt *rcvdpkt) {
    printf("----------\n");
    printf("rtupdate2 srcid: %i\n", rcvdpkt->sourceid);
    printf("rtupdate2 destid: %i\n", rcvdpkt->destid);
    printf("rtupdate2 mincosts: ");
    for (int i = 0; i < 4; i++) {
        printf("%i ", rcvdpkt->mincost[i]);
    }
    printf("\n");

    // Steps to take:
    // - Check srcid (use as index for 2d distance table)
    // - Iterate through mincosts array (index is destination node)
    // - If connectcosts2[sourceid] + rcvdpkt->mincosts[i] < connectcosts2[sourceid] + dt2.costs[sourceid][i] then update dt2.costs[sourceid][i]
    // - If update made in last step, then resend dt2 to directly connected nodes

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

        printf("dt2.costs[i][srcid]: %i\n", dt2.costs[i][rcvdpkt->sourceid]);
        printf("connectcosts2[srcid]: %i\n", connectcosts2[rcvdpkt->sourceid]);
        printf("connectcost2[srcid] + rcvdpkt->mincost[i]: %i\n", connectcosts2[rcvdpkt->sourceid] + rcvdpkt->mincost[i]);
        if (dt2.costs[i][rcvdpkt->sourceid] == 0 || (connectcosts2[rcvdpkt->sourceid] + rcvdpkt->mincost[i]) < dt2.costs[i][rcvdpkt->sourceid]) {
            dt2.costs[i][rcvdpkt->sourceid] = connectcosts2[rcvdpkt->sourceid] + rcvdpkt->mincost[i];
            table_updated = true;
        }
    }

    printf("----------\n");

    if (table_updated == true) {
        printf("dt2 was updated. New table below.\n\n");
        printdt2(&dt2);

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
                if (dt2.costs[i][j] == 0) {
                    continue;
                }

                // Update mincost[i] with lowest cost for node i from each of the options of dt2.costs[i][j]
                if (dt2.costs[i][j] < mincosts2[i]) {
                    mincosts2[i] = dt2.costs[i][j];
                }
            }
        }

        printf("dt2update mincosts: ");
        for (int i = 0; i < 4; i++) {
            printf("%i ", mincosts2[i]);
        }
        printf("\n");

        printf("dt2update sending out new min costs.\n");
        sendcosts2();
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
