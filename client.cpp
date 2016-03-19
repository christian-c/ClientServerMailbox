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
#define CENTRAL_MAILBOX 1121    //Central Mailbox number 
#define NUM_PROCESSES 4            //Total number of external processes

using namespace std;

/*
 * Declare the message structure.
 */

typedef struct msgbuffer {
    long    mtype;
    char    mtext[MSGSZ];
};

typedef std::map<std::string, msgpack::object> MapStrMsgPackObj;


int main(int argc, char *argv[])
{
    msgbuffer sbuf, rbuf;
    int uid;
    int msqid, msqidC;
    int msgflg = IPC_CREAT | 0666;
    key_t key;
    size_t buf_length;

    if(argc != 2) {
      cout << "[ERROR] Too few arguments" << endl;
      cout << "[USAGE] ./client_start uid" << endl;
      return -1;
    }
    else
    {
        uid = atoi(argv[1]);
        if (uid <= 0 || uid >= 4)
        {
            cout << "[ERROR] 1 <= UID <= 4" << endl;
            return -1;
        }
    }

    key = CENTRAL_MAILBOX;

    printf("\nLocating Server...\n");

    cout << "msgget: Calling msgget(" << key <<") " << endl;

    if ((msqidC = msgget(key, msgflg )) < 0) {
        perror("msgget");
        return -1;
    }
    else 
        cout << "msgget: msgget succeeded: msqid = " << msqidC << "\n";

    printf("\nStarting Client...\n");

    if ((msqid = msgget(key + uid, msgflg )) < 0) {
        perror("msgget");
        cout << "\n";
        return -1;
    }
    else 
        cout << "msgget: msgget succeeded: msqid = " << msqid << "\n" << endl;

    vector <int> cur_servo_stt, pre_servo_stt;

    for (int i = 0; i < ROBOT_DOF; ++i)
    {
        pre_servo_stt.push_back(-1);
        cur_servo_stt.push_back(0);
    }

    while (1)
    {
        cout << "[MX05_" << msqid << "] > ";
        int cur_stat;
        cin >> cur_stat;
        
        for (int i = 0; i < ROBOT_DOF; ++i)
        {
            cur_servo_stt[i] = cur_stat;
        }

        string msg = "N/A";
        stringstream send_buf;
        msgpack::packer<stringstream> pk(&send_buf);
        pk.pack_map(4);
        pk.pack(string("ID"));
        pk.pack(msqid);
        pk.pack(string("PreviousStat"));
        pk.pack(pre_servo_stt);
        pk.pack(string("CurrentStat"));
        pk.pack(cur_servo_stt);   
        pk.pack(string("Message"));
        pk.pack(msg);


        sbuf.mtype = 1;
        memcpy(sbuf.mtext, send_buf.str().data(), MSGSZ); 
        buf_length = strlen(sbuf.mtext) + 1 ;

        /*
         * Send a message.
         */
        if (msgsnd(msqidC, &sbuf, MSGSZ + 1, IPC_NOWAIT) < 0) {
            cout << "[MX05_" << msqid << "] > NG...\n" << endl;
            perror("msgsnd");
            return -1;
        }

        else 
        {
            msgpack::unpacked snt_msg;
            msgpack::unpack(&snt_msg, sbuf.mtext, MSGSZ);

            msgpack::object obj = snt_msg.get();
            cout << "[MX05_" << msqid << "] > OK...\n" << endl;
        }

        for (int i = 0; i < ROBOT_DOF; ++i)
        {
            pre_servo_stt[i] = cur_servo_stt[i];
        }

        if (msgrcv(msqid, &rbuf, MSGSZ + 1, 1, 0) < 0) {
            perror("msgrcv");
            return -1;
        }

        msgpack::unpacked rcv_msg;
        msgpack::unpack(&rcv_msg, rbuf.mtext, MSGSZ);

        msgpack::object obj = rcv_msg.get();

        // deserialize it.
        MapStrMsgPackObj mmap = obj.as<MapStrMsgPackObj>();
        
        std::string recv_msg;
        int sender;
        mmap["ID"].convert(&sender);
        mmap["Message"].convert(&recv_msg);

        cout << "[MX05_" << sender << "] < " << recv_msg << "\n" << endl;

    }

    return 0;
}