#include "kernel/types.h"
#include "user/user.h"

#define RD 0    //the read end of a pipe
#define WR 1    //the write end of a pipe

int main(int argc, char const *argv[]) {
    char buf = 'P';     //byte used to transmit

    int p_c2p[2];       //the file descriptors of pipe, child write and parent read
    int p_p2c[2];       //the file descriptors of pipe, child read and parent write
    pipe(p_c2p);
    pipe(p_p2c);

    int exit_status = 0;
    int pid = fork();   //fork to creat child process
    if (pid < 0) {
        fprintf(2, "fork() error!\n");
        close(p_p2c[RD]);
        close(p_p2c[WR]);
        close(p_c2p[RD]);
        close(p_c2p[WR]);
        exit(1);
    } else if (pid == 0) {
        //child process
        close(p_p2c[WR]);
        close(p_c2p[RD]);

        if (read(p_p2c[RD], &buf, sizeof(char)) != sizeof(char)) {      //child reads the pipeline
            fprintf(2, "Child read() fail!\n");
            exit_status = 1;
        } else {
            fprintf(1, "%d: received ping\n", getpid());
        }

        if (write(p_c2p[WR], &buf, sizeof(char)) != sizeof(char)) {     //child writes to pipeline
            fprintf(2, "Child write() fail!\n");
            exit_status = 1;
        }

        close(p_p2c[RD]);
        close(p_c2p[WR]);

        exit(exit_status);
    } else {
        //parent process
        close(p_p2c[RD]);
        close(p_c2p[WR]);

        if (write(p_p2c[WR], &buf, sizeof(char)) != sizeof(char)) {
            fprintf(2, "parent write() fail!\n");
            exit_status = 1;
        }

        wait(0);

        if ( read(p_c2p[RD], &buf, sizeof(char)) != sizeof(char) ) {
            fprintf(2, "parent read() fail!\n");
            exit_status = 1;
        } else {
            fprintf(1, "%d: received pong\n");
        }

        close(p_p2c[WR]);
        close(p_c2p[RD]);

        exit(exit_status);
    }
    
}