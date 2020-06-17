#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <ctype.h>
#include <signal.h>

void greatestCommonDenom(int first, int second);
char *trim(char *userInput);
int hexToDec(char num[]);
void killChild(int signalll);

int main(int argc, char *argv[], char *envp[]){
  char *userInput = NULL;
  char stop[20] = "exit";
  char args[5] = "args";
  char gcd[100] = "gcd";
  char englishName[100] = "englishName";
  char readFile[100];
  char tempGCD[100];
  struct passwd *p;
  uid_t uid;
  char userID[20];
  char hostName[100] = "\0";
  char *found = NULL;
  int i = 0, j = 0, k = 0;
  int firstGCD = 0, secondGCD = 0;
  int nameLength = 0;
  int printLength = 0;
  int foundName = 0;
  int pid;
  int flag = 0;
  int quotes = 0;
  int execReturn = 0;
  int goodInput = 1;
  char sudo;
  FILE *fp;
  FILE *fp2;
  char *fileName = NULL;
  char dot = '.';

  int GCDinvalidChar1 = 0;
  int GCDinvalidChar2 = 0;

  char playerName[20];
  char playerInput[200];
  char help[300] = "Type: north - to move north\nType: south - to move south\nType: east - to move east\nType: west - to move west\nType: pick up - to pick up and item\nType: use - to use an item\n";
  int position = 0;
  int key = 0;
  int firstTimepos4 = 0;
  int lanturn = 0;
  int map = 0;
  int lit = 0;
  int tryKey = 0;


  /*Gets user ID. Found help with this function online @https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.bpxbd00/rtgtui.htm*/
  if((p = getpwuid(uid = getuid())) == NULL ){
    printf("Error, could not find userID\n");
  }
  strcpy(userID, p->pw_name);
  gethostname(hostName, 100);

  if(geteuid() == 0){
    sudo = '#';
  }
  else{
    sudo = '$';
  }

  /*loop forever*/
  while(1){
/* Wait for input */
  userInput = malloc(100);
  found = malloc(100);
  printf("%s@%s%c ", userID, hostName, sudo);
  fgets(userInput, 100, stdin);

  /*Parse input and throw it into argv*/
  k = 0;
  while( (found = strsep(&userInput," ")) != NULL){
    trim(found);
    argv[k] = found;
    k++;
  }
  argv[k] = NULL;
  free(userInput);
  free(found);

  /*exits the program*/
  if( strcmp(argv[0], stop) == 0 ){
    exit(0);
  }

  /*Outputting to a file*/
  else if ( (argv[0] != NULL && strcmp(argv[0], ">") == 0) || (argv[1] != NULL && strcmp(argv[1], ">") == 0) || (argv[2] != NULL && strcmp(argv[2], ">") == 0) || (argv[3] != NULL && strcmp(argv[3], ">") == 0) ){ //fix this line
    int signSpot = 0;

    /*find which argv[i] has the file to redirect to*/
    for( i = 0; i < k; i++){
      if( strrchr(argv[i], dot) != NULL ){
        fileName = argv[i];
      }
    }

    for( i = 0; i < k; i++ ){
      signSpot++;
      if(strcmp(argv[i], ">") == 0){
        break;
      }
    }

    signSpot -= 1;

    for( i = k - 1; i >= signSpot; i-- ){
      argv[i] = NULL;
    }

    pid = fork();

    if(pid < 0){
      fprintf(stderr, "Usage: fork failed\n");
    }
    else if(pid == 0){
      fp2 = freopen(fileName, "w", stdout);
      execvp(argv[0], argv);
      fclose(fp2);
      exit(1);
    }
    else{
      wait(NULL);
    }
  }

  /*Reading from a file*/
  else if ( (argv[0] != NULL && strcmp(argv[0], "<") == 0) || (argv[1] != NULL && strcmp(argv[1], "<") == 0) || (argv[2] != NULL && strcmp(argv[2], "<") == 0) || (argv[3] != NULL && strcmp(argv[3], "<") == 0) ){ //fix this line
    for( i = 0; i < k; i++ ){
      if( strrchr(argv[i], dot) != NULL ){
        fileName = argv[i];
      }
    }

    for( i = 0; i < k; i++ ){
      if(strncmp(argv[i], "-", 1) == 0){
        flag = 1;
      }
    }

    if(flag == 0){
      pid = fork();

      if(pid < 0){
        fprintf(stderr, "Usage: fork failed\n");
      }
      else if(pid == 0){
        //close(1);
        fp = freopen(fileName, "r", stdin);
        execlp(argv[0], argv[0], (char *)NULL);
        fclose(fp);
        exit(1);
      }
      else{
        wait(NULL);
      }
    }
    else if(flag == 1){
      pid = fork();

      if(pid < 0){
        fprintf(stderr, "Usage: fork failed\n");
      }
      else if(pid == 0){
      //  close(1);
        fp = freopen(fileName, "r", stdin);
        execlp(argv[0], argv[0], argv[1], (char *)NULL);
        perror("execlp");
        exit(1);
      }
      else{
        wait(NULL);
      }
    }

  }

  /*kind of working background process function*/
  else if( strcmp(argv[k-1], "&") == 0 ){
    argv[k-1] = NULL;

    signal(SIGCHLD, killChild);

    pid = fork();

    if(pid < 0){
      fprintf(stderr, "Usage: fork failed\n");
    }
    else if(pid == 0){
      //use ps -a
      execReturn = execvp(argv[0], argv);
      if( execReturn < 0 ){
        fprintf(stderr, "Usage: %s not found\n", argv[0]);
        exit(1);
      }
      exit(1);

    }
    else{

    }
    execReturn = 0;
  }

  /*print args function*/
  else if(strcmp(argv[0], args) == 0){
    goodInput = 1;

    /*Checks for bad input*/
    for(i = 0; i < k; i++){
      for(j = 0; j < strlen(argv[i]); j++){
        if( isdigit(argv[i][j]) || isalpha(argv[i][j]) || argv[i][j] == '"' ){

        }
        else{
          fprintf(stderr, "Usage: Argument %s found incorrect data\n", argv[i]);
          goodInput = 0;
        }
      }
    }

    /*Checks for quotes and prints according*/
    if(goodInput){
      for(i = 0; i < k; i++){
        for(j = 0; j < strlen(argv[i]); j++){
          if(argv[i][j] == '"'){
            quotes++;
          }
        }
      }
      if(quotes % 2 == 0){
        quotes /= 2;
      }
      else if(quotes % 2 == 1){
        quotes -= 1;
        quotes /= 2;
      }
      printf("Argc = %d, Argv = ", k - 1 - quotes);
      for(i = 1; i < k; i++){
        if(k - i != 1 && argv[i][0] == '"'){
          printf("%s ", argv[i]);
        }
        else if(k - i == 1 && argv[i][strlen(argv[i]) - 1] == '"'){
          printf("%s", argv[i]);
        }
        else if(k - i != 1 ){
          printf("%s, ", argv[i]);
        }
        else{
          printf("%s", argv[i]);
        }
      }
      printf("\n");
    }
    else{
      goodInput = 1;
    }
  }

  /*fix if letters and make sure there is nothing bigger than 2 billion and fix hex numbers if like 0xfffffffffff*/
  else if(strcmp(argv[0], gcd) == 0){
    if(k != 3){
      fprintf(stderr, "Usage: Incorrect number of arguments\n");
    }
    else{
      GCDinvalidChar1 = 0;
      GCDinvalidChar2 = 0;
      //making sure the letters are lower case and checks if they are symbols
      for(i = 0; i < strlen(argv[1]); i++){
        if(isalpha(argv[1][i])){
          argv[1][i] = tolower(argv[1][i]);
        }
        //checks to make sure numbers or letters for first number
        if( isalpha(argv[1][i]) || isdigit(argv[1][i]) ){
        }
        else{
          GCDinvalidChar1 = 1;
        }
      }
      for(i = 0; i < strlen(argv[2]); i++){
        if(isalpha(argv[2][i])){
          argv[2][i] = tolower(argv[2][i]);
        }
        //checks to make sure numbers or letters for first number
        if( isalpha(argv[2][i]) || isdigit(argv[2][i]) ){
        }
        else{
          GCDinvalidChar2 = 1;
        }
      }
      /*Error checking of first number*/
      for(i = 2; i < strlen(argv[1]); i++){
        if(argv[1][0] == '0' && argv[1][1] == 'x'){
          if( isalpha(argv[1][i]) ){
            if( argv[1][i] == 'a' || argv[1][i] == 'b' || argv[1][i] == 'c' || argv[1][i] == 'd' || argv[1][i] == 'e' || argv[1][i] == 'f' ){
            }
            else{
              GCDinvalidChar1 = 1;
            }
          }
        }
      }
      /*Makes sure if not a hex number, there are no letters for first number*/
      if( argv[1][0] == '0' && argv[1][1] == 'x' ){
      }
      else{
        for(i = 0; i < strlen(argv[1]); i++){
          if( isdigit(argv[1][i]) == 0){
            GCDinvalidChar1 = 1;
          }
        }
      }

      /*Checks if number is too large*/

      //Error checking of the second number
      for(i = 2; i < strlen(argv[2]); i++){
        if(argv[2][0] == '0' && argv[2][1] == 'x'){
          if( isalpha(argv[2][i]) ){
            if( argv[2][i] == 'a' || argv[2][i] == 'b' || argv[2][i] == 'c' || argv[2][i] == 'd' || argv[2][i] == 'e' || argv[2][i] == 'f' ){
            }
            else{
              GCDinvalidChar2 = 1;
            }
          }
        }
      }
      /*Makes sure if not a hex number, there are no letters for second number*/
      if( argv[2][0] == '0' && argv[2][1] == 'x' ){
      }
      else{
        for(i = 0; i < strlen(argv[2]); i++){
          if( isdigit(argv[2][i]) == 0 ){
            GCDinvalidChar2 = 1;
          }
        }
      }

      if(GCDinvalidChar1 == 1){
        fprintf(stderr, "Usage: Argument 1, %s found incorrect data\n", argv[1]);
      }
      else if(GCDinvalidChar1 != 1){
        /*Determine if first number is hex*/
        if(argv[1][1] == 'x' && argv[1][0] == '0'){
          for(i = 0; i < strlen(argv[1]); i++){
            tempGCD[i] = argv[1][i + 2];
          }
          firstGCD = hexToDec(tempGCD);
        }
        else{
          firstGCD = atoi(argv[1]);
        }
      }

      if(GCDinvalidChar2 == 1){
        fprintf(stderr, "Usage: Argument 2, %s found incorrect data\n", argv[2]);
      }
      else if(GCDinvalidChar2 != 1){
        /*Determine if second number is hex*/
        if(argv[2][1] == 'x' && argv[2][0] == '0'){
          for(i = 0; i < strlen(argv[2]); i++){
            tempGCD[i] = argv[2][i + 2];
          }
          secondGCD = hexToDec(tempGCD);
        }
        else{
          secondGCD = atoi(argv[2]);
        }
      }

      /*Find the gcd of our new numbers*/
      if(GCDinvalidChar1 == 0 && GCDinvalidChar2 == 0){
        printf("GCD(%s, %s) = ", argv[1], argv[2]);
        greatestCommonDenom(firstGCD, secondGCD);
      }

      GCDinvalidChar1 = 0;
      GCDinvalidChar2 = 0;

    }
  }

  /*My own command, print the meaning of an english name*/
  else if(strcmp(argv[0], englishName) == 0){
    if(k != 2){
      fprintf(stderr, "Usage: Incorrect number of arguments. Arguments should be [englishName] [NAME]\n");
    }
    else{
      fp = fopen("names.txt", "r");
      if(fp == NULL) fprintf(stderr, "Usage: Could not open file\n");
      else{
        char c;
        nameLength = strlen(argv[1]);
        if(argv[1][0] < 65 || argv[1][0] > 90){
          c = toupper(argv[1][0]);
          argv[1][0] = c;
        }

        /*Name website: //http://www.world-english.org/boys_names-v-z.htm*/
        while(fgets(readFile, 100, fp)){
          if(strncmp(argv[1], readFile, nameLength) == 0){
            printLength = strlen(readFile) - nameLength;
            printf("The name %s means:%.*s\n", argv[1], printLength - 1, readFile + nameLength);
            foundName = 1;
          }
        }
        if(foundName != 1){
          fprintf(stderr, "Usage: Entered name is not an old english name\n");
          foundName = 0;
        }
        foundName = 0;
      }
      fclose(fp);
    }
  }

  /*I know this is messy, but it is all the executables in the bin folder. Couldnt make it work any other way*/
  else if( strcmp(argv[0], "ls") == 0 || strcmp(argv[0], "pwd") == 0 || strcmp(argv[0], "cat") == 0 || strcmp(argv[0], "clear") == 0 || strcmp(argv[0], "busybox") == 0 || strcmp(argv[0], "chgrp") == 0 || strcmp(argv[0], "netstat") == 0 || strcmp(argv[0], "su") == 0 || strcmp(argv[0], "bash") == 0 || strcmp(argv[0], "chmod") == 0 || strcmp(argv[0], "cp") == 0 || strcmp(argv[0], "chown") == 0 || strcmp(argv[0], "csh") == 0 || strcmp(argv[0], "dash") == 0 || strcmp(argv[0], "date") == 0 || strcmp(argv[0], "dir") == 0 || strcmp(argv[0], "echo") == 0 || strcmp(argv[0], "false") == 0 || strcmp(argv[0], "grep") == 0 || strcmp(argv[0], "fuser") == 0 || strcmp(argv[0], "hostname") == 0 || strcmp(argv[0], "ip") == 0 || strcmp(argv[0], "kmod") == 0 || strcmp(argv[0], "less") == 0 || strcmp(argv[0], "lessecho") == 0 || strcmp(argv[0], "lessfile") == 0 || strcmp(argv[0], "lesskey") == 0 || strcmp(argv[0], "lesspipe") == 0 || strcmp(argv[0], "ln") == 0 || strcmp(argv[0], "loadkeys") == 0 || strcmp(argv[0], "login") == 0 || strcmp(argv[0], "lsblk") == 0 || strcmp(argv[0], "lsmod") == 0 || strcmp(argv[0], "mkdir") == 0 || strcmp(argv[0], "more") == 0 || strcmp(argv[0], "mount") == 0 || strcmp(argv[0], "mountpoint") == 0 || strcmp(argv[0], "mt") == 0 || strcmp(argv[0], "mv") == 0 || strcmp(argv[0], "nano") == 0 || strcmp(argv[0], "nc") == 0 || strcmp(argv[0], "netcat") == 0 || strcmp(argv[0], "open") == 0 || strcmp(argv[0], "ping") == 0 || strcmp(argv[0], "ps") == 0 || strcmp(argv[0], "readlink") == 0 || strcmp(argv[0], "red") == 0 || strcmp(argv[0], "rm") == 0 || strcmp(argv[0], "rmdir") == 0 || strcmp(argv[0], "rnano") == 0 || strcmp(argv[0], "run-parts") == 0 || strcmp(argv[0], "sed") == 0 || strcmp(argv[0], "setfont") == 0 || strcmp(argv[0], "setupcon") == 0 || strcmp(argv[0], "sh") == 0 || strcmp(argv[0], "sleep") == 0 || strcmp(argv[0], "ss") == 0 || strcmp(argv[0], "stty") == 0 || strcmp(argv[0], "su") == 0 || strcmp(argv[0], "sync") == 0 || strcmp(argv[0], "systemd") == 0 || strcmp(argv[0], "talif") == 0 || strcmp(argv[0], "tar") == 0 || strcmp(argv[0], "tcsh") == 0 || strcmp(argv[0], "tempfile") == 0 || strcmp(argv[0], "touch") == 0 || strcmp(argv[0], "true") == 0 || strcmp(argv[0], "umount") == 0 || strcmp(argv[0], "uname") == 0 || strcmp(argv[0], "vdir") == 0 || strcmp(argv[0], "which") == 0 || strcmp(argv[0], "zcat") == 0 || strcmp(argv[0], "zcmp") == 0 || strcmp(argv[0], "zdiff") == 0 || strcmp(argv[0], "zegrep") == 0 || strcmp(argv[0], "zfgrep") == 0 || strcmp(argv[0], "zforce") == 0 || strcmp(argv[0], "zgrep") == 0 || strcmp(argv[0], "zless") == 0 || strcmp(argv[0], "zmore") == 0 || strcmp(argv[0], "znew") == 0 || strcmp(argv[0], "xeyes") == 0 ){

    pid = fork();

    if(pid < 0){
      fprintf(stderr, "Error, fork failed\n");
      exit(1);
    }
    else if(pid == 0){
      execvp(argv[0], argv);
      fprintf(stderr, "Couldn't be executed\n");
      exit(1);
    }
    else{
      wait(NULL);
    }

  }

  /*This is my game that I put wayyyy too much time into and hope you have fun playing :)*/
  else if( strcmp(argv[0], "jork") == 0 ){
    if(argv[1] != NULL){
      printf("Can't start game, type \"jork\" to start\n");
    }
    else{
      pid = fork();
      if(pid < 0){
        printf("Couldn't start game\n");
      }
      else if(pid == 0){
        execvp("clear", argv);
        exit(1);
      }
      else{
        wait(NULL);
      }

      printf("*CLANG*\n");
      sleep(2);
      printf("*CLANG*\n");
      sleep(2);
      printf("*CLANG*\n");
      sleep(2);
      printf("Stranger> Hey! You're awake!\n");
      sleep(3);
      printf("Stranger> Thank god! Do you remember anything?\n");
      sleep(1);
      printf("You> ");
      fgets(playerInput, 200, stdin);
      sleep(2);
      printf("Stranger> What is your name?\n");
      sleep(1);
      printf("You> ");
      fgets(playerName, 20, stdin);
      for(i = 0; i < 20; i++){
        if(playerName[i] == '\n'){
          playerName[i] = '\0';
        }
      }
      sleep(2);
      printf("Stranger> Well %s, to my knowledge we are trapped in a maze\n", playerName);
      sleep(2);
      printf("Stranger> I did some exploring before you woke up but couldn't find a way out!\n");
      sleep(4);
      printf("*You hear a mysterious voice*\nGame Master> Hello %s, hello Alfie\n", playerName);
      sleep(3);
      printf("Game Master> I put you two here\n");
      sleep(2);
      printf("*Mysterious Laugh*\n");
      sleep(2);
      printf("Game Master> Your goal is to escape, or else... Die\n");
      sleep(2);
      printf("Game Master> Here are your controls\n%s", help);
      sleep(4);
      printf("Game Master> Are you ready?\n");
      printf("%s> ", playerName);
      fgets(playerInput, 200, stdin);
      for(i = 0; i < 20; i++){
        if(playerInput[i] == '\n'){
          playerInput[i] = '\0';
        }
      }
      if( strcmp(playerInput, "yes") == 0 || strcmp(playerInput, "Yes") == 0 || strcmp(playerInput, "si") == 0 || strcmp(playerInput, "oui") == 0 ){
        sleep(2);
        printf("Game Master> Alright then, good luck\n");
        sleep(2);
        printf("*Mysterious Laugh*\n");
        sleep(2);
      }
      else{
        sleep(2);
        printf("Game Master> Well, too bad\n");
        sleep(2);
        printf("*Mysterious Laugh*\n");
        sleep(2);
      }

      printf("*Alfie looks at you in fear*\n");
      sleep(2);
      printf("Alfie> Well, you heard him. Let's get out of here!\n");
      sleep(2);
      printf("*Press ENTER to continue*\n");
      while(strcmp(playerInput, "\n") != 0){
        fgets(playerInput, 200, stdin);
      }
      playerInput[0] = '\0';

      pid = fork();
      if(pid < 0){
        printf("Couldn't continue\n");
        exit(-1);
      }
      else if(pid == 0){
        execvp("clear", argv);
        exit(1);
      }
      else{
        wait(NULL);
      }

      //Game loop
      int firstTime = 0;
      while(1){
        if(position == 0){ //Starting room
          sleep(1);
          printf("*You are standing in a dimly lit room*\n");
          sleep(2);
          printf("*There is a door to your north*\n");
          if(firstTime == 0){
            sleep(2);
            printf("*Remember, type: \'north\', \'south\', \'east\', \'west\' to move & \'pick up\', or \'use\' to interact with items*\n");
            sleep(2);
            printf("*If you see the prompt \'%s>\' you can type commands*\n", playerName);
            sleep(2);
            printf("*Make sure to read the prompts carefully. There is often important information*\n");
            sleep(2);
            firstTime = 1;
          }
          while(1){
            printf("%s> ", playerName);
            fgets(playerInput, 200, stdin);
            if(strcmp(playerInput, "north\n") == 0){
              sleep(1);
              printf("*You open the door*\n");
              position = 1;
              break;
            }
            else if(strcmp(playerInput, "east\n") == 0 || strcmp(playerInput, "west\n") == 0 || strcmp(playerInput, "south\n") == 0){
              sleep(1);
              printf("*Can't move, there is a wall*\n");
            }
            else if(strcmp(playerInput, "pick up\n") == 0 || strcmp(playerInput, "use\n") == 0 ){
              sleep(1);
              printf("*No nearby items*\n");
            }
            else{
              printf("*Command not found*\n");
            }
          }

        }
        if(position == 1){ //In the hallway
          sleep(2);
          printf("*You are in a hallway*\n");
          sleep(2);
          printf("Alfie> We can either move east or west, I'm following you!\n");
          while(1){
            printf("%s> ", playerName);
            fgets(playerInput, 200, stdin);
            if( strcmp(playerInput, "east\n") == 0){
              sleep(2);
              printf("*You and Alfie move east, hit a corner and turn north*\n");
              position = 3;
              break;
            }
            else if( strcmp(playerInput, "west\n") == 0 ){
              sleep(2);
              printf("*You and Alfie move west*\n");
              sleep(2);
              printf("*You reach a corner and turn north*\n");
              position = 2;
              break;
            }
            else if( strcmp(playerInput, "south\n") == 0){
              sleep(2);
              printf("*You turn around and enter the room again*\n");
              position = 0;
              break;
            }
            else if( strcmp(playerInput, "north\n") == 0 ){
              sleep(1);
              printf("*Can't move, there is a wall*\n");
            }
            else{
              printf("*Command not found*\n");
            }
          }
        }


        if(position == 2){ //moved west from the starting hallway
          sleep(2);
          printf("Alfie> There is a door to the west! Do you want to enter or continue north or south down the hallway?\n");
          while(1){
            printf("%s> ", playerName);
            fgets(playerInput, 200, stdin);
            if( strcmp(playerInput, "west\n") == 0){
              sleep(1);
              printf("*You open the door*\n");
              position = 4;
              break;
            }
            else if( strcmp(playerInput, "north\n") == 0 ){
              sleep(1);
              printf("*You and Alfie move down the hall north, hit a corner and turn east*\n");
              position = 5;
              break;
            }
            else if( strcmp(playerInput, "south\n") == 0){
              sleep(1);
              printf("*You turn around and head towards the room you started in, hit a corner and turn east*\n");
              position = 1;
              break;
            }
            else if( strcmp(playerInput, "east\n") == 0 ){
              sleep(1);
              printf("*Can't move, there is a wall*\n");
            }
            else{
              printf("*Command not found*\n");
            }
          }

        }

        if(position == 3){ //To the east out of the first room
          sleep(2);
          printf("*There is a door to your east and the hallway goes north and south*\n");
            while(1){
              sleep(2);
              printf("%s> ", playerName);
              fgets(playerInput, 200, stdin);
              if( strcmp(playerInput, "north\n") == 0 ){
                sleep(2);
                printf("*You continue down the hall north, hit a corner and turn west*\n");
                position = 5;
                break;
              }
              else if( strcmp(playerInput, "south\n") == 0){
                sleep(2);
                printf("*You walk south towards the room you woke up in*\n");
                position = 1;
                break;
              }
              else if( strcmp(playerInput, "west\n") == 0){
                sleep(2);
                printf("*Can't move, there is a wall\n");
              }
              else if( strcmp(playerInput, "east\n") == 0 ){
                sleep(2);
                printf("*You open the door*\n");
                position = 6;
                break;
              }
              else{
                printf("*Command not found*\n");
              }
            }
          }

        if(position == 4){ //room on the left side with a lanturn
          if(firstTimepos4 == 0){
            sleep(1);
            printf("Alfie> Look, there is a lanturn. We should take it\n");
            firstTimepos4 = 1;
          }
          else{
            sleep(1);
            printf("Alfie> We've already been here, looks like an empty room\n");
          }
            sleep(2);
            while(1){
              printf("%s> ", playerName);
              fgets(playerInput, 200, stdin);
              if( strcmp(playerInput, "pick up\n") == 0){
                sleep(2);
                printf("*You pick up the lanturn*\n");
                sleep(2);
                printf("*You can now see in dark rooms*\n");
                lanturn = 1;
                firstTimepos4 = 1;
              }
              else if( strcmp(playerInput, "north\n") == 0 ){
                sleep(1);
                printf("*Can't move, there is a wall*\n");
              }
              else if( strcmp(playerInput, "south\n") == 0){
                sleep(1);
                printf("*Can't move, there is a wall*\n");
              }
              else if( strcmp(playerInput, "west\n") == 0){
                sleep(1);
                printf("*Can't move, there is a wall*\n");
              }
              else if( strcmp(playerInput, "east\n") == 0 ){
                sleep(1);
                if(lanturn == 1){
                  printf("*You exit back into the hallway, carrying the lanturn*\n");
                }
                else{
                  printf("*You exit back into the hallway*\n");
                }
                position = 2;
                break;
              }
              else{
                printf("*Command not found*\n");
              }
            }
          }


        if(position == 5){ //At the top of the square, souuth is locked door, north is door with bed
          if(map == 0){
            sleep(2);
            printf("*Alife spots a picture on the wall* \n");
            sleep(2);
            printf("Alfie> Look %s, this looks like a map!\n", playerName);
            sleep(2);
            printf("*You both look at the map*\n");
            sleep(2);
            printf("     *     \n ----$----\n |   *   |\n |       |\n*-       -*\n |       |\n |       |\n ----|----\n     *      \n");
            sleep(2);
            printf("'*' represents a room and '-' or '|' represent hallways\n");
            sleep(3);
            printf("*You are standing at the '$' sign*\n");
            sleep(3);
            printf("*Remember, you started in the southern most room*\n");
            map = 1;
          }
          else if(map == 2){
            sleep(2);
            printf("*Alfie looks at the map again*\n");
            sleep(2);
            printf("     *     \n ----$----\n |   *   |\n |       |\n*-       -*\n |       |\n |       |\n ----|----\n     *      \n");
            map = 3;
          }
          map += 1;
            while(1){
              sleep(2);
              if(tryKey == 1 && key == 1){
                printf("*There is a door to your north and a door to your south, hallways to east and west. You can try using your key on the southern door*\n");
              }
              else{
                printf("*There is a door to your north and a door to your south, hallways to east and west*\n");
              }
              printf("%s> ", playerName);
              fgets(playerInput, 200, stdin);
              if( strcmp(playerInput, "north\n") == 0 ){
                sleep(2);
                printf("*You open the door to the north*\n");
                position = 7;
                break;
              }
              else if( strcmp(playerInput, "south\n") == 0){
                sleep(2);
                printf("*You try to open the door, but it's locked*\n");
                if(key == 1){
                  sleep(2);
                  printf("Alfie> Try using the key on the door!\n");
                  tryKey = 1;
                }
                else{
                  sleep(2);
                  printf("%s> I need to find the key\n", playerName);
                }
              }
              else if( strcmp(playerInput, "use\n") == 0 && key == 1){
                sleep(2);
                printf("*You put the key in the lock*\n");
                sleep(2);
                printf("*The door opens and you see a staircase going up*\n");
                sleep(2);
                printf("Alfie> Come on! This must be the way out!\n");
                sleep(2);
                printf("*You follow Alfie up the stairs*\n");
                position = 8;
                break;
              }
              else if( strcmp(playerInput, "use\n") == 0 && tryKey == 0){
                sleep(2);
                printf("*Could not use an item*\n");
              }
              else if( strcmp(playerInput, "west\n") == 0){
                sleep(2);
                printf("*You move down the hall west, hit a corner and turn south*\n");
                position = 2;
                break;
              }
              else if( strcmp(playerInput, "east\n") == 0 ){
                sleep(2);
                printf("*You move down the hall east, hit a corner and turn south*\n");
                position = 3;
                break;
              }
              else{
                printf("*Command not found*\n");
              }
            }
          }

          if(position == 7){ //In the furthest north room
            sleep(2);
            printf("*You walk into the room*\n");
            sleep(2);
            printf("Alfie> It looks like this room is empty\n");
            sleep(2);
            printf("Alife> I guess let's go back and keep looking around\n");
            sleep(3);
            printf("*Suddenly, a trap door opens and you both fall into a pit*\n");
            sleep(3);
            printf("Game Master> *Mysterious laugh*\n");
            sleep(3);
            printf("*There is a tight tunnel that leads south*\n");
            sleep(3);
            printf("*One by one, you and Alife crawl through*\n");
            sleep(3);
            printf("*You reach a ladder and climb up it*\n");
            sleep(3);
            printf("Alfie> Hey this is the room we started in!\n");
            sleep(2);
            printf("Game master> *Mysterious laugh*\n");
            position = 0;
            }


            if(position == 6){ //In the furthest east room
              sleep(2);
              printf("*You walk into the room but it is pitch black!*\n");
              if(lanturn == 0){
                sleep(2);
                printf("Alfie> Hmm, we might need to find a light source and come back\n");
              }
              else if(lanturn == 1){
                sleep(2);
                printf("Alfie> %s, use the lanturn! Then we will be able to see in here\n", playerName);
              }
                while(1){
                  sleep(2);
                  printf("%s> ", playerName);
                  fgets(playerInput, 200, stdin);
                    if( strcmp(playerInput, "use\n") == 0 && lanturn == 1){
                      sleep(2);
                      printf("*You hold up the lanturn*\n");
                      sleep(2);
                      printf("Alfie> Look, there is a key! Pick it up\n");
                      lit = 1;
                    }
                    else if( strcmp(playerInput, "use\n") == 0 && lanturn == 0){
                      sleep(2);
                      printf("%s> I need to find a light source!\n", playerName);
                    }
                    else if( strcmp(playerInput, "pick up\n") == 0 && lit == 1){
                      sleep(2);
                      printf("*You picked up the key and put it in your pocket*\n");
                      key = 1;
                    }
                    else if( strcmp(playerInput, "pick up\n") == 0 && lit != 1){
                      printf("*Couldn't find an item, room is too dark*\n");
                    }
                    else if( strcmp(playerInput, "south\n") == 0){
                      sleep(2);
                      printf("*Cant move, there is a wall*\n");
                    }
                    else if( strcmp(playerInput, "west\n") == 0){
                      sleep(2);
                      printf("*You turn around and walk back into the hall*\n");
                      position = 3;
                      break;
                    }
                    else if( strcmp(playerInput, "east\n") == 0 ){
                      sleep(2);
                      printf("*Can't move, there is a wall*\n");
                    }
                    else if(strcmp(playerInput, "north\n") == 0 ){
                      sleep(2);
                      printf("*Can't move, there is a wall*\n");
                    }
                    else{
                      printf("*Command not found*\n");
                    }
                  }
                }



              if(position == 8){ //Top of the stairs
                sleep(2);
                printf("*Suddenly, you hear the door behind you slam shut*\n");
                sleep(2);
                printf("Alfie> Don't worry %s, let's keep going. We must be getting close\n", playerName);
                while(1){
                    sleep(2);
                    printf("*You are standing at the top of the stairs, looking at three doors*\n");
                    sleep(2);
                    printf("*There is one to the south, one to the east, and one to the west. The stairs go down to the north*\n");
                    sleep(2);
                    printf("Alfie> Alright %s, it's your decision!\n", playerName);

                    printf("%s> ", playerName);
                    fgets(playerInput, 200, stdin);
                    if( strcmp(playerInput, "north\n") == 0 ){
                      sleep(2);
                      printf("*You walk back down the stairs and try opening the door that slammed shut*\n");
                      sleep(2);
                      printf("*It is locked*\n");
                      sleep(2);
                      printf("Alife> Looks like we can't go back\n");
                      sleep(2);
                      printf("*You both walk back up the stairs*\n");
                    }
                    else if( strcmp(playerInput, "west\n") == 0 ){
                      sleep(2);
                      printf("*You open the door and walk into the room*\n");
                      sleep(2);
                      printf("*Click*\n");
                      sleep(2);
                      printf("*The door locked behind you*\n");
                      sleep(2);
                      printf("Game Master> *Mysterious Laugh*\n");
                      sleep(2);
                      printf("*Screeeeetch*\n");
                      sleep(2);
                      printf("*The walls start to close in*\n");
                      sleep(2);
                      printf("*Screeeeetch*\n");
                      sleep(2);
                      printf("Alfie> No! Not like this!\n");
                      sleep(2);
                      printf("Alfie> Well, it was nice knowing you %s!\n", playerName);
                      sleep(2);
                      printf("*The walls finally close all the way in. You died*\n");
                      sleep(3);
                      printf("*Game over*\n");
                      sleep(2);
                      exit(-1);
                    }
                    else if( strcmp(playerInput, "south\n") == 0 ){
                      sleep(2);
                      printf("*You open the door and walk in the room*\n");
                      sleep(2);
                      printf("*It is pitch black*\n");
                      sleep(2);
                      printf("*You hold up your lanturn*\n");
                      sleep(2);
                      printf("*Suddenly, a trap door opens up below you*\n");
                      sleep(2);
                      printf("*You fall through and hit the ground hard*!\n");
                      sleep(2);
                      printf("Alife> Ouch!\n");
                      sleep(2);
                      printf("Alife> We are back to the beginning again!\n");
                      position = 0;
                      break;
                    }
                    else if( strcmp(playerInput, "east\n") == 0 ){
                      sleep(2);
                      printf("*You open the door and see blue sky!*\n");
                      sleep(2);
                      printf("Alife> LOOK, A DRAGON!\n");
                      sleep(2);
                      printf("*The dragon stands up and blows fire as a greating sign*\n");
                      sleep(2);
                      printf("*You both walk over to it*\n");
                      sleep(2);
                      printf("*It has a mount on it's back for two riders*\n");
                      sleep(2);
                      printf("*You both hop on and it flies away*\n");
                      sleep(2);
                      printf("*You notice a little piece of paper tucked under the mount*\n");
                      sleep(2);
                      printf("Alife> Pick up the paper!\n");
                      position = 9;
                      break;
                    }
                    else{
                      printf("*Command not found*\n");
                    }
                  }
                }


                if(position == 9){ //going to the dragon

                  while(1){

                    printf("%s> ", playerName);
                    fgets(playerInput, 200, stdin);
                    if( strcmp(playerInput, "north\n") == 0 ){
                      sleep(2);
                      printf("*Can't move, you don't want to fall off the dragon!*\n");
                    }
                    else if( strcmp(playerInput, "south\n") == 0 ){
                      sleep(2);
                      printf("*Can't move, you don't want to fall off the dragon!*\n");
                    }
                    else if( strcmp(playerInput, "east\n") == 0 ){
                      sleep(2);
                      printf("*Can't move, you don't want to fall off the dragon!*\n");
                    }
                    else if( strcmp(playerInput, "west\n") == 0 ){
                      sleep(2);
                      printf("*Can't move, you don't want to fall off the dragon!*\n");
                    }
                    else if( strcmp(playerInput, "pick up\n") == 0 ){
                      sleep(2);
                      printf("*You pick up the piece of paper*\n");
                      sleep(2);
                      printf("Dear %s\nCongrats on eascaping the maze!\nTo finish the game, run the shell again and type \'englishName Alife\' as a command\n", playerName);
                      exit(-1);
                    }
                    else{
                      printf("*Command not found*\n");
                    }
                  }
                }



      } //While loop

    }

  }

  else if( strcmp(argv[0], "\n") == 0){
    //Do nothing
  }

  /*Prmopt when no command was found*/
  else{
    printf("jash: %s: command not found\n", argv[0]);
  }

  /*Makes sure everything is cleared*/
  for(i = 0; i < k; i++){
    argv[i] = NULL;
  }
  argv[k] = NULL;

  }

  return 0;
}

/*signal handler function*/
void killChild(int signalll){
  waitpid(signalll, NULL, WNOHANG);
}

/*Function that finds GCD of two numbers*/
void greatestCommonDenom(int first, int second){
  int i;
  int GCD = 0;

  for(i = 1; i <= first && i <= second; i++){
    if( (first % i == 0) && (second % i == 0) ) GCD = i;
  }

  printf("%d\n", GCD);

}

/*Helper function that converts hex number to decimal*/
int hexToDec(char num[]){
  int len = strlen(num);
  int i = 0;
  int base = 1;
  int decimal = 0;

    for ( i = len - 1; i >= 0; i-- ){
      if ( num[i] >= '0' && num[i] <= '9' ){
        decimal += (num[i] - 48) * base;
        base *= 16;
      }
      else if( num[i] >= 'a' && num[i] <= 'f' ){
        decimal += (num[i] - 87) * base;
        base *= 16;
      }
    }

    return decimal;
}

/*Helper function that takes out white spaces when reading the command line*/
char *trim(char *userInput){
  char *endOfString;
  int length = strlen(userInput) - 1;

  endOfString = userInput + length;

  while(endOfString > userInput && isspace(*endOfString)){
    endOfString = endOfString - 1;
  }

  *(endOfString+1) = '\0';

  return userInput;
}
