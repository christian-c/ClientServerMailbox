#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <msgpack.hpp>

#define MSGSZ 256
#define ROBOT_DOF 12

using namespace std;

/*
 * Declare the message structure.
 */

typedef struct msgbuffer {
    long    mtype;
    char    mtext[MSGSZ];
};

typedef std::map<std::string, msgpack::object> MapStrMsgPackObj;

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
    
    std::string recv_msg;
    vector <int> prev_stat, cur_stat;
    mmap["PreviousStat"].convert(&prev_stat);
    mmap["CurrentStat"].convert(&cur_stat);
    mmap["Message"].convert(&recv_msg);


    cout <<  "[MX05_00] Servo_Stat[-1] : " << prev_stat[0] << endl; 
    cout <<  "          Servo_Stat[ 0] : " << cur_stat[0] << endl; 
    cout <<  "          Message : " << recv_msg << endl;

    return 0;
}