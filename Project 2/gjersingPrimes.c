// Christopher Gjersing
// CS370 1002 & 1003
// Project #2: Threading Project

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <string.h>

//Global variables
pthread_mutex_t myLock; // thread locking
int iCounter = 10;			// iterator, starts at 10 to only process double digits
int twinPrimeCount = 2;	// return value, starts at 3 for (3,5,7) in unchecked 0-9
int limit = -1;					// iterator limit

int estSqrt(int check){
	double estimate = check;
	double div = check/estimate;
	double new = div+estimate;
	new = new/2;
	while((estimate-new) > 1){
		estimate = new;
		div = check/new;
		new = div+new;
		new = new/2;
	}
	new = new+1;
	return (int)new;
}

//Thread function to count twin primes
void * twinPrims(){
	while(iCounter < limit){ //main loop
		// CRITICAL : Get and increment twinPrimeCount within lock to avoid race conditions.
		pthread_mutex_lock(&myLock);
		int check = iCounter;
		iCounter++;
		pthread_mutex_unlock(&myLock);
		
		// REMAINDER: Checking if N is a prime.
		if(check < limit && (check%2) != 0){
			//Calculate k, the estimated square root of check
			int k = estSqrt(check);
			//Divide by odds [3, k], if %i != 0 for all then twin prime found
			int prime = 1;
			for(int i = 3; i < k; i += 2){//starts at 3 and adds 2 per increment
				if((check%i) == 0)
					prime = 0;
			}
			//if n is a prime, is (n-2) a prime?
			if(prime == 1){
				//initialize values
				int underPrime = 1;
				int under = check-2;
				int underEst = estSqrt(under);
				for(int i = 3; i < underEst; i += 2){
					if((under%i) == 0)
						underPrime = 0;
				}
				//CRITICAL: If a twin prime found, increment count within lock.
				if(underPrime == 1){
					pthread_mutex_lock(&myLock);
					twinPrimeCount++;
					pthread_mutex_unlock(&myLock);
				}
			}
		}
	}
	return NULL;
}

//Command line processing and thread management
int main(int argc, char *argv[]){
	//Command line argument error processing and error message
	if(argc != 5){	//Check if correct # of args
		printf("Invalid number of arguments entered.\n");
	}
	else if(strcmp(argv[1], "-t") != 0){	//check if first arg is correct
		printf("Invalid argument. '%s' entered instead of '-t'.\n", argv[1]);
	}
	else if(atoi(argv[2]) < 1 || atoi(argv[2]) > 4){	//check range on # of threads arg
		printf("Invalid number of threads entered. Please enter an integer from 1 to 4.\n");
	}
	else if(strcmp(argv[3], "-l") != 0){	//check if third arg is correct
		printf("Invalid argument. '%s' entered instead of '-l'.\n", argv[3]);
	}
	else if(atoi(argv[4]) < 100 || atoi(argv[4]) > 10000000){ //check range on limit arg
		printf("Invalid limit entered. Please enter an integer from 100 to 10,000,000.\n");
	}
	//All arguments valid, run program.
	else{
		limit = atoi(argv[4]);
		int numthreads = atoi(argv[2]);
		printf("\033[0;1mCS 370 Twin Primes Program.\033[0m\n");
		printf("\tHardware Cores: %d\n", get_nprocs());
		printf("\tThread Count: %d\n", numthreads);
		printf("\tPrime Limit: %d\n", limit);
		
		//Initialize thread variables, as well as corresponding error integers
		unsigned long int thdErr1, thdErr2, thdErr3, thdErr4, mtxErr;
		pthread_t thd1, thd2, thd3, thd4;
		
		//Initialize mutex "myLock"
		//Utilization: locking and unlocking of critcal sections for code
		//LOCK: pthread_mutex_lock(&myLock);
		//UNLOCK: pthread_mutex_unlock(&myLock);
		mtxErr = pthread_mutex_init(&myLock, NULL);
		if(mtxErr != 0)
			perror("Mutex initialization failed.\n");
		
		//Thread creation, based on argument parameter. Always at least 1 thread, up to 4.
		thdErr1 = pthread_create(&thd1, NULL, &twinPrims, NULL);			//thread 1
		if(thdErr1 != 0)
			perror("Thread 1 fail to create.\n");
		if(numthreads > 1){
			thdErr2 = pthread_create(&thd2, NULL, &twinPrims, NULL);		//thread 2
			if(thdErr2 != 0)
				perror("Thread 2 fail to create.\n");
		}
		if(numthreads > 2){
			thdErr3 = pthread_create(&thd3, NULL, &twinPrims, NULL);		//thread 3
			if(thdErr3 != 0)
				perror("Thread 3 fail to create.\n");
		}
		if(numthreads > 3){
			thdErr4 = pthread_create(&thd4, NULL, &twinPrims, NULL);		//thread 4
			if(thdErr4 != 0)
				perror("Thread 4 fail to create.\n");
		}
		
		//Wait for threads to complete, only join if previously created.
		pthread_join(thd1, NULL);
		if(numthreads > 1)
			pthread_join(thd2, NULL);
		if(numthreads > 2)
			pthread_join(thd3, NULL);
		if(numthreads > 3)
			pthread_join(thd4, NULL);
		//twinPrimeCount++;
		//twinPrimeCount /= 2;
			
		printf("Count of twin primes between 1 and %d is %d\n", limit, twinPrimeCount);
		return 0;
	}
}
