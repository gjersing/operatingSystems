#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdbool.h>

#define STUDENT_COUNT_MIN 2
#define STUDENT_COUNT_MAX 10
#define CHAIR_COUNT 3
#define HELPS_MAX 3

// Declare Semaphores
sem_t student_sem;
sem_t professor_sem;
sem_t chair_sem;

pthread_mutex_t lock;

int chairCount = CHAIR_COUNT;
int studentCount;
int studentsRemaining;

// printf("\033[0;31mI’m a message in red\033[0m\n", 0); // red
// printf("\033[0;92mI’m a message in green\033[0m\n", 0); // green
// printf("\033[0;94mI’m a message in blue\033[0m\n", 0); // blue
// printf("\033[0;93mI’m a message in yellow\033[0m\n", 0); // yellow

int isValid(int input)
{
    if (input < STUDENT_COUNT_MIN || input > STUDENT_COUNT_MAX)
    {
        // Throw Input Error
        printf("Error, invalid Student Count.\n");
        exit(0);
    }
    return true;
}
int getInput()
{
    int input;
    printf("\nHow many students coming to professor's office?: ");
    scanf("%d", &input);

    // Validate input
    if (!isValid(input))
    {
        exit(0);
    }
    return input;
}

void *professor(void *args)
{
    // Loop forever
    while (1)
    {
        sem_wait(&student_sem);

        // Help all waiting students in a loop
        while (1)
        {
            if (chairCount == CHAIR_COUNT)
            {
                // All chairs are empty, wait for a student
                break;
            }
            // decrement chair count (must use mutex)
            // Chair has been vacated, freeing one
            pthread_mutex_lock(&lock);
            chairCount++;
            pthread_mutex_unlock(&lock);

            // Student vacates chair (applicable chair semaphore)
            sem_post(&chair_sem);

            // Display message (see example)
            printf("\033[0;92mStudent frees chair and enters professors office. Remaining chairs %d\033[0m\n", chairCount);

            // Professor helps the student (random amount of time)
            sem_post(&professor_sem);
            pthread_mutex_lock(&lock);
            // Random amount of time: usleep(rand() % 1500000);
            printf("\033[0;92mProfessor is helping a student.\033[0m\n");
            usleep(rand() % 2000000);
            studentsRemaining--;
            pthread_mutex_unlock(&lock);

            // If all students have been helped (all helps counts 0). Exit thread
            if (studentsRemaining <= 0)
            {
                break;
            }
        }
        if (studentsRemaining <= 0)
        {
            pthread_exit(NULL);
        }
    }
    // Professor leaves
    pthread_exit(NULL);
}
void *student(void *args)
{
    // Loop forever
    while (1)
    {
        // Display message that student is working on the assignment (see example)
        printf("\033[0;93mStudent %d doing assignment.\033[0m\n", *(int *)args);

        // Student works on the assignment for a random amount of time
        usleep(rand() % 2000000);

        // Student decides to ask for help (display message)
        printf("\033[0;31mStudent %d needs help from the professor.\033[0m\n", *(int *)args);

        // Get chair count (must use mutex)
        sem_wait(&chair_sem);

        // If a chair is available
        if (chairCount > 0)
        {
            if (chairCount == CHAIR_COUNT)
            {
                // If all chairs available
                // WAKEY WAKEY HELPPPPP
                // awaken the sleeping professor
                printf("\033[0;92mProfessor has been awakened by a student.\033[0m\n");
            }

            // Occupy a chair
            // update chair count (must use mutex)
            pthread_mutex_lock(&lock);
            chairCount--;
            pthread_mutex_unlock(&lock);

            // wait until professor is available to help the student (applicable chair semaphore)
            sem_post(&student_sem);
            sem_post(&chair_sem);

            // wait until professor is done helping
            sem_wait(&professor_sem);
            // when professor helps, display message
            printf("\033[0;31mStudent %d is getting help from the professor.\033[0m\n", *(int *)args);
            pthread_exit(NULL);
            // decrement the help count for that student
            //int index = *(int *)args;
        }
        else
        {
            // If no chairs available
            // Display message, continue working on the assignment
            printf("\033[0;31mChairs occupied, student %d will return later.\033[0m\n", *(int *)args);
            sem_post(&chair_sem);
            pthread_mutex_unlock(&lock);
        }
    }
    // Students all helped
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{

    usleep(rand() % 2000000);

    // Display Header
    printf("\033[0;1mCS 370 - Sleeping Professor Project\033[0m\n");
    // Get student count
    studentCount = getInput();
    studentsRemaining = studentCount;
    // Dynamically create student help array
    int *studentHelp = malloc(sizeof(int) * studentCount);
    ;
    // Populate array
    for (int i = 0; i < studentCount; i++)
    {
        studentHelp[i] = (rand() % HELPS_MAX + 1);
    }

    // Initialize Semaphores with values
    sem_init(&student_sem, 0, 0);
    sem_init(&professor_sem, 0, 0);
    sem_init(&chair_sem, 0, 1);

    // Initialize Mutex Lock
    pthread_mutex_init(&lock, NULL);

    // Create one professor thread
    pthread_t professorThread;
    if (pthread_create(&professorThread, NULL, &professor, NULL))
    {
        perror("Failed to create professor thread.");
    }

    // Allocate student thread array
    pthread_t *students = malloc(sizeof(pthread_t) * studentCount);
    // Populate Array
    for (int i = 0; i < studentCount; i++)
    {
        int *threadIndex = malloc(sizeof(*threadIndex));
        *threadIndex = i + 1;
        if (pthread_create(&students[i], NULL, &student, threadIndex))
        {
            perror("Failed to student create thread.");
        }
    }

    // Join together
    pthread_join(professorThread, NULL);
    for (int i = 0; i < studentCount; i++)
    {
        pthread_join(students[i], NULL);
    }
    printf("\033[0;94mAll students assisted, professor is leaving.\033[0m\n");
    exit(0);
}
