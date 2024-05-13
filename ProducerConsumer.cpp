#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <iostream>
using namespace std;

sem_t pelletMutex;
sem_t pelletEmpty;
sem_t pelletFull;
int counter = 0;

void* producer(void* count)
{
 sem_wait(&pelletEmpty);
 sem_wait(&pelletMutex);
 
 counter++;
cout<<"pellet Produced   ->"<<counter<<endl;

 sem_post(&pelletFull);
 sem_post(&pelletMutex);

}
void* consumer(void* count)
{
 sem_wait(&pelletFull);
 sem_wait(&pelletMutex);
 
 counter--;
 cout<<"pellet Consumed   ->"<<counter<<endl;
 sem_post(&pelletEmpty);
 sem_post(&pelletMutex);   
}


int main()
{
    sem_init(&pelletMutex,0,1);
    sem_init(&pelletEmpty,0,8);
    sem_init(&pelletFull,0,0);

pthread_t pid[2];
for(int i=0;i<5;i++)
{
  pthread_create(&pid[0],NULL,producer,NULL);
  pthread_create(&pid[1],NULL,consumer,NULL);

}
pthread_join(pid[0],NULL);
  pthread_join(pid[1],NULL);
    return 0;
}