#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>
using namespace std;

sem_t chopSticks[5];

void *Eats(void *ptr)
{
    int *philosopherID = (int *)ptr;
    while (true)
    {
        if (*philosopherID % 2 != 0)
        {
            sem_wait(&chopSticks[*philosopherID]);
            sem_wait(&chopSticks[(*philosopherID + 1) % 5]);

            cout << "Philosopher odd " << *philosopherID << " Eats \n";
            sleep(1);

            sem_post(&chopSticks[*philosopherID]);
            sem_post(&chopSticks[(*philosopherID + 1) % 5]);
        }
        else
        {
            sem_wait(&chopSticks[(*philosopherID + 1) % 5]);
            sem_wait(&chopSticks[*philosopherID]);

            cout << "Philosopher even " << *philosopherID << " Eats \n";
            sleep(1);

            sem_post(&chopSticks[(*philosopherID + 1) % 5]);
            sem_post(&chopSticks[*philosopherID]);
        }
    }
}

int main()
{
    pthread_t philosophers[5];
    int philosopherIDs[] = {0, 1, 2, 3, 4};

    for (int i = 0; i < 5; i++)
        sem_init(&chopSticks[i], 0, 1);

    for (int i = 0; i < 5; i++)
        pthread_create(&philosophers[i], NULL, Eats, &philosopherIDs[i]);

    for (int i = 0; i < 5; i++)
        pthread_join(philosophers[i], NULL);

    return 0;
}
