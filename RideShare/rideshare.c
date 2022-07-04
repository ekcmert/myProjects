#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

// Mert Ekici 26772 ekicimert@sabanciuniv.edu
// CS307 - PA3 Riding to a Soccer Game

//number of threads
#define NTHR 100

//mutex locks 
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockW = PTHREAD_MUTEX_INITIALIZER;

//global variables
int moveCount = 0;
int countA = 0;
int countB = 0;
int inA = 0;
int inB = 0;

//semaphores and barriers
sem_t barrierForm;
sem_t barrierEnter;
sem_t barrierA;
sem_t barrierB;
sem_t barrierMove;

//supporter struct as an argument for rideShare function
struct supporter
{
	char team;
	int isCaptain;
};


//rideshare function (thread function)
void *rideShare(void *args)
{

	//typecast and seperate the void args using supporter struct
	char myTeam = ((struct supporter*)args)->team;
	int *amICaptain = &((struct supporter*)args)->isCaptain;
	
	//PHASE I -> looking for a car

	//form barrier (if supporters are entering the car wait until they leave)
	sem_wait(&barrierForm);

	//lock (for countA, countB, barrierMove)
	pthread_mutex_lock(&(lock));

	if(myTeam == 'A') countA++;
	else countB++;

	//form prompter

	//mutex lock for write operation
	pthread_mutex_lock(&(lockW));
	printf("Thread ID: %ld, ", pthread_self());
	printf("Team: %c, ", myTeam);
	printf("%s\n","I am looking for a car");
	fflush(stdout);
	pthread_mutex_unlock(&(lockW));
	
	//if valid combination is possible prepare barrierA and barrierB depending on the form
	//decrease barrierMove to 0 (lock it) or wait for it
	//lock barrierform (do not form new combination until this band moves)
	//match the former person as the captain
	if(myTeam == 'A'){
		if( (countA == 2 && countB >= 2) || (countA == 4) ){
		sem_wait(&barrierMove);
		*amICaptain = 1;
		sem_post(&barrierEnter);
		if(countA==4){
			sem_post(&barrierA);
			sem_post(&barrierA);
			sem_post(&barrierA);
			sem_post(&barrierA);
		}
		else{
			sem_post(&barrierB);
			sem_post(&barrierB);
			sem_post(&barrierA);
			sem_post(&barrierA);
		}
		}
		else{
			sem_post(&barrierForm);
		}
	
	}
	else if(myTeam == 'B'){
		if( (countA >= 2 && countB == 2) ||  (countB == 4) ){
		sem_wait(&barrierMove);
		*amICaptain = 1;
		sem_post(&barrierEnter);
		if(countB==4){
			sem_post(&barrierB);
			sem_post(&barrierB);
			sem_post(&barrierB);
			sem_post(&barrierB);
		}
		else{
			sem_post(&barrierB);
			sem_post(&barrierB);
			sem_post(&barrierA);
			sem_post(&barrierA);
		}
		}
		else{
			sem_post(&barrierForm);
		}
	
	}	
	pthread_mutex_unlock(&(lock));

	//PHASE 2 -> found a spot in a car

	//wait for the correct supporters
	if(myTeam == 'A'){
		sem_wait(&barrierA);
	}
	else{
		sem_wait(&barrierB);
	}

	//enter the car
	sem_wait(&barrierEnter);
	
	pthread_mutex_lock(&(lock));

	//enter prompter
	pthread_mutex_lock(&(lockW));
	printf("Thread ID: %ld, ", pthread_self());
	printf("Team: %c, ", myTeam);
	printf("%s\n","I have found a spot in a car");
	fflush(stdout);
	pthread_mutex_unlock(&(lockW));

	if(myTeam == 'A') {inA++; countA--;}
	else {inB++; countB--;}

	//reset inA and inB
	//send move signal to the people waiting for barrierMove 
	if((inA == 2 && inB == 2) || (inA == 4) || (inB == 4)){
		inA = 0;
		inB = 0;
		sem_post(&barrierMove);
	}
	else{
		//add one more
		sem_post(&barrierEnter);
	}

	pthread_mutex_unlock(&(lock));


	//PHASE 3 --> captain 

	sem_wait(&barrierMove);
	
	//mutex lock for moveCount and barrierForm
	pthread_mutex_lock(&(lock2));
	if(*amICaptain == 1){

	//captain prompter
	pthread_mutex_lock(&(lockW));
	printf("Thread ID: %ld, ", pthread_self());
	printf("Team: %c, ", myTeam);
	printf("%s\n","I am the captain and driving the car");
	fflush(stdout);
	pthread_mutex_unlock(&(lockW));
	}

	moveCount++;

	//when moveCount reaches 4, reset it and unlock the form barrier
	if(moveCount == 4){
		
		moveCount = 0;
		sem_post(&barrierForm);
	}
	pthread_mutex_unlock(&(lock2));
	sem_post(&barrierMove);

}


int main(int argc, char *argv[])
{

	pthread_t threads[NTHR];

	int numA = atoi(argv[1]);
	int numB = atoi(argv[2]);
	int total = numA + numB;

	//semaphore initialization
	sem_init(&barrierForm, 0, 1);
	sem_init(&barrierEnter, 0, 0);
	sem_init(&barrierMove, 0, 1);

	sem_init(&barrierA, 0, 0);
	sem_init(&barrierB, 0, 0);

	// input validity
	// size of each team should be an even number
	// total number of supporters should be a multiple of fourS

	if (argc != 3)
	{
	}
	else
	{
		if ((numA % 2) != 0 || (numB % 2) != 0)
		{
		}
		else if ((numA + numB) % 4 != 0)
		{
		}
		else
		{
			//create threads of supporterA and supporterB
			for (int i = 0; i < total; i++)
			{
				if (i < numA)
				{
					struct supporter *supA = (struct supporter *)malloc(sizeof(struct supporter));
					supA->team = 'A';
					supA->isCaptain = 0;
					pthread_create(&(threads[i]), NULL, rideShare, (void *)supA);
				}
				else
				{
					struct supporter *supB = (struct supporter *)malloc(sizeof(struct supporter));
					supB->team = 'B';
					supB->isCaptain = 0;
					pthread_create(&(threads[i]), NULL, rideShare, (void *)supB);
				}
			}

			//wait for the threads not terminated yet
			for (int j = 0; j < total; j++)
			{
				pthread_join(threads[j], NULL);
			}
		}
	}

	printf("The main terminates\n");
	fflush(stdout);

	return 0;
}
