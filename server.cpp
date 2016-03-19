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
      cout << "[USAGE] ./server_start critical_status" << endl;
      return -1;
   }

   printf("\nStarting Server...\n");

   //Set up local variables
   int i,result,length,status;             //counter for loops
   int uid = 0;                               //central process ID
   int critStat = atoi(argv[1]);        //starting temperature
   int msqid[NUM_PROCESSES];       //mailbox IDs for all processes
   int tempAry[NUM_PROCESSES];   //array of process temperatures

   //Create the Central Servers Mailbox
   int msqidC = msgget(CENTRAL_MAILBOX, 0666 | IPC_CREAT);
   if (msqidC < 0)
      return -1;
   else
      cout << "[CMBOX ID]" << msqidC << endl;
   printf("\nSetting up the mailbox...\n");  
   //Create the mailboxes for the other processes and store their IDs

   for(i = 1; i <= NUM_PROCESSES; i++){
      msqid[(i-1)] = msgget((CENTRAL_MAILBOX + i), 0666 | IPC_CREAT);
      if (msqid[(i-1)] < 0) {
         cout << "[" << msqid[i-1] << "] NG..." << endl;
         return -1;
      }
      else
         cout << "[" << msqid[i-1] << "] OK..." << endl;
   }

   //Initialize the message to be sent
   msgp.mtype = 1;
   string msg = "N/A";
   vector <int> cur_servo_stt, pre_servo_stt;

   for (int i = 0; i < ROBOT_DOF; ++i)
   {
     pre_servo_stt.push_back(-1);
     cur_servo_stt.push_back(0);
   }

   while(1){

         result = msgrcv( msqidC, &cmbox, length, 1, 0);
         if (result < 0)
            return -1;

         msgpack::unpacked recv_msg;
         msgpack::unpack(&recv_msg, cmbox.mtext, MSGSZ);
         msgpack::object obj = recv_msg.get();

         cout << obj << endl;


         // cout << "[" << cmbox.pid << "]" << cmbox.status << endl;
         
         // if (cmbox.status == initTemp)
         // {
         //    msgp.stable = 0;
         // }
         // else
         // {
         //    msgp.stable = 1;
         // }

         // msgp.status = cmbox.status; 
         // result = msgsnd( msqid[0], &msgp, length, 0);

         // msgp.prev_stat = cmbox.status;


   }

   printf("\nShutting down Server...\n");

   //Remove the mailbox
   status = msgctl(msqidC, IPC_RMID, 0);

   //Validate nothing when wrong when trying to remove mailbox
   if(status != 0){
      printf("\nERROR closing mailbox\n");
   }
}