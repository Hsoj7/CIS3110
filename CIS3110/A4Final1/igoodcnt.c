#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

/*variable declarations*/
int NITER = 1000000;
int cnt = 0;
/*global semaphore variable*/
sem_t ourSem;

/*Function to increment the shared CNT variable based on the number of threads
  which is 2 in our case*/
void *Count(void * a)
{
    int i, tmp;
    /*sem_wait waits on the passed semaphore. In other words, it blocks all other
    processes/threads until the current one is done executing*/
    sem_wait(&ourSem);
    for(i = 0; i < NITER; i++)
    {
        tmp = cnt;      /* copy the global cnt locally */
        tmp = tmp+1;    /* increment the local copy */
        cnt = tmp;      /* store the local value into the global cnt */
    }
    /*sem_post increments the value of the passed semaphore to indicate its completion
    and wakes up any blocked processes/threads that are ready to execute*/
    sem_post(&ourSem);
}

int main(int argc, char * argv[])
{
  /*local thread variables*/
	pthread_t tid1, tid2;
  int i = 0;

	/* validate arguments*/
	/* implementation*/
	/* & error checking of the command line*/

	/*checks to make sure there are not too many arguments*/
	if(argc > 2){
    fprintf(stderr, "Usage: %s, Too many Arguments\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  /*checks to make sure a value for niter was included*/
  else if(argc < 2){
    fprintf(stderr, "Usage: %s, Must include # for NITER\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  NITER = atoi(argv[1]);
  /*checks if the niter value is alphabetic*/
  for(i = 0; i < strlen(argv[1]); i++){
    if(isalpha(argv[1][i])){
      fprintf(stderr, "Usage: %s, %s, must be numeric\n", argv[0], argv[1]);
      exit(EXIT_FAILURE);
    }
  }
  /*checks to make sure the niter value is not negative*/
  if(NITER < 1){
    fprintf(stderr, "Usage: %s, NITER must be a positive, above 0 value\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  /*initializes the semaphore with 0, indicating the semaphore is shared between
  threads of the process and 1, indicating */
  if(sem_init(&ourSem, 0, 1) == -1){
    fprintf(stderr, "Usage: %s, Error initializing semaphore\n", argv[0]);
    exit(1);
  }
	/* Parsed the arguments passed to my C program
	   Including the number of times that each thread increments
	   the shared count cnt
	   For example, NITER = 20000;*/

	/* Display the number of times (entered by the user) that each thread*/
	/* increments the shared count cnt*/
/* PLEASE DO NOT remove or modify the following code*/
	printf("2*NITER is [%d]\n", 2*NITER);
/* End of code section*/

	/* creating Thread 1*/
	if(pthread_create(&tid1, NULL, Count, NULL) != 0)
	{
    /*error checks to make sure the thread was created successfully*/
		printf("\n ERROR creating thread 1");
		exit(1);
	}


	/* creating Thread 2*/
	if(pthread_create(&tid2, NULL, Count, NULL) != 0)
	{
    /*error checks to make sure the thread was created successfully*/
		printf("\n ERROR creating thread 2");
		exit(1);
	}

	if(pthread_join(tid1, NULL))	/* wait for the thread 1 to finish */
	{
    /*error checks to make sure the thread was joined successfully*/
		printf("\n ERROR joining thread");
		exit(1);
	}

	if(pthread_join(tid2, NULL))        /* wait for the thread 2 to finish */
	{
    /*error checks to make sure the thread was joined successfully*/
		printf("\n ERROR joining thread");
		exit(1);
	}
  /*terminates the semaphore we created at the beginning of the program*/
  sem_destroy(&ourSem);

        /* Display the value of count cnt*/
/* PLEASE DO NOT remove or modify the following code*/
	printf("\nCounter is [%d]\n", cnt);
	if (cnt == 2 * NITER)
		printf("\nPASS\n");
	else
		printf("\nFAIL\n");
/* End of code section*/
  /*exits the threads*/
	pthread_exit(NULL);
}
