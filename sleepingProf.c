// Christopher Gjersing
// NSHE: 5004590677
// CS370 1002 & 1003
// Project #3: Semaphores / Tired Professor



//provided files
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdbool.h>

//provided parameters
#define 	STUDENT_COUNT_MIN		2
#define 	STUDENT_COUNT_MAX		10
#define 	CHAIR_COUNT					3
#define 	HELPS_MAX						3

//Global variables
int openChairs = CHAIR_COUNT;	//tracks the number of open waiting chairs
int numStudents;							//numStudents value, entered by user
int *studentHelp;							//array of integers, size numStudents, with # of times they need helped
int totalHelp = 0;

//Mutex and semaphores
pthread_mutex_t teachStudent; //locks up the office chair when student is being helped
pthread_mutex_t changeChairs; //mutex for locking the openChairs value
sem_t profReady;							//used to signal when, and if, the professor is ready
sem_t studentReady;						//used to signal when, and if, the student is ready
sem_t checkChairs;						//used to track and take the waiting chairs outside office

//Student thread function: Works on assignment, visits office, checks chairs, gets help or leaves, repeats.
void * student(void *id){
		int threadId = (int)(intptr_t)id;	//Grab thread # from arguments
		//printf("Student %d will need helped %d times.\n", threadId+1, studentHelp[threadId]);
		
		while(1){
			//work on assignment for random amount of time
			printf("\033[0;93mStudent %d doing assignment.\033[0m\n", threadId+1);
			usleep(rand()%2000000);
			//Student decides to ask for help
			printf("\033[0;31mStudent %d needs help from the professor.\033[0m\n", threadId+1);
			sem_wait(&checkChairs);
			pthread_mutex_lock(&changeChairs);
			if(openChairs > 0){	//if a seat is open, take it
					if(openChairs == CHAIR_COUNT){	//if all seats are open, wake up professor
							printf("\033[0;92mProfessor has been awakened by a student.\033[0m\n");	
					}
					openChairs--;
					pthread_mutex_unlock(&changeChairs);
					//signal that the student is waiting and ready to be helped by professor
					sem_post(&studentReady);
					//release the checkChairs semaphore for other students
					sem_post(&checkChairs);
					
					//wait for the professor to be ready
					sem_wait(&profReady);
					studentHelp[threadId]--;
					printf("\033[0;31mStudent %d is getting help from the professor.\033[0m\n", threadId+1);
					if(studentHelp[threadId] == 0){
							pthread_exit(NULL);	//exit if student has been helped the correct # of times
					}
			}
			else{
					sem_post(&checkChairs); //release checkChairs for other students
					pthread_mutex_unlock(&changeChairs);
					printf("\033[0;31mChairs occupied, student %d will return later.\033[0m\n", threadId+1);
			}
		}
		pthread_exit(NULL); //just in case we somehow leave the thread while loop
}

//Professor thread function: Very tired person. Sleeps until awakened by a student.
//Helps all waiting students until going to sleep, probably slobbering all over the desk.
void * professor(){
		int countHelped = 0;
		while(1){
				sem_wait(&studentReady); //sleep until student awakens
				while(1){ //loop to help all waiting students
						
						//Student vacates chair and enters office
						pthread_mutex_lock(&changeChairs);
						if(openChairs == CHAIR_COUNT){
								pthread_mutex_unlock(&changeChairs);
								break;
						}
						openChairs++;
						pthread_mutex_unlock(&changeChairs);
						printf("\033[0;92mStudent frees chair and enters professors office. Remaining chairs %d\033[0m\n", openChairs);
						//increase the number of waiting chairs as student gets up to enter office		
						
				
						//Let the student enter the office
						sem_post(&profReady);
						pthread_mutex_lock(&teachStudent);
						printf("\033[0;92mProfessor is helping a student.\033[0m\n");
						//help students inside of mutex
						usleep(rand()%1500000);
						countHelped++;
						pthread_mutex_unlock(&teachStudent);	
						if(countHelped == totalHelp){
								break;
						}
				}		
				if(countHelped == totalHelp){
						pthread_exit(NULL);
				}
		}
		pthread_exit(NULL);
}

//Main: get student count, dynamically create professor students, and student helps.
int main(int argc, char *argv[]){
		//red: 	 	printf("\033[0;31mred\033[0m\n");
		//green: 	printf("\033[0;92mgreen\033[0m\n");
		//blue:	 	printf("\033[0;94mblue\033[0m\n");
		//yellow:	printf("\033[0;93myellow\033[0m\n");
		//uline:	  printf("\033[0;1muline\033[0m\n");
		
		//Display header
		printf("\033[0;1mCS 370 - Sleeping Professor Project\033[0m\n\n");
		
		//Read and validate student count, please don't enter a non-integer it will go crazy.
		printf("How many students coming to professor's office? ");
		scanf("%d", &numStudents);
		//Loop until a valid number is entered.
		while(numStudents < STUDENT_COUNT_MIN || numStudents > STUDENT_COUNT_MAX){
				printf("Error, invalid number of students entered.\n");
				printf("Student count must be between %d and %d.\n", STUDENT_COUNT_MIN, STUDENT_COUNT_MAX);
				printf("How many students coming to professor's office? ");
				scanf("%d", &numStudents);
		}
		
		//Dynamically create student helps array of random size [0, HELPS_MAX+1]
		studentHelp = malloc(numStudents * sizeof(*studentHelp));
		srand(time(NULL));
		for(int i = 0; i < numStudents; i++){
				studentHelp[i] = (rand()%HELPS_MAX+1);
				totalHelp += studentHelp[i];
				//printf("Student %d will need helped %d times.\n", i+1, studentHelp[i]);
		}
		printf("I need to help %d students %d times today? Seriously? They should read the book.\n\n", numStudents, totalHelp);
		//Dynamically create student threads array based on numStudents
		pthread_t *studentThd;
		studentThd = (pthread_t*)malloc(sizeof(pthread_t)*numStudents);
		
		//Intialize semaphores
		sem_init(&profReady, 0, 0);
		sem_init(&studentReady, 0, 0);
		sem_init(&checkChairs, 0, 1);
		pthread_mutex_init(&teachStudent, NULL);
		pthread_mutex_init(&changeChairs, NULL);
		
		//Create one professor
		pthread_t professorThd;
		int errCheck = pthread_create(&professorThd, NULL, professor, NULL);
		if(errCheck){
				printf("Error creating professor thread.\n");
		}
		//Create student threads
		for(int i = 0; i < numStudents; i++){
				errCheck = pthread_create(&studentThd[i], NULL, student, (void *)(intptr_t)i);
				if(errCheck){
						printf("Error creating student thread %d.\n", i);
				}
		}
		
		//Wait for threads to complete before exiting
		pthread_join(professorThd, NULL);
		for(int i = 0; i < numStudents; i++){
				pthread_join(studentThd[i], NULL);
		}
		printf("\033[0;94mAll students assisted, professor is leaving.\033[0m\n");
		free(studentHelp);
		free(studentThd);
}
