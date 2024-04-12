#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

const uint INT_LEN = sizeof(int);

void primes(int lpipe[2]);
int lpipe_first_data(int lpipe[2], int *dst);
void transmit_data(int lpipe[2], int rpipe[2], int first);

int main(int argc, char const *argv[]) {
    
    int p[2];
    pipe(p);

    for ( int i = 2; i <= 35; ++i ) {
        write(p[WR], &i, INT_LEN);
    }

    if (fork() == 0) {
        primes(p);
    } else {
        close(p[WR]);
        close(p[RD]);
        wait(0);
    }
    
    exit(0);
}

void primes(int lpipe[2]) {
    close(lpipe[WR]);
    int first;
    if ( lpipe_first_data(lpipe, &first) == 0 ) {
        int p[2];
        pipe(p);
        transmit_data(lpipe, p, first);

        if (fork() == 0) {
            primes(p);
        } else {
            close(p[RD]);
            wait(0);
        }
    }
    exit(0);
}

int lpipe_first_data(int lpipe[2], int *dst) {
    if ( read(lpipe[RD], dst, INT_LEN) == INT_LEN ) {
        printf("prime %d\n", *dst);
        return 0;
    }
    return -1;
}

void transmit_data(int lpipe[2], int rpipe[2], int first) {
    int data;
    while ( read(lpipe[RD], &data, INT_LEN) == INT_LEN ) {
        if ( data % first) {
            write(rpipe[WR], &data, INT_LEN);
        }
    }
    close(lpipe[RD]);
    close(rpipe[WR]);
}



