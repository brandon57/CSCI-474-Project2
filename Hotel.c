#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

//Global variables
char rooms_open[] = {'x', 'x', 'x'};
int check_in_counter[2];
int check_out_counter[2];
int room_num = 0;
#define num_guests 5

struct activities
{
    int pool;
    int restaurant;
    int fitness_center;
    int business_center;
};
struct activities act_visits = {0, 0, 0, 0};

//Semaphores
sem_t rooms;
sem_t check_in_wait, check_in, get_key;
sem_t check_out_wait, check_out, get_receicpt;

void *guest(void *ID);
void *Check_in_res(void *none);
void *Check_out_res(void *none);
void Hotel_activity(int ID);


int main()
{
    srand(time(NULL));
    sem_init(&rooms, 0, 3);

    sem_init(&check_in, 0, 1);
    sem_init(&check_in_wait, 0, 0);
    sem_init(&get_key, 0, 0);

    sem_init(&check_out, 0, 1);
    sem_init(&check_out_wait, 0, 0);
    sem_init(&get_receicpt, 0, 0);

    //Check in and out reservationists
    pthread_t check_in_res, check_out_res;
    pthread_create(&check_in_res, NULL, Check_in_res, NULL);
    pthread_create(&check_out_res, NULL, Check_out_res, NULL);
    
    pthread_t guests[num_guests];
    int guest_ID[num_guests];
    int thread;

    //This creates all the guests
    for(int i = 0; i < num_guests; i++)
    {
        guest_ID[i] = i;
        thread = pthread_create(&guests[i], NULL, guest, (void *) &guest_ID[i]);
        if(thread)
        {
            printf("ERROR: Counldn't create thread %d", guest_ID[i]);
            exit(-1);
        }
    }

    //This waits till all the guest threads are done
    for(int i = 0; i < num_guests; i++)
    {
        pthread_join(guests[i], NULL);
    }

    //Summary
    printf("\n");
    printf("Summary:\n");
    printf("--------------------------------\n");
    printf("Total Guests: %d\n", num_guests);
    printf("Visits to pool: %d\n", act_visits.pool);
    printf("Visits to restaurant: %d\n", act_visits.restaurant);
    printf("Visits to fitness center: %d\n", act_visits.fitness_center);
    printf("Visits to business center: %d\n", act_visits.business_center);
    printf("--------------------------------\n");

}

void *guest(void *ID)
{
    int *guest_id = (int*) ID;
    int room;
    sem_wait(&rooms);
    printf("Guest %d enters the hotel\n", *guest_id);

    //Check_in
    //Mutual exclusion
    sem_wait(&check_in);
    printf("Guest %d goes to the check-in reservationist\n", *guest_id);
    check_in_counter[0] = *guest_id;
    sem_post(&check_in_wait);
    sem_wait(&get_key);
    room = check_in_counter[1];
    printf("Guest %d receives room %d and completes check-in\n", *guest_id, room);

    //Hotel Activity
    Hotel_activity(*guest_id);

    //Check_out
    //Mutual exclusion
    sem_wait(&check_out);
    printf("Guest %d goes to the check-out reservationist and returns room %d\n", *guest_id, room);
    check_out_counter[0] = *guest_id; 
    sem_post(&check_out_wait);
    sem_wait(&get_receicpt);
    printf("Guest %d receives the receipt\n", *guest_id);
    sem_post(&rooms);
    pthread_exit(0);
}

void *Check_in_res(void *none)
{
    while(1)
    {
        sem_wait(&check_in_wait);
        printf("The check-in reservationist greets Guest %d\n", check_in_counter[0]);
        while(1)
        {
            room_num = rand() % 3;
            if(rooms_open[room_num] == 'x')
            {
                rooms_open[room_num] = check_in_counter[0];
                check_in_counter[1] = room_num;
                break;
            }
        }
        printf("Check-in reservationist assigns room %d to Guest %d\n", room_num, check_in_counter[0]);
        sem_post(&get_key);
        sem_post(&check_in);
    }
}

void *Check_out_res(void *none)
{
    while(1)
    {
        sem_wait(&check_out_wait);
        for(int i = 0; i < sizeof(rooms_open); i++)
        {
            if(rooms_open[i] == check_out_counter[0])
            {
                printf("The check-out reservationist greets Guest %d and receives the key from room %d\n", check_out_counter[0], i);
                rooms_open[i] = 'x';
                printf("The receipt was printed for Guest %d\n", check_out_counter[0]);
                break;
            }
        }
        sem_post(&get_receicpt);
        sem_post(&check_out);
    }
}

void Hotel_activity(int ID)
{
    switch((rand() % 4))
    {
        case 0:
            printf("Guest %d goes to the swimming pool\n", ID);
            act_visits.pool++;
            break;
        case 1:
            printf("Guest %d goes to the restaurant\n", ID);
            act_visits.restaurant++;
            break;
        case 2:
            printf("Guest %d goes to the fitness center\n", ID);
            act_visits.fitness_center++;
            break;
        case 3:
            printf("Guest %d goes to the business center\n", ID);
            act_visits.business_center++;
            break;
    }
    sleep((rand() % 2) + 1);
}