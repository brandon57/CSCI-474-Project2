#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

char rooms_open[] = {'x', 'x', 'x'};
int check_in_counter[2];
int check_out_counter[2];
int room_num = 0;
#define num_guests 1

struct activities
{
    int pool;
    int restaurant;
    int fitness_center;
    int business_center;
};
struct activities act_visits = {0, 0, 0, 0};

sem_t rooms;
sem_t guest_check_in, check_in;
sem_t guest_check_out, check_out;

void *guest(void *ID);
void *Check_in_res(void *none);
void *Check_out_res(void *none);
void Hotel_activity(int ID);


int main()
{
    struct activities act_visits = {0, 0, 0, 0};
    
    sem_init(&rooms, 0, 3);

    sem_init(&check_in, 0, 1);
    sem_init(&guest_check_in, 0, 0);

    sem_init(&check_out, 0, 1);
    sem_init(&guest_check_out, 0, 0);

    pthread_t check_in_res, check_out_res;
    pthread_create(&check_in_res, NULL, Check_in_res, NULL);
    pthread_create(&check_out_res, NULL, Check_out_res, NULL);
    pthread_t guests[num_guests];
    int guest_ID[num_guests];



    for(int i = 0; i < num_guests; i++)
    {
        guest_ID[i] = i;
        pthread_create(&guests[i], NULL, guest, (void *) &guest_ID[i]);
    }

    for(int i = 0; i < num_guests; i++)
    {
        pthread_join(guests[i], NULL);
    }

}

void *guest(void *ID)
{
    int *guest_id = (int*) ID;
    sem_wait(&rooms);
    printf("\n");
    printf("Guest %d enters the hotel\n", *guest_id);
    //Check_in
    //Mutual exclusion
    // sem_wait(&check_in);
    sem_wait(&check_in);
    printf("Guest %d goes to the check-in reservationist\n", *guest_id);
    check_in_counter[0] = *guest_id;
    //check_in_counter[1] = 
    sem_post(&guest_check_in);
    
    //Guest gets room

    //Hotel Activity
    Hotel_activity(*guest_id);

    // //Check_out
    // //Mutual exclusion
    sem_wait(&check_out);
    printf("Guest %d goes to the check-out reservationist\n", *guest_id);
    check_out_counter[0] = *guest_id;
    //check_in_counter[1] = 
    sem_post(&guest_check_out);
    sem_post(&rooms);
    pthread_exit(0);
}

void *Check_in_res(void *none)
{
    srand(time(NULL));
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
        sem_post(&check_in);
    }
}

void *Check_out_res(void *none)
{
    srand(time(NULL));
    while(1)
    {
        sem_wait(&guest_check_out);
        for(int i = 0; i < sizeof(rooms_open); i++)
        {
            if(rooms_open[i] == check_out_counter[0])
            {
                printf("The check-out reservationist greets Guest %d and receives the key from room %d\n", check_out_counter[0], i);
                rooms_open[i] = 'x';
                printf("The receipt was printed\n");
                break;
            }
        }
        sem_post(&check_out);
    }
}


// int check_in(int ID)
// {
//     printf("The check-in reservationist greets Guest #%d\n", ID);
//     int Room_num = 0;
//     return Room_num;
// }

void Hotel_activity(int ID)
{
    //srand(time(NULL));
    //printf("test\n");
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

// void check_Out()
// {

// }