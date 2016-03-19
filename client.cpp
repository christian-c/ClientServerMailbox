#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <msgpack.hpp>

#define MSGSZ 256
#define CENTRAL_MAILBOX 12434    //Central Mailbox number 
#define NUM_PROCESSES 1            //Total number of external processes
#define ROBOT_DOF 12

using namespace std;

typedef struct msgbuffer {
         long    mtype;
         char    mtext[MSGSZ];
         };

//MAIN function
int main(int argc, char *argv[]) {

   msgbuffer msgp, cmbox;

   if(argc != 2) {
      cout << "[ERROR] Too few arguments" << endl;
      cout << "[USAGE] ./client_start uid" << endl;
      return -1;
   }

   //Setup local variables
   int result, length, status;
   int uid = atoi(argv[1]);

   //Create the Central Servers Mailbox
   int msqidC = msgget(CENTRAL_MAILBOX, 0666 | IPC_CREAT);
   if (msqidC < 0)
      return -1;

   //Create the mailbox for this process and store it's IDs
   int msqid = msgget((CENTRAL_MAILBOX + uid), 0666 | IPC_CREAT);
   if (msqid < 0)
      return -1;

   cout << "[CMBOX ID]" << msqidC << endl;
   cout << "[USER ID]" << msqid << endl;

   //Initialize the message to be sent
   msgp.mtype = 1;
   string msg = "N/A";
   vector <int> cur_servo_stt, pre_servo_stt;

   for (int i = 0; i < ROBOT_DOF; ++i)
   {
     pre_servo_stt.push_back(-1);
     cur_servo_stt.push_back(-1);
   }

   //while(1){

      int cur_stat;
      cout << "[MX05_" << msqid << "] > ";
      cin >> cur_stat;
   
      for (int i = 0; i < ROBOT_DOF; ++i)
      {
        cur_servo_stt[i] = cur_stat;
      }

      stringstream ssbuffer;
      msgpack::packer<stringstream> pk(&ssbuffer);
      pk.pack_map(4);
      pk.pack(string("ID"));
      pk.pack(msqid);
      pk.pack(string("PreviousStat"));
      pk.pack(pre_servo_stt);
      pk.pack(string("CurrentStat"));
      pk.pack(cur_servo_stt);   
      pk.pack(string("Message"));
      pk.pack(msg);

      memcpy(cmbox.mtext, ssbuffer.str().data(), MSGSZ); 
      size_t buf_length = strlen(cmbox.mtext) + 1 ;

      if (msgsnd(msqidC, &cmbox, MSGSZ + 1, IPC_NOWAIT) < 0) {
         perror("msgsnd");
         return -1;
      }

      else 
      {
         msgpack::unpacked snt_msg;
         msgpack::unpack(&snt_msg, cmbox.mtext, MSGSZ);

         msgpack::object obj = snt_msg.get();
         cout << "[MX05_" << msqidC << "] > " << obj << endl;
      }

      //Wait for a new message from the central server
      //result = msgrcv( msqid, &msgp, length, 1, 0);
      
      //If the new message indicates all the processes have the same status
      //break the loop and print out the final temperature
   //}

   //Remove the mailbox
   status = msgctl(msqid, IPC_RMID, 0);

   //Validate nothing when wrong when trying to remove mailbox
   if(status != 0){
      printf("\nERROR closing mailbox\n");
   }
}