// Mert Ekici 26772 ekicimert@sabanciuniv.edu
// CS307 - PA4 Heap Management

#include <iostream>
#include <string>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>


//Mutex Locks for read and write on the linked-list
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t printLock = PTHREAD_MUTEX_INITIALIZER;

using namespace std;


//data struct that holds information about the segment
typedef struct data{
	int id;
	int size;
	int index;
} data_t;

//linked list node
struct node{
	node* next;
	node* prev;
	data_t info;
};


//LinkedList class to be used in HeapManager class
class LinkedList
{
public:
	node* head;

	//constructor
	LinkedList(){
		head = NULL;
	}

};


//HeapManager class
class HeapManager{

private:
	LinkedList heapList;

public:
	int initHeap(int size){

		//create the initial node
		node* initNode = new node;

		//nullify the next and prev nodes of the initial node
		initNode->next = NULL;
		initNode->prev = NULL;

		//fill the information of the initial node
		initNode->info.index = 0;
		initNode->info.size = size;
		initNode->info.id = -1;

		//assign the initial node as the head of heapList

		pthread_mutex_lock(&lock);
		heapList.head = initNode;


		//print before the return
		cout<< "Memory initialized" << endl;
		print();

		pthread_mutex_unlock(&lock);
		return 1;

	};
	int myMalloc(int ID, int size){

		pthread_mutex_lock(&lock);


		node* curr = heapList.head;


		while(curr){

			//if the node is free
			if(curr->info.id == -1){

				//if the free node has enough space
				if(curr->info.size >= size){

					//if the space is more than required
					if(curr->info.size > size){

						node* newFNode = new node;
						node* newSNode = new node;

						//add connections 
						newFNode->next = newSNode;
						newFNode->prev = curr->prev;

						//null safety prev node link update
						if(newFNode->prev){
							newFNode->prev->next = newFNode;
						}
						else{
							heapList.head = newFNode;
						}

						//add information 
						newFNode->info.index = curr->info.index;
						newFNode->info.size = size;
						newFNode->info.id = ID;

						//add connections 
						newSNode->next = curr->next;
						newSNode->prev = newFNode;

						//null safety next node link update
						if(newSNode->next){
							newSNode->next->prev = newSNode;
						}

						//add information 
						newSNode->info.index = curr->info.index + size;
						newSNode->info.size = curr->info.size - size;
						newSNode->info.id = -1;


						//free the curr (new nodes are used now)
						free(curr);

						//print the success info
						cout<<"Allocated for thread "<<ID<<endl;
						print();
						pthread_mutex_unlock(&lock);


						return newFNode->info.index;

					}

					//if it equals required size
					else{

						//this update is sufficient, only assign the given ID (now it is not free -> allocated)
						curr->info.id = ID;

						cout<<"Allocated for thread "<<ID<<endl;
						print();
						pthread_mutex_unlock(&lock);

						return curr->info.index;
					}
				}
				// if free but space is not big enough
				else {
					curr = curr->next;
				}
			}
			//if the node is not free
			else{
				curr = curr->next;
			}
		}


		//no suitable candidate found

		//print the failure info
		cout<<"Can not allocate, requested size "<<size<<"  for thread "<<ID<<" is bigger than remaining size"<<endl;
		print();
		pthread_mutex_unlock(&lock);

		return -1;


	};
	int myFree(int ID, int index){

		pthread_mutex_lock(&lock);


		node* curr = heapList.head;

		while(curr){
			if(curr->info.id == ID && curr->info.index == index){
				//mark as free
				curr->info.id = -1;
				node* next = curr->next;
				node* prev = curr->prev;
				//check next node if it is free merge 
				if(next){
					if(next->info.id == -1){

						// add next's size to curr node and update curr's next
						curr->info.size += next->info.size;
						curr->next = next->next;

						//null safety next node link update
						if(curr->next){
							curr->next->prev = curr;
						}
						free(next);
					}
				}
				//check prev node if it is free merge 
				if(prev){
					if(prev->info.id == -1){

						//add curr's size to prev node and update prev's next
						prev->info.size += curr->info.size;
						prev->next = curr->next;

						//null safety next node link update
						if(prev->next){
							prev->next->prev = prev;
						}
						free(curr);
					}
				}
				//print the success info
				cout<<"Freed for thread "<<ID<<endl;
				print();
				pthread_mutex_unlock(&lock);

				return 1;
			}
			else{
				curr = curr->next;
			}		
		}

		print();
		pthread_mutex_unlock(&lock);

		return -1;	

	};


	void print(){
		pthread_mutex_lock(&printLock);
		node* curr = heapList.head;

		while(curr){

			//print the node in the given format
			cout<<"["<<curr->info.id<<"]"<<"["<<curr->info.size<<"]"<<"["<<curr->info.index<<"]";

			//if not last element
			if(curr->next){
				cout<<"---";
			}

			//if last element
			else{
				cout<<endl;
			}
			curr = curr->next;
		}
		pthread_mutex_unlock(&printLock);

	};
};



