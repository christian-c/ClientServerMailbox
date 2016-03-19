#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
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

int main()
{
    msgbuffer sbuf;
    int msqid;
    int msgflg = IPC_CREAT | 0666;
    key_t key;
    size_t buf_length;

    /*
     * Get the message queue id for the
     * "name" 1234, which was created by
     * the server.
     */
    key = 1234;

    cout << "\nmsgget: Calling msgget(" << key <<") " << endl;

    if ((msqid = msgget(key, msgflg )) < 0) {
        perror("msgget");
        return -1;
    }
    else 
        cout << "msgget: msgget succeeded: msqid = " << msqid << "\n";

    /*
     * We'll send message type 1
     */
     
    sbuf.mtype = 1;
    
    cout << "msgget: msgget succeeded: msqid = " << msqid << "\n";
    
    strcpy(sbuf.mtext, "Did you get this?");
    
    cout << "msgget: msgget succeeded: msqid = " << msqid << "\n";
    
    buf_length = strlen(sbuf.mtext) + 1 ;
    
    

    /*
     * Send a message.
     */
    if (msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT) < 0) {
        printf ("%d, %d, %s, %d\n", msqid, sbuf.mtype, sbuf.mtext, int(buf_length));
        perror("msgsnd");
        return -1;
    }

   else 
      printf("Message: \"%s\" Sent\n", sbuf.mtext);
      
    return 0;
}