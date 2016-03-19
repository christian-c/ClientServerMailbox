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

int main()
{
    int msqidC;
    int msqid[NUM_PROCESSES];
    key_t key;
    msgbuffer rbuf, sbuf;

    printf("\nStarting Server...\n");

    key = CENTRAL_MAILBOX;

    if ((msqidC = msgget(key, 0666 | IPC_CREAT)) < 0) {
        cout << "[MX05_" << msqidC << "] NG..." << endl;
        perror("msgget");
        return -1;
    }
    else
    {
        cout << "[MX05_" << msqidC << "] OK..." << endl;   
    }

    printf("\nSetting up the mailbox...\n");  
    //Create the mailboxes for the other processes and store their IDs

    for(int i = 1; i <= NUM_PROCESSES; i++){
      msqid[i-1] = msgget((CENTRAL_MAILBOX + i), 0666 | IPC_CREAT);
      if (msqid[(i-1)] < 0) {
         cout << "[MX05_" << msqid[i-1] << "] NG..." << endl;
         return -1;
      }
      else
         cout << "[MX05_" << msqid[i-1] << "] OK..." << endl;
    }
    cout << "\n";

    while (1)
    
    {
        /*
         * Receive an answer of message type 1.
         */
        if (msgrcv(msqidC, &rbuf, MSGSZ + 1, 1, 0) < 0) {
            perror("msgrcv");
            return -1;
        }

        msgpack::unpacked rcv_msg;
        msgpack::unpack(&rcv_msg, rbuf.mtext, MSGSZ);

        msgpack::object obj = rcv_msg.get();

        // deserialize it.
        MapStrMsgPackObj mmap = obj.as<MapStrMsgPackObj>();
        
        std::string recv_msg;
        vector <int> prev_stat, cur_stat;
        int sender;
        mmap["ID"].convert(&sender);
        mmap["PreviousStat"].convert(&prev_stat);
        mmap["CurrentStat"].convert(&cur_stat);
        mmap["Message"].convert(&recv_msg);


        cout << "[MX05_" << mmap["ID"] << "] < Servo_Stat[-1] : " << prev_stat[0] << endl; 
        cout <<  "                Servo_Stat[ 0] : " << cur_stat[0] << endl; 
        cout <<  "                Message : " << recv_msg << "\n" << endl;

        string msg = "N/A";
        if (cur_stat[0] != prev_stat[0] && cur_stat[0] == 18)
            msg = "WRN_ALARM";
        stringstream send_buf;
        msgpack::packer<stringstream> pk(&send_buf);
        pk.pack_map(2);
        pk.pack(string("ID"));
        pk.pack(msqidC);
        pk.pack(string("Message"));
        pk.pack(msg);

        sbuf.mtype = 1;
        memcpy(sbuf.mtext, send_buf.str().data(), MSGSZ); 
        size_t buf_length = strlen(sbuf.mtext) + 1 ;

        if (msgsnd(msqid[0], &sbuf, MSGSZ + 1, 0) < 0) {
            cout << "[MX05_" << msqid << "] > NG...\n" << endl;
            perror("msgsnd");
            return -1;
        }

        else
        {
            msgpack::unpacked snd_msg;
            msgpack::unpack(&snd_msg, sbuf.mtext, MSGSZ);
            obj = snd_msg.get();
            cout << "[MX05_" << msqidC << "] > " << msg << "\n" << endl;
        }
    }

    return 0;
}