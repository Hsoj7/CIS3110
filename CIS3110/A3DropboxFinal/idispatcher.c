/*
Name: Joshua Stone
ID: 1013900
Mail: jstone04@uoguelph.ca
Assignment 3
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

//struct + function definitions
struct dispatcher{
  int processID;
  int arrive;
  char event;
  int timeStartedRunning;
  int timeStartedReady;
  int totalRunTime;
  int totalReadyTime;
  int totalBlockedTime;
  struct dispatcher *next;
};

struct resourceQueue1{
  int processID;
  int arrive;
  char event;
  int timeStartedRunning;
  int timeStartedReady;
  int totalRunTime;
  int totalReadyTime;
  int totalBlockedTime;
  struct resourceQueue1 *next;
};

void removeQueue(int arrive, int processID);
void insertQueue(int arrive, int processID);
void backToFront(int arrive);
void deleteNode(int processID, int arrive);
void insert(int arrive, char event, int processID);
void printR();
void print();

//declaring global values
struct dispatcher *head = NULL;
struct resourceQueue1 *front = NULL;
int zeroRunTime = 0;
int zeroArriveTime = 0;
int timesArr[500];
int timesArrCount = 0;

int main(){
  char *userString = NULL;
  char *temp = NULL;
  int i = 0, k = 0;
  int tableHolder = 0;
  int tableHolderPrev = 0;
  int numProcesses = 0, count = 0;
  int firstRun = 0;
  //linkedlist stuff
  int processID = 0;
  int arrive = 0;
  char event = '\0';

  //sets all values in the time array to -1, a bad value
  for(i = 0; i < 500; i++){
    timesArr[i] = -1;
  }
  //loops until the input is a new line or nothing
  while(1){
    char myArgv[5][100];
    //needed the copy of userString to fix memory leak issue
    char *userStringCopy = NULL;
    //mallocs memory and gets the string from stdin
    userString = malloc(20);
    fgets(userString, 20, stdin);
    //if end of the file we are testing with, break;
    if(strlen(userString) == 0){
      break;
    }
    if(userString[0] == '\n'){
      break;
    }
    //takes care of white space in the userString
    for(i = strlen(userString); i > 0; i--){
      if(isalpha(userString[i]) || isdigit(userString[i])) break;
      userString[i] = '\0';
    }
    userStringCopy = userString;
    //string separate the input based on white space
    k = 0;
    while( (temp = strsep(&userStringCopy, " ")) != NULL ){
      strcpy(myArgv[k], temp);
      k++;
    }
    free(userString);

    //makes sure the letters are upper case
    for(i = 0; i < strlen(myArgv[1]); i++){
      myArgv[1][i] = toupper(myArgv[1][i]);
    }
    //create process, calls insert function
    if(strcmp(myArgv[1], "C") == 0){
      arrive = atoi(myArgv[0]);
      event = myArgv[1][0];
      processID = atoi(myArgv[2]);
      numProcesses++;
      insert(arrive, event, processID);
    }
    //exit process, calls deleteNode function
    else if(strcmp(myArgv[1], "E") == 0){
      //do calculations for time running, time ready, etc.
      processID = atoi(myArgv[2]);
      deleteNode(processID, atoi(myArgv[0]));
    }
    //request resource queue, calls insertQueue function
    else if(strcmp(myArgv[1], "R") == 0){
        insertQueue(atoi(myArgv[0]), atoi(myArgv[3]));
    }
    //interrupt from resource queue, calls removeQueue
    else if(strcmp(myArgv[1], "I") == 0){
        removeQueue(atoi(myArgv[0]), atoi(myArgv[3]));
    }
    //timer interrupt, ca;;s backToFront function
    else if(strcmp(myArgv[1], "T") == 0){
      backToFront(atoi(myArgv[0]));
    }
    //else, could not find the character
    else{
      printf("Error, unrecognized character %s\n", myArgv[1]);
    }
  }
  //prints out the results found
  //it basically searches for the next process ID in increasing order
  //I have the processes write their values to timesArr when they exit, so the values are out of order
  printf("0 %d\n", zeroRunTime);
  while(count < numProcesses){
    if(firstRun == 0){
      tableHolder = 500;
      for(i = 0; i < 500; i++){
        if(timesArr[i] < tableHolder && timesArr[i] > 0){
          tableHolder = timesArr[i];
        }
      }
      for(i = 0; i < 500; i++){
        if(timesArr[i] == tableHolder){
          printf("%d %d %d %d\n", timesArr[i], timesArr[i+1], timesArr[i+2], timesArr[i+3]);
          tableHolderPrev = tableHolder;
          break;
        }
      }
      firstRun++;
    }
    else{
      tableHolder = 500;
      for(i = 0; i < 500; i++){
        if(timesArr[i] < tableHolder && timesArr[i] > tableHolderPrev){
          tableHolder = timesArr[i];
        }
      }
      for(i = 0; i < 500; i++){
        if(timesArr[i] == tableHolder){
          printf("%d %d %d %d\n", timesArr[i], timesArr[i+1], timesArr[i+2], timesArr[i+3]);
          tableHolderPrev = tableHolder;
        }
      }
    }
    count++;
  }

  free(userString);
  return 0;
}
//function removes a process from the resource queue
void removeQueue(int arrive, int processID){
  struct resourceQueue1 *Rptr = NULL;
  struct resourceQueue1 *RptrPrevious = NULL;
  struct resourceQueue1 *temp = NULL;
  struct dispatcher *Dptr = malloc(sizeof(struct dispatcher));
  int lengthList = 0;
  int loopcounter = 0;
  //initalizing pointers to the struct
  Rptr = front;
  RptrPrevious = Rptr;
  temp = front;
  //checks how long the list is
  while(temp != NULL){
    lengthList++;
    temp = temp->next;
  }
  //if there is nothing in head, we know process '0' is running
  if(head == NULL){
    zeroRunTime += (arrive - zeroArriveTime);
    zeroArriveTime = 0;
  }
  //if length of the list is 1, copy everything then 'front' which is the resource queue pointer
  //gets set to null because it is empty
  if(lengthList == 1){
    if(Rptr->processID == processID){
      //copy elements of Rptr back to first in Dptr
      Dptr->processID = Rptr->processID;
      Dptr->arrive = 0;
      Dptr->timeStartedRunning = 0;
      Dptr->event = Rptr->event;
      Dptr->totalRunTime = Rptr->totalRunTime;
      Dptr->totalReadyTime = Rptr->totalReadyTime;
      Dptr->totalBlockedTime += (arrive - Rptr->arrive);
      free(Rptr);
      front->next = NULL;
      front = NULL;

      Dptr->next = head;
      head = Dptr;
    }
  }
  //if length 2, the process could either be at the front or the back
  //this basically finds which spot its in and updates the list accordingly
  else if(lengthList == 2){
    //finds if it front or back
    while(Rptr != NULL && Rptr->processID != processID){
      RptrPrevious = Rptr;
      Rptr = Rptr->next;
      loopcounter++;
    }

    if(Rptr->processID == processID){
      //copy elements of Rptr back to first in Dptr
      Dptr->processID = Rptr->processID;
      Dptr->arrive = 0;
      Dptr->timeStartedRunning = 0;
      Dptr->event = Rptr->event;
      Dptr->totalRunTime = Rptr->totalRunTime;
      Dptr->totalReadyTime = Rptr->totalReadyTime;
      Dptr->totalBlockedTime += (arrive - Rptr->arrive);
    }
    //means the one we are looking for is at the back
    if(loopcounter == 0){
      front = Rptr->next;
      front->next = NULL;
      free(Rptr);
    }
    //else its at the front
    else if(loopcounter == 1){
      front = RptrPrevious;
      front->next = NULL;
      free(Rptr);
    }
    Dptr->next = head;
    head = Dptr;
  }
  else{
    //this means the list is length 3 or greater
    while(Rptr != NULL && Rptr->processID != processID){
      RptrPrevious = Rptr;
      Rptr = Rptr->next;
      loopcounter++;
    }
    if(Rptr->processID == processID){
      //copy elements of Rptr back to first in Dptr
      Dptr->processID = Rptr->processID;
      Dptr->arrive = 0;
      Dptr->timeStartedRunning = 0;
      Dptr->event = Rptr->event;
      Dptr->totalRunTime = Rptr->totalRunTime;
      Dptr->totalReadyTime = Rptr->totalReadyTime;
      Dptr->totalBlockedTime += (arrive - Rptr->arrive);

      RptrPrevious->next = Rptr->next;
      free(Rptr);
      Dptr->next = head;
      head = Dptr;
    }
  }
  //means there is only 1 in the queue, so the process starts running
  if(head->next == NULL){
    head->timeStartedRunning = arrive;
  }
  //menas there is more than one in the queue, so it becomes ready
  else{
    head->timeStartedReady = arrive;
  }
}
//function takes a process out of the ready queue and inserts it to the back of the resource queue
void insertQueue(int arrive, int processID){
  struct resourceQueue1 *Rptr = malloc(sizeof(struct resourceQueue1));
  struct dispatcher *Dptr = NULL;
  struct dispatcher *DptrPrevious = NULL;
  struct dispatcher *temp = head;
  int lengthList = 0;
  int loopcounter = 0;
  //initalizing pointers to dispatcher queue
  Dptr = head;
  DptrPrevious = Dptr;
  //finds list length of dispatcher
  while(temp != NULL){
    lengthList++;
    if(temp->next == NULL) break;
    temp = temp->next;
  }
  //if there is only 1 node, delete it from ready queue and insert it to resource queue
  if(lengthList == 1){
    //if the process was running or ready, increase the total time
    if(Dptr->timeStartedRunning > 0){
      Dptr->totalRunTime += (arrive - Dptr->timeStartedRunning);
      Dptr->timeStartedRunning = 0;
    }
    if(Dptr->timeStartedReady > 0){
      Dptr->totalReadyTime += (arrive - Dptr->timeStartedReady);
      Dptr->timeStartedReady = 0;
    }
    if(processID == Dptr->processID){
      Rptr->processID = Dptr->processID;
      Rptr->arrive = arrive;
      Rptr->timeStartedRunning = 0;;
      Rptr->event = Dptr->event;
      Rptr->totalRunTime = Dptr->totalRunTime;
      Rptr->totalReadyTime = Dptr->totalReadyTime;
      Rptr->totalBlockedTime = Dptr->totalBlockedTime;
      //head becomes empty
      head->next = NULL;
      head = NULL;

      free(Dptr);
      //puts the process to back of resource queue
      Rptr->next = front;
      front = Rptr;
    }
  }
  else if(lengthList == 2){
    //if list length is 2, the process could either be at the front or back
    while(Dptr != NULL && Dptr->processID != processID){
      DptrPrevious = Dptr;
      Dptr = Dptr->next;
      loopcounter++;
    }
    //calculates running/ready time
    if(Dptr->timeStartedRunning > 0){
      Dptr->totalRunTime += (arrive - Dptr->timeStartedRunning);
      Dptr->timeStartedRunning = 0;
    }
    if(Dptr->timeStartedReady > 0){
      Dptr->totalReadyTime += (arrive - Dptr->timeStartedReady);
      Dptr->timeStartedReady = 0;
    }
    if(processID == Dptr->processID){
      Rptr->processID = Dptr->processID;
      Rptr->arrive = arrive;
      Rptr->timeStartedRunning = 0;;
      Rptr->event = Dptr->event;
      Rptr->totalRunTime = Dptr->totalRunTime;
      Rptr->totalReadyTime = Dptr->totalReadyTime;
      Rptr->totalBlockedTime = Dptr->totalBlockedTime;
    }
    //means its at the back so it now becomes the front
    if(loopcounter == 0){
      head = Dptr->next;
      head->next = NULL;
      free(Dptr);
    }
    //else its at the front, we need to calculate ready time and it starts to run
    else if(loopcounter == 1){
      head = DptrPrevious;
      head->next = NULL;
      free(Dptr);
      if(head->timeStartedReady > 0){
        head->totalReadyTime += (arrive - head->timeStartedReady);
        head->timeStartedReady = 0;
      }
      head->timeStartedRunning = arrive;
    }
    //insert node in the resource queue to the back
    Rptr->next = front;
    front = Rptr;
  }
  else if(lengthList > 2){
    //runs if length of dispatcher queue is greater than 2
    loopcounter = 0;
    while(Dptr != NULL && Dptr->next != NULL && Dptr->processID != processID){
      DptrPrevious = Dptr;
      Dptr = Dptr->next;
      loopcounter++;
    }
    //caculates and upadtes times
    if(Dptr->timeStartedRunning > 0){
      //printf("arrive = %d, start running = %d\n", arrive, Dptr->);
      Dptr->totalRunTime += (arrive - Dptr->timeStartedRunning);
      Dptr->timeStartedRunning = 0;
    }
    if(Dptr->timeStartedReady > 0){
      Dptr->totalReadyTime += (arrive - Dptr->timeStartedReady);
      Dptr->timeStartedReady = 0;
    }
    if(processID == Dptr->processID){
      Rptr->processID = Dptr->processID;
      Rptr->arrive = arrive;
      Rptr->timeStartedRunning = 0;;
      Rptr->event = Dptr->event;
      Rptr->totalRunTime = Dptr->totalRunTime;
      Rptr->totalReadyTime = Dptr->totalReadyTime;
      Rptr->totalBlockedTime = Dptr->totalBlockedTime;
      //delete the node and set links
      DptrPrevious->next = Dptr->next;
      free(Dptr);
      //put the node to the back of resource queue
      Rptr->next = front;
      front = Rptr;
      //if its the currently running process, we need to update the times
      if(loopcounter + 1 == lengthList){
        if(DptrPrevious->timeStartedReady > 0){
          DptrPrevious->totalReadyTime += (arrive - DptrPrevious->timeStartedReady);
          DptrPrevious->timeStartedReady = 0;
        }
        DptrPrevious->timeStartedRunning = arrive;
      }
    }
  }
  //if length was one, the list is empty now, so 0 starts running
  if(lengthList == 1){
    zeroArriveTime = arrive;
  }
}
//function puts the running queue to the back of the list and starts running the next in line
void backToFront(int arrive){
  struct dispatcher *back; //= malloc(sizeof(struct dispatcher));
  struct dispatcher *beforeBack = NULL;

  back = head;
  //makes sure there is at least 2 nodes to swap
  if(head == NULL || head->next == NULL){

  }
  //puts the front to the back and pushes every other forward
  else{
    while(back->next != NULL){
      beforeBack = back;
      back = back->next;
    }
    //calculates the running time the node at the end of the list
    //before putting it to the front
    back->totalRunTime += (arrive - back->timeStartedRunning);
    back->timeStartedRunning = 0;
    back->timeStartedReady = arrive;
    beforeBack->totalReadyTime += (arrive - beforeBack->timeStartedReady);
    beforeBack->timeStartedReady = 0;
    beforeBack->timeStartedRunning = arrive;

    beforeBack->next = NULL;
    back->next = head;
    head = back;
  }
}
//function deletes the node from the ready queue
void deleteNode(int processID, int arrive){
  struct dispatcher *current = NULL;
  struct dispatcher *previous = NULL;
  struct dispatcher *temp = NULL;
  int lengthList = 0;
  int shiftCount = 0;
  //initalizing dispatcher pointers
  current = head;
  temp = head;
  //finds length of the list
  while(temp != NULL){
    lengthList++;
    temp = temp->next;
  }
  //if there is only one element, delete it and fully empty the queue
  if(lengthList == 1){
    if(current->processID == processID){
      if(current->timeStartedRunning > 0){
        current->totalRunTime += (arrive - current->timeStartedRunning);
        current->timeStartedRunning = 0;
      }
      if(current->timeStartedReady > 0){
        current->totalReadyTime += (arrive - current->timeStartedReady);
        current->timeStartedReady = 0;
      }
      //update times in the timesArray
      timesArr[timesArrCount] = current->processID;
      timesArrCount++;
      timesArr[timesArrCount] = current->totalRunTime;
      timesArrCount++;
      timesArr[timesArrCount] = current->totalReadyTime;
      timesArrCount++;
      timesArr[timesArrCount] = current->totalBlockedTime;
      timesArrCount++;
      //set everything back to zero before exiting.
      current->totalBlockedTime = 0;
      current->totalRunTime = 0;
      current->totalReadyTime = 0;
      current->processID = 0;

      free(current);
      head = NULL;
      //since the length was only 1, after deleting, zero starts running
      zeroArriveTime = arrive;
    }
  }
  else if(lengthList == 2){
    //if length 2, the node is either at the front or back
    if(current->processID == processID){
      if(current->timeStartedRunning > 0){
        current->totalRunTime += arrive - current->timeStartedRunning;
        current->timeStartedRunning = 0;
      }
      if(current->timeStartedReady > 0){
        current->totalReadyTime += (arrive - current->timeStartedReady);
        current->timeStartedReady = 0;
      }
      //update timesarr with values before deleting it
      timesArr[timesArrCount] = current->processID;
      timesArrCount++;
      timesArr[timesArrCount] = current->totalRunTime;
      timesArrCount++;
      timesArr[timesArrCount] = current->totalReadyTime;
      timesArrCount++;
      timesArr[timesArrCount] = current->totalBlockedTime;
      timesArrCount++;
      //set everything back to 0
      current->totalBlockedTime = 0;
      current->totalRunTime = 0;
      current->totalReadyTime = 0;
      current->processID = 0;
      //it was the element at the back, no shift was required so the
      //one at the front of the list stays running
      head = current->next;
      head->next = NULL;
      free(current);
    }
    else{
      //the node is at the front of the list
      previous = current;
      current = current->next;
      if(current->processID == processID){
        if(current->timeStartedRunning > 0){
          current->totalRunTime += arrive - current->timeStartedRunning;
          current->timeStartedRunning = 0;
        }
        if(current->timeStartedReady > 0){
          current->totalReadyTime += (arrive - current->timeStartedReady);
          current->timeStartedReady = 0;
        }
        //update times
        timesArr[timesArrCount] = current->processID;
        timesArrCount++;
        timesArr[timesArrCount] = current->totalRunTime;
        timesArrCount++;
        timesArr[timesArrCount] = current->totalReadyTime;
        timesArrCount++;
        timesArr[timesArrCount] = current->totalBlockedTime;
        timesArrCount++;
        //set everything back to 0
        current->totalBlockedTime = 0;
        current->totalRunTime = 0;
        current->totalReadyTime = 0;
        current->processID = 0;
        //the node was at the front, so the previous one updates its ready time and starts running
        head = previous;
        free(current);
        head->next = NULL;
        head->totalReadyTime += (arrive - head->timeStartedReady);
        head->timeStartedReady = 0;
        head->timeStartedRunning = arrive;
      }
    }
  }
  else{
    //length was greater than 2 and node was at the back
    if(current->processID == processID){
      if(current->timeStartedRunning > 0){
        current->totalRunTime += arrive - current->timeStartedRunning;
        current->timeStartedRunning = 0;
      }
      if(current->timeStartedReady > 0){
        current->totalReadyTime += (arrive - current->timeStartedReady);
        current->timeStartedReady = 0;
      }
      //then put all data into timesArr
      timesArr[timesArrCount] = current->processID;
      timesArrCount++;
      timesArr[timesArrCount] = current->totalRunTime;
      timesArrCount++;
      timesArr[timesArrCount] = current->totalReadyTime;
      timesArrCount++;
      timesArr[timesArrCount] = current->totalBlockedTime;
      timesArrCount++;
      //set everything back to 0
      current->totalBlockedTime = 0;
      current->totalRunTime = 0;
      current->totalReadyTime = 0;
      current->processID = 0;

      head = current->next;
      free(current);
    }
    else{
      //node was not at the back, so it finds it and increases shift count
      while(current != NULL && current->processID != processID){
        previous = current;
        current = current->next;
        shiftCount++;
      }
      if(current->processID == processID){
        if(current->timeStartedRunning > 0){
          current->totalRunTime += arrive - current->timeStartedRunning;
          current->timeStartedRunning = 0;
        }
        if(current->timeStartedReady > 0){
          current->totalReadyTime += (arrive - current->timeStartedReady);
          current->timeStartedReady = 0;
        }
        //then put all data into timesArr
        timesArr[timesArrCount] = current->processID;
        timesArrCount++;
        timesArr[timesArrCount] = current->totalRunTime;
        timesArrCount++;
        timesArr[timesArrCount] = current->totalReadyTime;
        timesArrCount++;
        timesArr[timesArrCount] = current->totalBlockedTime;
        timesArrCount++;
        //sets values back to 0
        current->totalBlockedTime = 0;
        current->totalRunTime = 0;
        current->totalReadyTime = 0;
        current->processID = 0;
        //updates the link
        previous->next = current->next;
        //if the node is at the front of the list, or running, update its times
        if(shiftCount+1 == lengthList){
          if(previous->timeStartedReady > 0){
            previous->totalReadyTime += (arrive - previous->timeStartedReady);
            previous->timeStartedReady = 0;
          }
          previous->timeStartedRunning = arrive;
        }
        free(current);
      }
    }
  }
}
//functions inserts a newly created node into the ready queue
void insert(int arrive, char event, int processID){
  struct dispatcher *ptr = malloc(sizeof(struct dispatcher));
  struct dispatcher *temp = NULL;
  int lengthList = 0;
  //initalizing everything in the struct
  ptr->arrive = arrive;
  ptr->event = event;
  ptr->processID = processID;
  ptr->totalRunTime = 0;
  ptr->totalReadyTime = 0;
  ptr->totalBlockedTime = 0;
  ptr->timeStartedRunning = 0;
  ptr->timeStartedReady = 0;

  //increment 0 run time by time of first command
  if(head == NULL){
    zeroRunTime += arrive - zeroArriveTime;
    zeroArriveTime = 0;
  }
  //puts the ptr to the back of the list
  ptr->next = head;
  head = ptr;
  //finds length of the list
  temp = head;
  while(temp != NULL){
    lengthList++;
    if(temp->next == NULL) break;
    temp = temp->next;
  }
  //if length is one, the process is running
  if(lengthList == 1){
    head->timeStartedRunning = arrive;
  }
  //otherwise it is waiting
  else if(lengthList > 1){
    head->timeStartedReady = arrive;
  }
}
//helper function to print the resource queue - used mainly for debugging
void printR(){
  struct resourceQueue1 *ptr = front;
  int whileStuck = 0;
  if(front != NULL){
    while(ptr != NULL){
      if(whileStuck > 30){
        printf("printRWhile Stuck\n");
        break;
      }
      printf("RESOURCE: PID %d, run %d, ready %d, block %d\n", ptr->processID, ptr->totalRunTime, ptr->totalReadyTime, ptr->totalBlockedTime);
      ptr = ptr->next;
    }
  }
  else{
    printf("RESOURCE: Empty\n");
  }
}
//helper function to print ready queue - used mainly for debugging
void print(){
  struct dispatcher *link = head;
  int whileStuck = 0;
  if(head != NULL){
    while(link != NULL){
      if(whileStuck > 30){
        printf("printWhile stuck\n");
        break;
      }
      if(link == NULL) break;
      printf("RUN/READY: PID %d, run %d, ready %d, block %d\n", link->processID, link->totalRunTime, link->totalReadyTime, link->totalBlockedTime);
      link = link->next;
      whileStuck++;
    }
  }
  else{
    printf("RUN/READY: Empty\n");
  }
}
