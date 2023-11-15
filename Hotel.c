#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t rooms, check_in, check_out;

void *guest(void);

int main()
{
    sem_init(&rooms, 0, 2);
    sem_init(&check_in, 0, 1);
    sem_init(&check_out, 0, 1);

    pthread_t guests[5];


    for(int i = 0; i < 4; i++)
    {
        guests[i] = i;
        pthread_create(&guests[i], NULL, pr)
    }

}

void *guests(void )
{
    //Check_in
    //Mutual exclusion
    sem_wait(&check_in);
    printf("The check-in reservationist greets Guest %d\n", i);

    sem_post(&check_in);

    //Hotel Activity
    sem_wait(&rooms);
    printf("");

    //Check_out
    //Mutual exclusion
    
    pthread_exit(0);
}
