#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <msgpack.hpp>
#define MSGSZ 256

#define MSGPACK_USE_CPP03
using namespace std;

/*
 * Declare the message structure.
 */

typedef struct msgbuffer {
         long    mtype;
         char    mtext[MSGSZ];
         };

std::string hexStr(unsigned char* data, int len)
{
    std::stringstream ss;
    ss << std::hex;
    for(int i=0;i<len;++i)
        ss << std::setw(2) << std::setfill('0') << (int)data[i];
    return ss.str();
}

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
    key = 12434;

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
    
    string msg = "Did you get this?";
    stringstream server_buf;
    msgpack::packer<stringstream> pk(&server_buf);
    pk.pack_map(2);
    pk.pack(string("ID"));
    pk.pack(msqid);
    pk.pack(string("Message"));
    pk.pack(msg);

    sbuf.mtype = 1;
    memcpy(sbuf.mtext, server_buf.str().data(), MSGSZ); 
    buf_length = strlen(sbuf.mtext) + 1 ;

    /*
     * Send a message.
     */
    if (msgsnd(msqid, &sbuf, MSGSZ + 1, IPC_NOWAIT) < 0) {
        printf ("%d, %d, %s, %d\n", msqid, sbuf.mtype, sbuf.mtext, int(buf_length));
        perror("msgsnd");
        return -1;
    }

    else 
    {
        msgpack::unpacked snt_msg;
        msgpack::unpack(&snt_msg, sbuf.mtext, MSGSZ);

        msgpack::object obj = snt_msg.get();
        cout << "[MX05_00] " << obj << endl;
    }
      
    return 0;
}