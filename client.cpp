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

typedef std::map<std::string, msgpack::object> MapStrMsgPackObj;

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
    int msqid;
    key_t key;
    msgbuffer rbuf;

    /*
     * Get the message queue id for the
     * "name" 1234, which was created by
     * the server.
     */
    key = 12434;

    if ((msqid = msgget(key, 0666)) < 0) {
        perror("msgget");
        return -1;
    }

    /*
     * Receive an answer of message type 1.
     */
    if (msgrcv(msqid, &rbuf, MSGSZ + 1, 1, 0) < 0) {
        perror("msgrcv");
        return -1;
    }
    msgpack::unpacker pac;

    msgpack::unpacked snt_msg;
    msgpack::unpack(&snt_msg, rbuf.mtext, MSGSZ);

    msgpack::object obj = snt_msg.get();

    // deserialize it.
    MapStrMsgPackObj mmap = obj.as<MapStrMsgPackObj>();

    cout <<  "[MX05_00] " << mmap["Message"] << endl;

    return 0;
}