#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "prog3.h"
#include "node1.h"

#define NODE_ID 1


// Link costs to directly connected neighbours used for initializing the distance table.
int connectcosts1[4] = { 1, 0, 1, 999 };

// Minimum costs to all nodes, initially set to same as connectcosts
int mincosts1[4] = { 1, 0, 1, 999 };

struct distance_table dt1;


/* students to write the following two routines, and maybe some others */
void rtinit1() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (i == j) {
                dt1.costs[i][j] = connectcosts1[i];
            }
            else {
                dt1.costs[i][j] = 0;
            }
        }
    }

    struct rtpkt cost_pkt;
    cost_pkt.sourceid = NODE_ID;
    cost_pkt.destid = 0;
    memcpy(&cost_pkt.mincost, connectcosts1, 4 * sizeof(int));

    struct rtpkt *cost_pkt_ptr = &cost_pkt;

    // ignore self (link cost 0) and non directly connected nodes (999)
    // send direct neighbours our link costs
    for (int i = 0; i < 4; i++) {
        if (connectcosts1[i] != 0 && connectcosts1[i] != 999) {
            cost_pkt_ptr->destid = i;
            tolayer2(cost_pkt);
        }
    }

    printdt1(&dt1);
}


void rtupdate1(struct rtpkt *rcvdpkt) {
    printf("----------\n");
    printf("rtupdate1 srcid: %i\n", rcvdpkt->sourceid);
    printf("rtupdate1 destid: %i\n", rcvdpkt->destid);
    printf("rtupdate1 mincosts: ");
    for (int i = 0; i < 4; i++) {
        printf("%i ", rcvdpkt->mincost[i]);
    }
    printf("\n");

    // Steps to take:
    // - Check srcid (use as index for 2d distance table)
    // - Iterate through mincosts array (index is destination node)
    // - If connectcosts1[sourceid] + rcvdpkt->mincosts[i] < connectcosts1[sourceid] + dt1.costs[sourceid][i] then update dt1.costs[sourceid][i]
    // - If update made in last step, then resend dt1 to directly connected nodes

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

        printf("dt1.costs[i][srcid]: %i\n", dt1.costs[i][rcvdpkt->sourceid]);
        printf("connectcosts1[srcid]: %i\n", connectcosts1[rcvdpkt->sourceid]);
        printf("connectcost1[srcid] + rcvdpkt->mincost[i]: %i\n", connectcosts1[rcvdpkt->sourceid] + rcvdpkt->mincost[i]);
        if (dt1.costs[i][rcvdpkt->sourceid] == 0 || (connectcosts1[rcvdpkt->sourceid] + rcvdpkt->mincost[i]) < dt1.costs[i][rcvdpkt->sourceid]) {
            dt1.costs[i][rcvdpkt->sourceid] = connectcosts1[rcvdpkt->sourceid] + rcvdpkt->mincost[i];
            table_updated = true;
        }
    }

    printf("----------\n");

    if (table_updated == true) {
        printf("dt1 was updated. New table below.\n\n");
        printdt1(&dt1);

        // TODO: Send out updated distance tables to directly connected nodes.
        // Steps:
        // - For each destination dt1.costs[i] (except self), find minimum value in dt1.costs[i][j]
        // - Send minimum costs to each neighbouring node (connectcosts0[i] if not 999)


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
                if (dt1.costs[i][j] == 0) {
                    continue;
                }

                // Update mincost[i] with lowest cost for node i from each of the options of dt1.costs[i][j]
                if (dt1.costs[i][j] < mincosts1[i]) {
                    mincosts1[i] = dt1.costs[i][j];
                }
            }
        }

        printf("dt1update mincosts1: ");
        for (int i = 0; i < 4; i++) {
            printf("%i ", mincosts1[i]);
        }
        printf("\n");
    }
}


void printdt1(struct distance_table *dtptr) {
    printf("------------------------\n");
    printf("             via        \n");
    printf("   D1 |    0     2      \n");
    printf("------|-----------------\n");
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
