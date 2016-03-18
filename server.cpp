#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

using namespace std;
#define CENTRAL_MAILBOX 1200    //Central Mailbox number 
#define NUM_PROCESSES 1            //Total number of external processes

struct {
long priority;         //message priority
int prev_stat;
int status;             //temperature
int pid;                //process id
int stable;            //boolean for temperature stability
} msgp, cmbox;

//MAIN function
int main(int argc, char *argv[]) {

   //Validate that a temperature was given via the command line
   if(argc != 2) {
      printf("USAGE: Too few arguments --./central.out Temp\n");
      exit(0);
   }

   printf("\nStarting Server...\n");

   //Set up local variables
   int i,result,length,status;             //counter for loops
   int uid = 0;                               //central process ID
   int initTemp = atoi(argv[1]);        //starting temperature
   int msqid[NUM_PROCESSES];       //mailbox IDs for all processes
   int unstable = 1;          //boolean to denote status stability
   int tempAry[NUM_PROCESSES];   //array of process temperatures

   //Create the Central Servers Mailbox
   int msqidC = msgget(CENTRAL_MAILBOX, 0600 | IPC_CREAT);

   printf("\nSetting up the mailbox...\n");  
   //Create the mailboxes for the other processes and store their IDs
   for(i = 1; i <= NUM_PROCESSES; i++){
      msqid[(i-1)] = msgget((CENTRAL_MAILBOX + i), 0600 | IPC_CREAT);
      cout << msqid[i-1] << " ";
   }
   cout << endl;

   //Initialize the message to be sent
   msgp.priority = 1;
   msgp.pid = uid;
   msgp.status = initTemp;
   msgp.prev_stat = 0;

   /* The length is essentially the size 
       of the structure minus sizeof(mtype) */
   length = sizeof(msgp) - sizeof(long);

   //While the processes have different temperatures

   while(1){
         result = msgrcv( msqidC, &cmbox, length, 1, 0);

         cout << "[" << cmbox.pid << "]" << cmbox.status << endl;
         
         if (cmbox.status == initTemp)
         {
            msgp.stable = 0;
         }
         else
         {
            msgp.stable = 1;
         }

         msgp.status = cmbox.status; 
         result = msgsnd( msqid[0], &msgp, length, 0);

         msgp.prev_stat = cmbox.status;


   }

   printf("\nShutting down Server...\n");

   //Remove the mailbox
   status = msgctl(msqidC, IPC_RMID, 0);

   //Validate nothing when wrong when trying to remove mailbox
   if(status != 0){
      printf("\nERROR closing mailbox\n");
   }
}