#include <stdio.h>
#include <stdlib.h>
#include <math.h>


typedef struct job {
 int id;
 int arrival;
 int unblock;
 int exectime;
 int respons;
 int turnaround;
 int wait;
 float ioratio;
 struct job *next;
} job;


typedef struct spec{
 int arrival;
 int exectime;
 float ioratio;
} spec;

spec specs[] = {
 {0, 10, 0.0},
 {0, 30, 0.7},
 {0, 20, 0.0},
 {40, 80, 0.4},
 {60, 30, 0.3},
 {120, 90, 0.3},
 {120, 40, 0.5},
 {140, 20, 0.2},
 {160, 10, 0.3},
 {180, 20, 0.3},
};

/*spec specs[] = {
    {0, 13, 0.0},
    {0, 4, 0.0},
    {0, 1, 0.0}
};*/

/*spec specs[] = {
    {0, 5, 0.0},
    {0, 3, 0.0},
    {0, 7, 0.0},
    {0, 1, 0.0}
};*/

job *readyq = NULL;
job *blockedq = NULL;
job *doneq = NULL;

void block(job *this){
    job *nxt = blockedq;
    job *prev = NULL;
    while (nxt != NULL){
        if (this->unblock < nxt->unblock)
            break;
        else {
            prev = nxt;
            nxt = nxt->next;
        }
    }
    this->next = nxt;
    if (prev != NULL)
        prev->next = this;
    else {
        blockedq = this;
    }
}

void ready(job *this){
    job *nxt = readyq;
    job *prev = NULL;

    while (nxt != NULL) {
        prev = nxt;
        nxt = nxt->next;
    }

    this->next = nxt;
    
    if (prev == NULL)
        readyq = this;
    else
        prev->next = this;
}

void unblock(int time){
    while (blockedq != NULL && blockedq->unblock <= time) {
        job *nxt = blockedq;
        blockedq = nxt->next;
        printf("(%4d) unblock job %2d\n", time, nxt->id);
        ready(nxt);
    }
}

void done (job *this){
    this->next = doneq;
    doneq = this;
}

#define ARRAY_LEN(array) (sizeof(array)/sizeof(array[0]))

void init() {
    for (int i = 0; i < ARRAY_LEN(specs); ++i) {
        job *new = (job*)malloc(sizeof(job));
        
        new->id = i + 1;
        new->arrival = specs[i].arrival;
        new->unblock = specs[i].arrival;
        new->exectime = specs[i].exectime;
        new->respons = -1;
        new->turnaround = 0;
        new->wait = 0;
        new->ioratio = specs[i].ioratio;
        
        block(new);
    }
}

#define IO_TIME 30

int io_op (float ratio, int exect) {
    int io = ((float) rand ( )) / RAND_MAX < ratio;
    if (io)
        io = (int) trunc( ((float) rand()) / RAND_MAX * (exect - 1)) + 1;
    return io;
}

int schedule(int time, int slot){
    if (readyq != NULL){
        job *nxt = readyq;
        readyq = readyq->next;

        if (nxt->respons == -1) {
            // printf("for job %d: time: %d, arrival: %d, respons: %d\n", nxt->id, time, nxt->arrival, time - nxt->arrival);
            nxt->respons = time - nxt->arrival;
            nxt->wait = nxt->respons;
        }

        int left = nxt->exectime;
        int exect = (left < slot) ? left : slot;

        int io = 0;
        if (exect > 1){
            io = io_op(nxt->ioratio, exect);
        }

        nxt->exectime -= exect;
        printf("(%4d) run job %2d for %3d ms", time, nxt->id , exect);
        
        if (nxt->exectime == 0){
            nxt->turnaround = time + exect - nxt->arrival;
            printf(" = done\n");
            done(nxt);
        } else {
            if (io){
                printf(" - %3d left - I/O\n", nxt->exectime);
                exect += IO_TIME;
            } else{
                printf(" - %3d left\n", nxt->exectime);
            }
            ready(nxt);
        }

        job *readynxt = readyq;
        while (readynxt) {
            if (readynxt->respons != -1 && readynxt != nxt) {
                readynxt->wait += exect;
            }
            readynxt = readynxt->next;
        }

        return exect;
    }
    else return 1;
}

int main(int argc, char *argv[]) {
    int slot = 10;
    if (argc == 2) {
        slot = atoi(argv[1]);
    }

    init();
    int time = 0;
    while (blockedq != NULL || readyq != NULL){
        unblock(time);

        int tick = schedule(time, slot);
        time += tick;
    }

    float tt = 0, rt = 0, wt = 0;
    int count = 0;
    job *nxt = doneq;
    while (nxt) {
        tt += nxt->turnaround;
        rt += nxt->respons;
        wt += nxt->wait;
        ++count;

        // printf("job: %d, wait: %d\n", nxt->id, nxt->wait);
        
        nxt = nxt->next;
    }
    tt /= count;
    rt /= count;
    wt /= count;

    printf("\n total execution time is %d \n", time);
    printf(" average exec time (tt): %f\n", tt);
    printf(" average response time: %f\n", rt);
    printf(" average wait time (wt): %f\n", wt);
    return 0;
}
