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

sem_t rooms;
sem_t guest_check_in, check_in, get_key;
sem_t guest_check_out, check_out, give_key;

void *guest(void *ID);
void *Check_in_res(void *none);
void *Check_out_res(void *none);
void Hotel_activity(int ID);


int main()
{
    //struct activities act_visits = {0, 0, 0, 0};
    srand(time(NULL));
    sem_init(&rooms, 0, 3);

    sem_init(&check_in, 0, 1);
    sem_init(&guest_check_in, 0, 0);
    sem_init(&get_key, 0, 0);

    sem_init(&check_out, 0, 1);
    sem_init(&guest_check_out, 0, 0);
    sem_init(&give_key, 0, 0);

    pthread_t check_in_res, check_out_res;
    pthread_create(&check_in_res, NULL, Check_in_res, NULL);
    pthread_create(&check_out_res, NULL, Check_out_res, NULL);
    pthread_t guests[num_guests];
    int guest_ID[num_guests];

    //This creates all the guests
    for(int i = 0; i < num_guests; i++)
    {
        guest_ID[i] = i;
        pthread_create(&guests[i], NULL, guest, (void *) &guest_ID[i]);
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
    sem_wait(&rooms);
    printf("Guest %d enters the hotel\n", *guest_id);
    //Check_in
    //Mutual exclusion
    sem_wait(&check_in);
    printf("Guest %d goes to the check-in reservationist\n", *guest_id);
    check_in_counter[0] = *guest_id;
    sem_post(&guest_check_in);
    sem_wait(&get_key);

    //Hotel Activity
    Hotel_activity(*guest_id);

    //Check_out
    //Mutual exclusion
    sem_wait(&check_out);
    printf("Guest %d goes to the check-out reservationist\n", *guest_id);
    check_out_counter[0] = *guest_id; 
    sem_post(&guest_check_out);
    sem_wait(&give_key);
    sem_post(&rooms);
    pthread_exit(0);
}

void *Check_in_res(void *none)
{
    
    while(1)
    {
        sem_wait(&guest_check_in);
        printf("The check-in reservationist greets Guest %d\n", check_in_counter[0]);
        while(1)
        {
            room_num = rand() % 3;
            if(rooms_open[room_num] == 'x')
            {
                rooms_open[room_num] = check_in_counter[0];
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
        sem_wait(&guest_check_out);
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
        sem_post(&give_key);
        sem_post(&check_out);
    }
}

void Hotel_activity(int ID)
{
    switch((rand() % 3) + 1)
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