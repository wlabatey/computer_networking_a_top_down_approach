#include <stdio.h>

#include "prog3.h"


struct distance_table {
    int costs[4][4];
} dt3;


/* students to write the following two routines, and maybe some others */

void rtinit3() {
}


void rtupdate3(struct rtpkt *rcvdpkt) {

}


void printdt3(struct distance_table *dtptr) {
    printf("             via     \n");
    printf("   D3 |    0     2 \n");
    printf("  ----|-----------\n");
    printf("     0|  %3d   %3d\n", dtptr->costs[0][0], dtptr->costs[0][2]);
    printf("dest 1|  %3d   %3d\n", dtptr->costs[1][0], dtptr->costs[1][2]);
    printf("     2|  %3d   %3d\n", dtptr->costs[2][0], dtptr->costs[2][2]);
}
