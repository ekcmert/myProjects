#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

// Mert Ekici 26772 ekicimert@sabanciuniv.edu
// CS307 - PA2 Synchronizing the CLI Simulator - Bonus Part (Ordered Lock)

// Buffer size
#define BUFSIZE 1000


// You can declare your shared variables here.

//int array that stores of thread indexes with background jobs and its counter
int bgs[50];
int bgscounter = 0;

//ordered lock struck: it holds current turn and mutex
struct orderlock {
	int turn;
	pthread_mutex_t lock;
};

//lock initializer
void lock_init(struct orderlock *l){
	l->turn = 0;
	l->lock = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
} 

struct orderlock oLock;

//struct to be passed to the display function
struct turnHolder{
	int turn;
	int* fd;
};


//lock function (if turn is not myturn wait)
void lock(struct orderlock *l,int myturn){
	while(myturn != l->turn){


		;
	}
	pthread_mutex_lock(&(l->lock));
}

//unlock function (if turn is not myturn wait)
void unlock(struct orderlock *l,int myturn){
	while(myturn != l->turn){


		;
	}
	l->turn += 1;
	pthread_mutex_unlock(&(l->lock));
}


// You should fill this function.
void *display(void *args)
{

	//myturn and fd from the struct
	int myturn = ((struct turnHolder*)args)->turn;
	int *fd = ((struct turnHolder*)args)->fd;

	//lock - enter critical section
	lock(&oLock,myturn);

	//no need for this
	//dup2(fd[0], STDIN_FILENO);

	printf("---- %ld\n", pthread_self());

	char buf[BUFSIZE];


	int n = read(fd[0], buf, BUFSIZE);
	for (int i = 0; i < n; i++)
	{
		// printing the array received from child process
		printf("%c", buf[i]);
	}

	printf("---- %ld\n", pthread_self());

	fflush(stdout);

	unlock(&oLock,myturn);
	//unlock - exit critical section
}

void removeCharacter(char *line, char characterToRemove)
{
	int length = strlen(line);
	int i, j;

	for (i = 0; i < length; i++)
	{
		if (line[i] == characterToRemove)
		{
			for (j = i; j < length; j++)
			{
				line[j] = line[j + 1];
			}
			length--;
			i--;
		}
	}
}

// You can modify the main except for the parser.
int main(int argc, char *argv[])
{
	FILE *fptr;

	//initialize the lock
	lock_init(&oLock);

	// You can use these variables for creating and manipulating shell threads.
	pthread_t threads[50];
	int thrCount = 0;
	int pointThread = 0;

	// Integer array for the pipes maximum thread size is 50. Thus, this array has 100 integers
	int fds[100];

	fptr = fopen("./commands.txt", "r");

	if (fptr == NULL)
	{
		exit(1);
	}

	char line[256];

	while (fgets(line, sizeof(line), fptr))
	{
		// remove the carriage return '\r'
		removeCharacter(line, '\r');
		char redirection = '-';
		char background = 'n';

		for (int i = 0; i < strlen(line); i++)
		{
			if (line[i] == '<')
			{
				redirection = '<';
			}
			else if (line[i] == '>')
			{
				redirection = '>';
			}
			if (line[i] == '&')
			{
				background = 'y';
			}
		}

		// PARSER STARTS
		// DO NOT MODIFY THE PARSER!
		char *myargs[10];

		int fileFlag = 0;

		int argNumber = 0;

		char *redirFile;

		int onWord = 0;
		char wordLength = 0;

		int redirFilelen = 0;

		for (int i = 0; i < strlen(line) && line[i] != '&' && line[i] != '\n'; i++)
		{
			if (line[i] != ' ' && line[i] != '<' && line[i] != '>')
			{
				onWord = 1;
				wordLength++;

				if ((i + 1 < strlen(line) && line[i + 1] == '\n') || i + 1 == strlen(line))
				{
					char *word = (char *)malloc(sizeof(char) * (wordLength + 1));
					memcpy(word, line + i + 1 - wordLength, wordLength);
					myargs[argNumber] = strdup(word);
					argNumber++;
					break;
				}
			}

			else if (onWord == 1 && line[i] == ' ')
			{
				onWord = 0;
				char *word = (char *)malloc(sizeof(char) * (wordLength + 1));
				memcpy(word, line + i - wordLength, wordLength);
				myargs[argNumber] = strdup(word);
				argNumber++;
				wordLength = 0;
			}

			else if (line[i] == '<' || line[i] == '>')
			{
				int k;
				for (k = i + 1; k < strlen(line) && line[k] != '\n'; k++)
				{
					if (line[k] != ' ')
					{
						fileFlag = 1;
						redirFilelen++;
						if (k + 1 == strlen(line))
						{
							break;
						}
					}
					else if (fileFlag == 1 && line[k] == ' ')
					{
						break;
					}
				}
				redirFile = (char *)malloc(sizeof(char) * (redirFilelen + 1));
				memcpy(redirFile, line + k - redirFilelen, redirFilelen);
				break;
			}
		}
		// PARSER ENDS

		myargs[argNumber] = NULL;


		// You should fill this 'if' block for handling wait commands.
		if (myargs[0][0] == 'w' && myargs[0][1] == 'a')
		{

			// wait for the threads
			for (int i = 0; i < bgscounter; i++)
			{
				pthread_join(threads[bgs[i]], NULL);
			}

			// wait for the output redirections
			while (wait(NULL) > 0)
				;
		}

		// if the command is not "wait"
		else{
		if (redirection != '>')
		{
			
			pointThread = thrCount;
			pipe(fds + pointThread * 2);
			struct turnHolder *mt = (struct turnHolder *)malloc(sizeof(struct turnHolder));
			mt->turn = pointThread;
			mt->fd = &fds[pointThread * 2];
			pthread_create(&threads[pointThread], NULL,display, (void *)mt );
			thrCount++;
		}
		int rc = fork();
		if (rc < 0)
		{
			exit(1);
		}

		else if (rc == 0)
		{

			if (redirection == '-')
			{
				// close read end of the pipe duplicate write end to STDOUT file descriptor
				close(fds[pointThread * 2]);
				dup2(fds[pointThread * 2 + 1], STDOUT_FILENO);
				close(fds[pointThread * 2 + 1]);
			}
			else if (redirection == '<')
			{
				// close read end of the pipe duplicate write end to STDOUT file descriptor
				close(STDIN_FILENO);
				int fileNo = open(redirFile, O_RDONLY);
				close(fds[pointThread * 2]);
				dup2(fds[pointThread * 2 + 1], STDOUT_FILENO);
				close(fds[pointThread * 2 + 1]);
			}
			// output redirectioning output will be printed directly
			else
			{
				close(STDOUT_FILENO);
				open(redirFile, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
			}
			
			// if execvp gives error, exit
			if(execvp(myargs[0], myargs)==-1) exit(0);
		}
		else
		{
			// if there is output redirection and the command is a background job, wait for the child process
			if (redirection == '>')
			{
				if (background == 'n')
				{
					waitpid(rc, NULL, 0);
				}
			}
			// if no output redirectioning (thread case), join the current thread and wait for the child
			else if (redirection != '>')
			{
				close(fds[pointThread * 2 + 1]);
				if (background == 'n')
				{
					pthread_join(threads[pointThread], NULL);
					waitpid(rc, NULL, 0);
				}
				//if the job is for the background, add this thread's index to bgs array
				else
					{
						bgs[bgscounter] = pointThread;
						bgscounter++;
					}
			}
		}

	}
		
	}
	// after all the lines are read, wait for the child processes and the running threads
	for (int i = 0; i < bgscounter; i++)
	{
		pthread_join(threads[bgs[i]], NULL);
	}
	while (wait(NULL) > 0)
		;

	return 0;
}
