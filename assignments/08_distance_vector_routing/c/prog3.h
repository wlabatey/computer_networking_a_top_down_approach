/* a rtpkt is the packet sent from one routing update process to
   another via the call tolayer3() */
struct rtpkt {
    int sourceid;       /* id of sending router sending this pkt */
    int destid;         /* id of router to which pkt being sent (must be an immediate neighbor) */
    int mincost[4];     /* min cost to node 0 ... 3 */
};

struct event {
    float evtime;            /* event time */
    int evtype;              /* event type code */
    int eventity;            /* entity where event occurs */
    struct rtpkt *rtpktptr;  /* ptr to packet (if any) assoc w/ this event */
    struct event *prev;
    struct event *next;
};

struct distance_table {
    int costs[4][4];
};

void creatertpkt(struct rtpkt *initrtpkt, int srcid, int destid, int mincosts[]);

void insertevent(struct event *evptr);

float jimsrand();

void printevlist();

void tolayer2(struct rtpkt packet);

void init();

extern int TRACE;  /* for my debugging */
extern int YES;
extern int NO;
