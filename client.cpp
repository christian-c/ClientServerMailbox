#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

using namespace std;
#define CENTRAL_MAILBOX 1200    //Central Mailbox number

struct {
long priority;        //message priority
int prev_stat;
int status;            //temperature
int pid;               //process id
int stable;          //boolean for temperature stability
} msgp, cmbox;

//MAIN function
int main(int argc, char *argv[]) {

   /* Validate that a temperature and a Unique 
       process ID was given via the command  */
   if(argc != 2) {
      printf("USAGE: Too few arguments --./central.out Temp UID");
      exit(0);
   }

   //Setup local variables
   int unstable = 1;
   int result, length, status;
   int uid = atoi(argv[1]);

   //Create the Central Servers Mailbox
   int msqidC = msgget(CENTRAL_MAILBOX, 0600 | IPC_CREAT);

   //Create the mailbox for this process and store it's IDs
   int msqid = msgget((CENTRAL_MAILBOX + uid), 0600 | IPC_CREAT);

   //Initialize the message to be sent
   cmbox.priority = 1;
   cmbox.pid = uid;
   cmbox.stable = 1;
   cmbox.prev_stat = 0;

   /* The length is essentially the size of 
       the structure minus sizeof(mtype) */
   length = sizeof(msgp) - sizeof(long);

   //While all the processes have different temps
   while(1){

      cin >> cmbox.status;
      //Send the current status to the central server
      result = msgsnd( msqidC, &cmbox, length, 0);

      //Wait for a new message from the central server
      result = msgrcv( msqid, &msgp, length, 1, 0);
      
      cout << "[" << msgp.pid << "]" << msgp.status << " " << msgp.prev_stat;

      if (msgp.status != msgp.prev_stat && msgp.status == 18)
      {
         cout << " [OK]" << endl;
      }
      else
      {
         cout << " [NG]" << endl;
      }

      //If the new message indicates all the processes have the same status
      //break the loop and print out the final temperature
   }

   //Remove the mailbox
   status = msgctl(msqid, IPC_RMID, 0);

   //Validate nothing when wrong when trying to remove mailbox
   if(status != 0){
      printf("\nERROR closing mailbox\n");
   }
}