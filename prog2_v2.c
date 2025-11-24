#pragma warning(disable:4996)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional or bidirectional
   data transfer protocols (from A to B. Bidirectional transfer of data
   is for extra credit and is not required).  Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

#define BIDIRECTIONAL 1   /* change to 1 if you're doing extra credit */
/* and write a routine called B_output */

/* a "msg" is the data unit passed from layer 5 (teachers code) to layer  */
/* 4 (students' code).  It contains the data (characters) to be delivered */
/* to layer 5 via the students transport level protocol entities.         */
struct msg {
    char data[20];
};

/* a packet is the data unit passed from layer 4 (students code) to layer */
/* 3 (teachers code).  Note the pre-defined packet structure, which all   */
/* students must follow. */
#define window_size 8 // window size == 8
#define BUFSIZE 1024
#define RTTTime 30.0 // RTT time == 30초

struct pkt {
    int seqnum;
    int acknum;
    int checksum;
    char payload[20];
};

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
struct A {
    int next_seqnum; // A의 sequence number
    int send_base; // A의 send base
    int ack_num; // A의 ack number

    struct pkt pending_packets[BUFSIZE]; // packet
    int timer_arr[BUFSIZE]; // timer를 중복 방지하는 배열
    int base_arr[BUFSIZE]; // base가 연속적으로 이어져있는지 확인하기 위해 만든 배열
}A;

struct B {
    int next_seqnum; // B의 sequence number
    int send_base; // B의 send base
    int ack_num; // B의 ack number

    struct pkt pending_packets[BUFSIZE]; // packet
    int timer_arr[BUFSIZE]; // timer를 중복 방지하는 배열
    int base_arr[BUFSIZE]; // base가 연속적으로 이어져있는지 확인하기 위해 만든 배열
}B;

unsigned short checksum(struct pkt* packet) {
    unsigned short sum = packet->seqnum ^ packet->acknum; 
    sum = ((packet->seqnum & packet->acknum) << 1) + sum;

    // [0, 1], [2,3] 이렇게 합침
    for (int i = 0; i < 10; i++) {
        unsigned short sum_payload = (packet->payload[i * 2] ^ packet->payload[(i * 2) + 1]);
        sum_payload = ((packet->payload[i * 2] & packet->payload[(i * 2) + 1]) << 1) + sum_payload; // xor + (& <<1)은 '+' 연산을 구현하는 것을 의미합니다.
        sum = (sum_payload ^ sum) + ((sum_payload & sum) << 1); 
    }
    return (unsigned short)~sum; // 1의 보수로 return 합니다.
}

/* called from layer 5, passed the data to be sent to other side */
A_output(message)
struct msg message;
{
    if (A.next_seqnum < A.send_base + window_size) { // window 안에 있는 경우
        // 패킷 만들기
        A.pending_packets[A.next_seqnum].seqnum = A.next_seqnum; // 패킷의 sequence number 
        if (A.ack_num == 0) A.pending_packets[A.next_seqnum].acknum = 999; // 처음 ack를 보내는 경우 acknum == 999
        else if (A.ack_num != 0) A.pending_packets[A.next_seqnum].acknum = A.ack_num; // 처음 ack 보내는 것이 아니므로, ack = input에서 최신화된 ack_num을 acknum에 받음
        strcpy(A.pending_packets[A.next_seqnum].payload, message.data); // message data를 packet의 payload에 복사
        // make checksum
        A.pending_packets[A.next_seqnum].checksum = checksum(&A.pending_packets[A.next_seqnum]); // 체크썸을 만들고 packet.checksum에 저장

        tolayer3(0, A.pending_packets[A.next_seqnum]); // 3 layer로 이동시킴 즉, B로 패킷을 전송시킴
        if (A.pending_packets[A.next_seqnum].acknum == 999) printf("A_output : Send_packet without ACK (seq: %d) \n", A.next_seqnum); // 처음 보내는 경우 seq number만 출력
        else if (A.pending_packets[A.next_seqnum].acknum != 999) printf("A_output : Send_packet with ACK (seq: %d ack: %d) \n", A.pending_packets[A.next_seqnum].seqnum, A.pending_packets[A.next_seqnum].acknum); // 처음이 아닌 경우 sequence number와 ack number 출력 
        
        starttimer(0, RTTTime, A.pending_packets[A.next_seqnum].seqnum); // 해당 seqnum을 값을 가지는 packet에 대해 timer start (RTT time 동안)
        A.timer_arr[A.pending_packets[A.next_seqnum].seqnum] = 1; // timer가 중복해서 꺼지는 것을 방지하기 위해서 set함
        printf("A_output : start timer. \n");
        printf("A_output : %s \n", A.pending_packets[A.next_seqnum].payload);
        A.next_seqnum++; // next seq number가 1 증가함
    }
    else if (A.next_seqnum >= A.send_base + window_size) printf("A_output : Out of Window \n"); // window 밖에 있는 경우
}

B_output(message)  /* need be completed only for extra credit */
struct msg message;
{
    if (B.next_seqnum < B.send_base + window_size) { // window 안에 있는 경우
        // 패킷 만들기
        B.pending_packets[B.next_seqnum].seqnum = B.next_seqnum; // 패킷의 sequence number
        if (B.ack_num == 0) B.pending_packets[B.next_seqnum].acknum = 999; // 만약 ack_num == 0 -> 처음을 의미하므로, acknum == 999
        else if (B.ack_num != 0) B.pending_packets[B.next_seqnum].acknum = B.ack_num; // 처음이 아니므로, input에서 최신화된 ack_num을 acknum에 받음
        strcpy(B.pending_packets[B.next_seqnum].payload, message.data); // message data를 packet의 payload에 복사
        // make checksum
        B.pending_packets[B.next_seqnum].checksum = checksum(&B.pending_packets[B.next_seqnum]); // 체크썸 만들고 packet.checksum에 저장

        tolayer3(1, B.pending_packets[B.next_seqnum]); // 3 layer로 이동시킴 즉, A로 패킷 전송 
        if (B.pending_packets[B.next_seqnum].acknum == 999) printf("B_output : Send_packet without ACK (seq: %d) \n", B.next_seqnum); // 처음 보내는 경우 seq number만 출력
        else if (B.pending_packets[B.next_seqnum].acknum != 999) printf("B_output : Send_packet with ACK (seq: %d ack: %d) \n", B.pending_packets[B.next_seqnum].seqnum, B.pending_packets[B.next_seqnum].acknum); // 처음이 아닌 경우 sequence number와 ack number 출력

        starttimer(1, RTTTime, B.pending_packets[B.next_seqnum].seqnum); // 해당 seqnum을 값을 가지는 packet에 대해 timer start (RTT time 동안)
        B.timer_arr[B.pending_packets[B.next_seqnum].seqnum] = 1; // timer 배열은 중복해서 timer를 끄는 것을 방지하기 위해, set함.
        printf("B_output : start timer. \n");
        printf("B_output : %s \n", B.pending_packets[B.next_seqnum].payload);
        B.next_seqnum++; // next seq number 1 증가
    }
    else if (B.next_seqnum >= B.send_base + window_size) printf("B_output : Out of Window \n"); // window 밖에 있는 경우

}

/* called from layer 3, when a packet arrives for layer 4 */
A_input(packet)
struct pkt packet;
{
    int rcv_checksum = packet.checksum; // 받은 packet에 저장되어 있는 checksum
    int sub_checksum = checksum(&packet); // 받은 packet을 직접 checksum 함수로 계산해서 checksum을 구함
    if ((sub_checksum ^ rcv_checksum) == 0) { // 비교인데, XOR(1의 갯수가 홀수면 1, 1의 갯수가 짝수면 0)인데, 같은 수가 나오면 항상 0
        printf("A_input : Receive_packet %s (seq: %d ack: %d) \n", packet.payload, packet.seqnum, packet.acknum); // receive packet 출력

        tolayer5(1, packet.payload); // 5 layer(A)로 packet의 payload를 보냄

        A.ack_num = packet.seqnum; // input의 sequence number == output의 acknum이므로 대입
        if (packet.acknum != 999 && A.timer_arr[packet.acknum] == 1) { // packet이 처음이 아니고, timer배열의 값이 1인 경우 즉, 중복되지 않은 경우
            stoptimer(0, packet.acknum); // 해당 packet.acknum timer을 stop
            A.timer_arr[packet.acknum] = 0; // 중복 방지를 위해 timer 배열 0으로 set
            A.base_arr[packet.acknum] = packet.acknum; // 연속적인 것을 검사하기 위해 base arr에 데이터 채움
            printf("A_input : stop timer. \n");
        }

        if (A.send_base == packet.acknum) {
            for (int i = 0; i < window_size; i++) { // 8번 반복하는데 이어져있는게 끊겨있으면 stop 하고 해당 값을 다음 send_base로 함
                A.send_base++; // send base는 하나씩 증가
                if (A.base_arr[i] == 0) break; // 이어져있는게 끊겨있을 경우
            }
        }

    }
    else if ((sub_checksum ^ rcv_checksum) != 0) printf("A_input : Fail to receive_packet %s \n", packet.payload);

}

/* called when A's timer goes off */
  /* This is called when the timer goes off
   * I loop from the base till the nextseq number and decrement 1 each time . If the value is 0
   * I will retransmit the packet.
   * If the value is 0 previously I just skip beacuse the ack for the pkt is received
   */
A_timerinterrupt(int seqnum)
{
    tolayer3(0, A.pending_packets[seqnum]); // timeout이 생길 경우 재전송을 해줘야함 따라서, pending_packet[seqnum]을 다시 B로 재전송함 
    starttimer(0, RTTTime, A.pending_packets[seqnum].seqnum); // timer도 재전송 하므로 다시 돌려야함
    printf("A_timerinterrupt : timeinterrupt : Send_packet with ACK (ACK = %d, seq = %d) \n", A.pending_packets[seqnum].acknum, A.pending_packets[seqnum].seqnum);
    printf("A_timerinterrupt : %s \n", A.pending_packets[seqnum].payload);
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
A_init()
{
    A.send_base = 1; // send_base 초기화
    A.next_seqnum = 1; // next_seqnum 1로 초기화
    A.ack_num = 0; // 처음 ack number 
    memset(A.pending_packets, NULL, sizeof(A.pending_packets));
    memset(A.timer_arr, 0, sizeof(A.timer_arr));
    memset(A.base_arr, 0, sizeof(A.base_arr));
}


/* Note that with simplex transfer from a-to-B, there is no B_output() */


/* called from layer 3, when a packet arrives for layer 4 at B*/
B_input(packet)
struct pkt packet;
{
    int rcv_checksum = packet.checksum; // 받은 packet에 저장되어 있는 checksum
    int sub_checksum = checksum(&packet); // 받은 packet을 직접 checksum 함수로 계산해서 checksum을 구함
    if ((sub_checksum ^ rcv_checksum) == 0) { // 비교인데, XOR(1의 갯수가 홀수면 1, 1의 갯수가 짝수면 0)인데, 같은 수가 나오면 항상 0
        printf("B_input : Receive_packet %s (seq: %d ack: %d) \n", packet.payload, packet.seqnum, packet.acknum);

        tolayer5(0, packet.payload); // 5 layer(B)로 packet의 payload를 보냄
   
        B.ack_num = packet.seqnum; // input의 sequence number == output의 acknum이므로 대입
        if (packet.acknum != 999 && B.timer_arr[packet.acknum] == 1) { // packet이 처음이 아니고, timer배열의 값이 1인 경우 즉, 중복되지 않은 경우
            stoptimer(1, packet.acknum); // 해당 packet.acknum timer을 stop
            B.timer_arr[packet.acknum] = 0; // 중복 방지를 위해 timer 배열 0으로 set
            B.base_arr[packet.acknum] = packet.acknum; // 연속적인 것을 검사하기 위해 base arr에 데이터 채움
            printf("B_input : stop timer. \n");
        }

        if (B.send_base == packet.acknum) {
            for (int i = 0; i < window_size; i++) { // 8번 반복하는데 이어져있는게 끊겨있으면 stop 하고 해당 값을 다음 send_base로 함
                B.send_base++; // send base는 하나씩 증가
                if (B.base_arr[i] == 0) break; // 이어져있는게 끊겨있을 경우
            }
        }
    }
    else if ((sub_checksum ^ rcv_checksum) != 0) printf("B_input : Fail to receive_packet %s \n", packet.payload); // checksum이 서로 다른 경우 0이 나올 수가 없음
}

/* called when B's timer goes off */
B_timerinterrupt(int seqnum)
{
    tolayer3(1, B.pending_packets[seqnum]); // timeout이 생길 경우 재전송을 해줘야함 따라서, pending_packet[seqnum]을 다시 A로 재전송함
    starttimer(1, RTTTime, B.pending_packets[seqnum].seqnum); // timer도 재전송 하므로 다시 돌려야함
    printf("B_timerinterrupt : timeinterrupt : Send_packet with ACK (ACK = %d, seq = %d) \n", B.pending_packets[seqnum].acknum, B.pending_packets[seqnum].seqnum);
    printf("B_timerinterrupt : %s \n", B.pending_packets[seqnum].payload);

}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
B_init()
{
    B.send_base = 1; // send_base 1로 초기화
    B.next_seqnum = 1; // next_seqnum 1로 초기화
    B.ack_num = 0; // 처음 ack number는 0
    memset(B.pending_packets, NULL, sizeof(B.pending_packets));
    memset(B.timer_arr, 0, sizeof(B.timer_arr));
    memset(B.base_arr, 0, sizeof(B.base_arr));
}


/*****************************************************************
***************** NETWORK EMULATION CODE STARTS BELOW ***********
The code below emulates the layer 3 and below network environment:
  - emulates the tranmission and delivery (possibly with bit-level corruption
    and packet loss) of packets across the layer 3/4 interface
  - handles the starting/stopping of a timer, and generates timer
    interrupts (resulting in calling students timer handler).
  - generates message to be sent (passed from later 5 to 4)

THERE IS NOT REASON THAT ANY STUDENT SHOULD HAVE TO READ OR UNDERSTAND
THE CODE BELOW.  YOU SHOLD NOT TOUCH, OR REFERENCE (in your code) ANY
OF THE DATA STRUCTURES BELOW.  If you're interested in how I designed
the emulator, you're welcome to look at the code - but again, you should have
to, and you defeinitely should not have to modify
******************************************************************/

struct event {
    float evtime;           /* event time */
    int evtype;             /* event type code */
    int eventity;           /* entity where event occurs */
    struct pkt packet;      /* packet (if any) assoc w/ this event */
    struct event* prev;
    struct event* next;
};
struct event* evlist = NULL;   /* the event list */

/* possible events: */
#define  TIMER_INTERRUPT 0  
#define  FROM_LAYER5     1
#define  FROM_LAYER3     2

#define  OFF             0
#define  ON              1
#define  A               0
#define  B               1



int TRACE = 1;             /* for my debugging */
int nsim = 0;              /* number of messages from 5 to 4 so far */
int nsimmax = 0;           /* number of msgs to generate, then stop */
float time = 0.000;
float lossprob;            /* probability that a packet is dropped  */
float corruptprob;         /* probability that one bit is packet is flipped */
float lambda;              /* arrival rate of messages from layer 5 */
int   ntolayer3;           /* number sent into layer 3 */
int   nlost;               /* number lost in media */
int ncorrupt;              /* number corrupted by media*/

main()
{
    struct event* eventptr;
    struct msg  msg2give;
    struct pkt  pkt2give;

    int i, j;
    char c;

    init();
    A_init();
    B_init();

    while (1) {
        eventptr = evlist;            /* get next event to simulate */
        if (eventptr == NULL)
            goto terminate;
        evlist = evlist->next;        /* remove this event from event list */
        if (evlist != NULL)
            evlist->prev = NULL;
        if (TRACE >= 2) {
            printf("\nEVENT time: %f,", eventptr->evtime);
            printf("  type: %d", eventptr->evtype);
            if (eventptr->evtype == 0)
                printf(", timerinterrupt  ");
            else if (eventptr->evtype == 1)
                printf(", fromlayer5 ");
            else
                printf(", fromlayer3 ");
            printf(" entity: %d\n", eventptr->eventity);
        }
        time = eventptr->evtime;        /* update time to next event time */
        if (nsim == nsimmax)
            break;                        /* all done with simulation */
        if (eventptr->evtype == FROM_LAYER5) {
            generate_next_arrival();   /* set up future arrival */
            /* fill in msg to give with string of same letter */
            j = nsim % 26;
            for (i = 0; i < 20; i++)
                msg2give.data[i] = 97 + j;
            if (TRACE > 2) {
                printf("          MAINLOOP: data given to student: ");
                for (i = 0; i < 20; i++)
                    printf("%c", msg2give.data[i]);
                printf("\n");
            }
            nsim++;
            if (eventptr->eventity == A)
                A_output(msg2give);
            else
                B_output(msg2give);
        }
        else if (eventptr->evtype == FROM_LAYER3) {
            pkt2give.seqnum = eventptr->packet.seqnum;
            pkt2give.acknum = eventptr->packet.acknum;
            pkt2give.checksum = eventptr->packet.checksum;
            for (i = 0; i < 20; i++)
                pkt2give.payload[i] = eventptr->packet.payload[i];
            if (eventptr->eventity == A)      /* deliver packet by calling */
                A_input(pkt2give);            /* appropriate entity */
            else
                B_input(pkt2give);
        }
        else if (eventptr->evtype == TIMER_INTERRUPT) {
            if (eventptr->eventity == A)
                A_timerinterrupt(eventptr->packet.seqnum);
            else
                B_timerinterrupt(eventptr->packet.seqnum);
        }
        else {
            printf("INTERNAL PANIC: unknown event type \n");
        }
        free(eventptr);
    }

terminate:
    printf(" Simulator terminated at time %f\n after sending %d msgs from layer5\n", time, nsim);
}



init()                         /* initialize the simulator */
{
    int i;
    float sum, avg;
    float jimsrand();


    printf("-----  Stop and Wait Network Simulator Version 1.1 -------- \n\n");
    printf("Enter the number of messages to simulate: ");
    scanf("%d", &nsimmax);
    printf("Enter packet loss probability [enter 0.0 for no loss]:");
    scanf("%f", &lossprob);
    printf("Enter packet corruption probability [0.0 for no corruption]:");
    scanf("%f", &corruptprob);
    printf("Enter average time between messages from sender's layer5 [ > 0.0]:");
    scanf("%f", &lambda);
    printf("Enter TRACE:");
    scanf("%d", &TRACE);

    srand(9999);              /* init random number generator */
    sum = 0.0;                /* test random number generator for students */
    for (i = 0; i < 1000; i++)
        sum = sum + jimsrand();    /* jimsrand() should be uniform in [0,1] */
    avg = sum / 1000.0;
    if (avg < 0.25 || avg > 0.75) {
        printf("It is likely that random number generation on your machine\n");
        printf("is different from what this emulator expects.  Please take\n");
        printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
        exit(0);
    }

    ntolayer3 = 0;
    nlost = 0;
    ncorrupt = 0;

    time = 0.0;                    /* initialize time to 0.0 */
    generate_next_arrival();     /* initialize event list */
}

/****************************************************************************/
/* jimsrand(): return a float in range [0,1].  The routine below is used to */
/* isolate all random number generation in one location.  We assume that the*/
/* system-supplied rand() function return an int in therange [0,mmm]        */
/****************************************************************************/
float jimsrand()
{
    //double mmm = 2147483647;   /* largest int  - MACHINE DEPENDENT!!!!!!!!   */
    double mmm = 32767;   /* largest int  - MACHINE DEPENDENT!!!!!!!!   */
    float x;                   /* individual students may need to change mmm */
    x = rand() / mmm;            /* x should be uniform in [0,1] */
    return(x);
}

/********************* EVENT HANDLINE ROUTINES *******/
/*  The next set of routines handle the event list   */
/*****************************************************/

generate_next_arrival()
{
    double x, log(), ceil();
    struct event* evptr;
    char* malloc();
    float ttime;
    int tempint;

    if (TRACE > 2)
        printf("          GENERATE NEXT ARRIVAL: creating new arrival\n");

    x = lambda * jimsrand() * 2;  /* x is uniform on [0,2*lambda] */
    /* having mean of lambda        */
    evptr = (struct event*)malloc(sizeof(struct event));
    evptr->evtime = time + x;
    evptr->evtype = FROM_LAYER5;
    if (BIDIRECTIONAL && (jimsrand() > 0.5))
        evptr->eventity = B;
    else
        evptr->eventity = A;
    insertevent(evptr);
}


insertevent(p)
struct event* p;
{
    struct event* q, * qold;

    if (TRACE > 2) {
        printf("            INSERTEVENT: time is %lf\n", time);
        printf("            INSERTEVENT: future time will be %lf\n", p->evtime);
    }
    q = evlist;     /* q points to header of list in which p struct inserted */
    if (q == NULL) {   /* list is empty */
        evlist = p;
        p->next = NULL;
        p->prev = NULL;
    }
    else {
        for (qold = q; q != NULL && p->evtime > q->evtime; q = q->next)
            qold = q;
        if (q == NULL) {   /* end of list */
            qold->next = p;
            p->prev = qold;
            p->next = NULL;
        }
        else if (q == evlist) { /* front of list */
            p->next = evlist;
            p->prev = NULL;
            p->next->prev = p;
            evlist = p;
        }
        else {     /* middle of list */
            p->next = q;
            p->prev = q->prev;
            q->prev->next = p;
            q->prev = p;
        }
    }
}

printevlist()
{
    struct event* q;
    int i;
    printf("--------------\nEvent List Follows:\n");
    for (q = evlist; q != NULL; q = q->next) {
        printf("Event time: %f, type: %d entity: %d\n", q->evtime, q->evtype, q->eventity);
    }
    printf("--------------\n");
}



/********************** Student-callable ROUTINES ***********************/

/* called by students routine to cancel a previously-started timer */
stoptimer(AorB, seqnum)
int AorB;  /* A or B is trying to stop timer */
int seqnum; /* sequence number of the packet whose timer needs to be stopped */
{
    struct event* q;

    if (TRACE > 2)
        printf("          STOP TIMER: stopping timer at %f for sequence number %d\n", time, seqnum);

    /* Search for the TIMER_INTERRUPT event with the given sequence number and AorB */
    for (q = evlist; q != NULL; q = q->next) {
        if ((q->evtype == TIMER_INTERRUPT) && (q->eventity == AorB) && (q->packet.seqnum == seqnum)) {
            /* remove this event */
            if (q->next == NULL && q->prev == NULL)
                evlist = NULL;         /* remove first and only event on list */
            else if (q->next == NULL) /* end of list - there is one in front */
                q->prev->next = NULL;
            else if (q == evlist) {   /* front of list - there must be event after */
                q->next->prev = NULL;
                evlist = q->next;
            }
            else {      /* middle of list */
                q->next->prev = q->prev;
                q->prev->next = q->next;
            }
            free(q);
            return;
        }
    }
    printf("Warning: unable to cancel your timer. It wasn't running.\n");
}



starttimer(AorB, increment, seqnum)
int AorB;  /* A or B is trying to start timer */
float increment;
int seqnum; /* sequence number of the packet whose timer needs to be started */
{
    struct event* q;
    struct event* evptr;
    char* malloc();

    if (TRACE > 2)
        printf("          START TIMER: starting timer at %f for sequence number %d\n", time, seqnum);

    /* Check if timer is already started for this sequence number */
    for (q = evlist; q != NULL; q = q->next) {
        if ((q->evtype == TIMER_INTERRUPT) && (q->eventity == AorB) && (q->packet.seqnum == seqnum)) {
            printf("Warning: attempt to start a timer that is already started\n");
            return;
        }
    }

    /* create future event for when timer goes off */
    evptr = (struct event*)malloc(sizeof(struct event));
    evptr->evtime = time + increment;
    evptr->evtype = TIMER_INTERRUPT;
    evptr->eventity = AorB;
    evptr->packet.seqnum = seqnum;
    insertevent(evptr);
}


/************************** TOLAYER3 ***************/
tolayer3(AorB, packet)
int AorB;  /* A or B is trying to stop timer */
struct pkt packet;
{
    struct pkt mypktptr;
    struct event* evptr, * q;
    char* malloc();
    float lastime, x, jimsrand();
    int i;


    ntolayer3++;

    /* simulate losses: */
    if (jimsrand() < lossprob) {
        nlost++;
        if (TRACE > 0)
            printf("          TOLAYER3: packet%d being lost\n", packet.seqnum);
        return;
    }

    /* make a copy of the packet student just gave me since he/she may decide */
    /* to do something with the packet after we return back to him/her */
    mypktptr.seqnum = packet.seqnum;
    mypktptr.acknum = packet.acknum;
    mypktptr.checksum = packet.checksum;
    for (i = 0; i < 20; i++)
        mypktptr.payload[i] = packet.payload[i];
    if (TRACE > 2) {
        printf("          TOLAYER3: seq: %d, ack %d, check: %d ", mypktptr.seqnum,
            mypktptr.acknum, mypktptr.checksum);
        for (i = 0; i < 20; i++)
            printf("%c", mypktptr.payload[i]);
        printf("\n");
    }

    /* create future event for arrival of packet at the other side */
    evptr = (struct event*)malloc(sizeof(struct event));
    evptr->evtype = FROM_LAYER3;   /* packet will pop out from layer3 */
    evptr->eventity = (AorB + 1) % 2; /* event occurs at other entity */
    /* finally, compute the arrival time of packet at the other end.
       medium can not reorder, so make sure packet arrives between 1 and 10
       time units after the latest arrival time of packets
       currently in the medium on their way to the destination */
    lastime = time;
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next) */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == FROM_LAYER3 && q->eventity == evptr->eventity))
            lastime = q->evtime;
    evptr->evtime = lastime + 1 + 9 * jimsrand();

    /* simulate corruption: */
    if (jimsrand() < corruptprob) {
        ncorrupt++;
        if ((x = jimsrand()) < .75)
            mypktptr.payload[0] = 'Z';   /* corrupt payload */
        else if (x < .875)
            mypktptr.seqnum = 999999;
        else
            mypktptr.acknum = 999999;
        if (TRACE > 0)
            printf("          TOLAYER3: packet%d being corrupted\n", packet.seqnum);
    }
    evptr->packet = mypktptr;       /* save ptr to my copy of packet */
    if (TRACE > 2)
        printf("          TOLAYER3: scheduling arrival on other side\n");
    insertevent(evptr);
}

tolayer5(AorB, datasent)
int AorB;
char datasent[20];
{
    int i;
    if (TRACE > 2) {
        printf("          TOLAYER5: data received: ");
        for (i = 0; i < 20; i++)
            printf("%c", datasent[i]);
        printf("\n");
    }
}