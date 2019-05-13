#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#define TRUE 1
#define FALSE 0
int numBatterInBowl = 0;
int numEggInBowl = 0;
int readyToEat = FALSE;
pthread_mutex_t lock;
pthread_cond_t needIngredients;
pthread_cond_t readyToBake;
pthread_cond_t startEating;
void addBatter()
{
  numBatterInBowl += 1;
  printf("Added batter\n");
}
void addEgg()
{
  numEggInBowl += 1;
  printf("Added egg\n");
}
void heatBowl()
{
  readyToEat = TRUE;
  numBatterInBowl = 0;
  numEggInBowl = 0;
  printf("Heated bowl\n");
}
void eatCake()
{
  readyToEat = FALSE;
  printf("Anyway, this cake is great.\n"
         "It’s so delicious and moist.\n");
}
void *batterAdder(void *arg)
{
  pthread_mutex_lock(&lock);
  while (1)
  {
    while (numBatterInBowl || readyToEat)
    {
      pthread_cond_wait(&needIngredients, &lock);
    }
    addBatter(); // Sets numBatterInBowl += 1
    sleep(1);
    pthread_cond_signal(&readyToBake);
  }
}
void *eggBreaker(void *arg)
{
  pthread_mutex_lock(&lock);
  while (1)
  {
    while (numEggInBowl >= 2 || readyToEat)
    {
      pthread_cond_wait(&needIngredients, &lock);
    }
    addEgg(); // Sets numEggInBowl += 1
    sleep(1);
    pthread_cond_signal(&readyToBake);
  }
}
void *bowlHeater(void *arg)
{
  pthread_mutex_lock(&lock);
  while (1)
  {
    while (numBatterInBowl != 1 || numEggInBowl != 2)
    {
      pthread_cond_wait(&readyToBake, &lock);
    }
    heatBowl(); // Sets readyToEat = true, numBatterInBowl = 0, numEggInBowl = 0
    sleep(1);
    pthread_cond_signal(&startEating);
  }
}
void *cakeEater(void *arg)
{
  pthread_mutex_lock(&lock);
  while (1)
  {
    while (!readyToEat)
    {
      pthread_cond_wait(&startEating, &lock);
    }
    eatCake(); // Sets readyToEat = false and cleans the bowl for another cake
    sleep(1);
    pthread_cond_broadcast(&needIngredients);
  }
}

int main(int argc, char *argv[])
{
  pthread_mutex_init(&lock, 0);
  pthread_cond_init(&needIngredients, 0);
  pthread_cond_init(&readyToBake, 0);
  pthread_cond_init(&startEating, 0);
  pthread_t thread1, thread2, thread3, thread4, thread5;
  pthread_create(&thread1, NULL, batterAdder, NULL);
  pthread_create(&thread2, NULL, eggBreaker, NULL);
  pthread_create(&thread3, NULL, eggBreaker, NULL);
  pthread_create(&thread4, NULL, bowlHeater, NULL);
  pthread_create(&thread5, NULL, cakeEater, NULL);
  while (1)
  {
    sleep(1000);
  }
  return 0;
}
