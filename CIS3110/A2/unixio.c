#include <fcntl.h> // open
#include <unistd.h> // read
#include <sys/types.h> // read
#include <sys/uio.h> // read
#include <stdio.h> // fopen, fread
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h> // gettimeofday
#include <ctype.h>
#include <dirent.h>

/*
can we hard code the positions of argv? Will they all always be the same?
what should happen if filename does not exist in the current directory?
what will filename (now data_1.txt) be in t.sh?
*/

int main(int argc, char *argv[] ) {
  struct timeb start, end; // maintain starting and finishing wall time.
  DIR *dPtr;
  struct dirent *directory;
  int milSeconds = 0;
  float seconds = 0.0;
  int typeofcalls = 0;
  int i = 0, j = 0;
  int foundFile = 0;
  long numBytes = 0;
  long size = 0;
  double doubleSize = 0.0;
  int readBytes = 0;
  int good = 1;
  int descriptor = 0;
  char dot = '.';
  char fileName[100];
  char buffer[65530]; //65535 is max in c99
  char WDFiles[50][50];
  FILE *fp;

  if(argc != 4){
    fprintf(stderr, "Usage: Incorrect number of arguments for %s\n", argv[0]);
    exit(1);
  }
  else{
    //parse argv
    if( strrchr(argv[1], dot) != NULL ){
      dPtr = opendir(".");
      if(dPtr != NULL){
        i = 0;
        while( (directory = readdir(dPtr)) != NULL){
        //  WDFiles[i] = directory->d_name;
        //  strcpy(WDFiles[i], directory->d_name);
          if(strncmp(directory->d_name, ".", 1) == 0){
          }
          else{
            strcpy(WDFiles[i], directory->d_name);
            i++;
          }
        }
      }

      for(j = 0; j < i; j++){
        if(strcmp(argv[1], WDFiles[j]) == 0){
          strcpy(fileName, argv[1]);
          foundFile = 1;
        }
      }

      if(foundFile == 0){
        fprintf(stderr, "Usage: Could not find file %s in argument 1\n", argv[1]);
        exit(1);
      }

    }
    else if( strrchr(argv[1], dot) == NULL ){
      fprintf(stderr, "Usage: Argument 1, No file was found\n");
      exit(1);
    }

    //check for non integers in bytes
    for(i = 0; i < strlen(argv[2]); i++){
      if( isdigit(argv[2][i]) ){
      }
      else{
        good = 0;
      }
    }

    if(good == 1){
      numBytes = atoi(argv[2]);
      if(numBytes >= 65530){
        fprintf(stderr, "Usage: number of bytes in argument 2: %s is too large\n", argv[2]);
        exit(1);
      }
      else if( numBytes <= 0 ){
        fprintf(stderr, "Usage: number of bytes in argument 2: %s, cannot be negative or zero\n", argv[2]);
        exit(1);
      }
    }
    else if(good == 0){
      fprintf(stderr, "Usage: %s number of bytes not found\n", argv[2]);
      exit(1);
    }

    //check typeofcalls
    if( isdigit(argv[3][0]) || argv[3][0] == '-'){
      if(argv[3][0] == '-'){
        fprintf(stderr, "Usage: Negative numbers not aloud: %s\n", argv[3]);
        exit(1);
      }
      else if( atoi(argv[3]) == 1 ){
        typeofcalls = 1;
      }
      else if( atoi(argv[3]) == 0 ){
        typeofcalls = 0;
      }
      else{
        fprintf(stderr, "Usage: In argument %d, %s is unrecognized\n", 3, argv[3]);
        exit(1);
      }
    }
  }

  fp = fopen(fileName, "r");
  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  fclose(fp);
  doubleSize = (double) (size/1024);
  doubleSize /= 1024;

  //system
  if (typeofcalls == 1){
    printf("Using Unix I/O systems calls to read %s by %ld bytes per read\n", fileName, numBytes);
    ftime(&start);
    descriptor = open(fileName, O_RDONLY);

    if( descriptor < 0 ){
      fprintf(stderr, "Usage: %s could not open\n", argv[1]);
      exit(1);
    }
    else{
      while(1){
        readBytes = read(descriptor, buffer, numBytes);
        if( readBytes < 0 ){
          fprintf(stderr, "Usage: Could not read file\n");
          exit(1);
        }
        else if(readBytes == 0) break;
      }
    }

    if( close(descriptor) < 0 ){
      fprintf(stderr, "Usage: %s failed to close\n", argv[1]);
      exit(1);
    }

  }
  //library
  else if (typeofcalls == 0) {
    ftime(&start);
    fp = fopen(fileName, "r");

    if(numBytes >= 2){
      printf("Using C functions to read %s by %ld bytes per fread\n", fileName, numBytes);
      while(!feof(fp)){
        fread(buffer, sizeof(char), numBytes, fp); //FIX
      }
    }
    else if(numBytes == 1){
      printf("Using C functions to read %s by 1 byte at a time\n", fileName);
      while(!feof(fp)){
        fgetc(fp);
      }
    }

    fclose(fp);
  }

  ftime(&end);

  /*Find run time*/
  milSeconds = (int) (1000.0 * (end.time - start.time) + (end.millitm - start.millitm));
  seconds = milSeconds / 1000.0;
  if(doubleSize >= 1){
    printf("Time took to run: %dms, %.3fs on file size roughly %.0fMB\n\n", milSeconds, seconds, doubleSize);
  }
  else{
    printf("Time took to run: %dms, %.3fs on file size roughly %.4fMB\n\n", milSeconds, seconds, doubleSize);
  }

  return 0;
}
