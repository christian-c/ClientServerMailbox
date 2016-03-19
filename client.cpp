#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <msgpack.hpp>
#define MSGSZ 256

using namespace std;

/*
 * Declare the message structure.
 */

typedef struct msgbuffer {
    long    mtype;
    char    mtext[MSGSZ];
};


main()
{
    int msqid;
    key_t key;
    msgbuffer rbuf;

    /*
     * Get the message queue id for the
     * "name" 1234, which was created by
     * the server.
     */
    key = 1234;

    if ((msqid = msgget(key, 0666)) < 0) {
        perror("msgget");
        return -1;
    }

    
    /*
     * Receive an answer of message type 1.
     */
    if (msgrcv(msqid, &rbuf, MSGSZ, 1, 0) < 0) {
        perror("msgrcv");
        return -1;
    }

    /*
     * Print the answer.
     */
    printf("%s\n", rbuf.mtext);
    return 0;
}