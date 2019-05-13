#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0

int tireSeatEngineReady = FALSE;

int isTireInLine = FALSE;
int isSprayInLine = FALSE;
int isChasisInLine = FALSE;
int isSeatInLine = FALSE;
int isEngineInLine = FALSE;
int isRoofInLine = FALSE;

int carReady = FALSE;
int carNumber = 0;

pthread_mutex_t lock;
pthread_mutex_t tireSeatEngine_mutex;

pthread_cond_t tireSeatEngine;
pthread_cond_t roof;
pthread_cond_t spray;
pthread_cond_t chasis;
pthread_cond_t carIsDone;

void addChasis() {
    isChasisInLine += 1;
    printf("Car #%d\n", ++carNumber);
    printf("B puts chassis on the conveyor\n");
}

void addTire() {
    pthread_mutex_lock(&tireSeatEngine_mutex);
    isTireInLine += 1;
    printf("A puts tires\n");
    if (isSeatInLine && isEngineInLine)
        tireSeatEngineReady = TRUE;
    pthread_mutex_unlock(&tireSeatEngine_mutex);
}

void addSeat() {
    pthread_mutex_lock(&tireSeatEngine_mutex);
    printf("C attaches seats\n");
    isSeatInLine += 1;
    if (isTireInLine && isEngineInLine)
        tireSeatEngineReady = TRUE;
    pthread_mutex_unlock(&tireSeatEngine_mutex);
}

void addEngine() {
    pthread_mutex_lock(&tireSeatEngine_mutex);
    printf("D places engine\n");
    isEngineInLine += 1;
    if (isTireInLine && isSeatInLine)
        tireSeatEngineReady = TRUE;
    pthread_mutex_unlock(&tireSeatEngine_mutex);
}

void addRoof() {
    isRoofInLine += 1;
    printf("D assembles top cover\n");
}

void addSpray() {
    isSprayInLine += 1;
    printf("A paints\n\n");
    carReady = TRUE;
}

void sendCar() {
    tireSeatEngineReady = FALSE;
    isSeatInLine = FALSE;
    isEngineInLine = FALSE;
    isTireInLine = FALSE;
    isRoofInLine = FALSE;
    isSprayInLine = FALSE;
    carReady = FALSE;
    isChasisInLine = FALSE;
}

void *chasisAdder(void *arg) {
    pthread_mutex_lock(&lock);
    while (TRUE) {
        while (isChasisInLine) {
            pthread_cond_wait(&chasis,
                              &lock); // use chasis because wait chasis to
                                      // install before do anything else
        }
        addChasis();
        sleep(1);
        pthread_cond_broadcast(&tireSeatEngine);

        while (!carReady)
            pthread_cond_wait(&carIsDone, &lock);

        sendCar();
        sleep(1);
    }
}

void *tireAndSprayAdder(void *arg) {
    pthread_mutex_lock(&lock);
    while (TRUE) {
        while (!isChasisInLine || isTireInLine)
            pthread_cond_wait(&tireSeatEngine, &lock);

        addTire();
        sleep(1);
        pthread_cond_signal(&roof);

        while (!isRoofInLine)
            pthread_cond_wait(&spray, &lock);

        addSpray();
        sleep(1);
        pthread_cond_signal(&carIsDone);
    }
}

void *seatAdder(void *arg) {
    pthread_mutex_lock(&lock);
    while (1) {
        while (!isChasisInLine || isSeatInLine)
            pthread_cond_wait(&tireSeatEngine, &lock);

        addSeat();
        sleep(1);
        pthread_cond_signal(&roof);
    }
}

void *engineAndRoofAdder(void *arg) {
    pthread_mutex_lock(&lock);
    while (1) {
        while (!isChasisInLine || isEngineInLine)
            pthread_cond_wait(&tireSeatEngine, &lock);

        addEngine();
        sleep(1);
        pthread_cond_signal(&roof);

        while (!tireSeatEngineReady)
            pthread_cond_wait(&roof, &lock);

        addRoof();
        sleep(1);
        pthread_cond_signal(&spray);
    }
}

int main(int argc, char *argv[]) {
    pthread_mutex_init(&lock, 0);
    pthread_mutex_init(&tireSeatEngine_mutex, 0);

    pthread_cond_init(&tireSeatEngine, 0);
    pthread_cond_init(&roof, 0);
    pthread_cond_init(&spray, 0);
    pthread_cond_init(&chasis, 0);
    pthread_cond_init(&carIsDone, 0);

    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;
    pthread_t thread4;

    pthread_create(&thread1, NULL, tireAndSprayAdder, NULL);
    pthread_create(&thread2, NULL, chasisAdder, NULL);
    pthread_create(&thread3, NULL, seatAdder, NULL);
    pthread_create(&thread4, NULL, engineAndRoofAdder, NULL);

    while (1)
        sleep(1000);

    return 0;
}
