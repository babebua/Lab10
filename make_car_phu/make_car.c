#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#define TRUE 1
#define FALSE 0

int isComponentComplete = FALSE;

int isSeatAttached = FALSE;
int isEngineAssembled = FALSE;
int isTiresAttached = FALSE;
int isCoverAttached = FALSE;
int isChasisReady = FALSE;

int isCarReady = FALSE;

int number = 0;

pthread_mutex_t lock;
pthread_mutex_t components_mutex;

pthread_cond_t chasis;
pthread_cond_t components;
pthread_cond_t topcover;
pthread_cond_t painting;
pthread_cond_t done;

void addTires() {
    pthread_mutex_lock(&components_mutex);
    printf("A puts tires\n");
    isTiresAttached = TRUE;
    if (isEngineAssembled && isSeatAttached) {
        isComponentComplete = TRUE;
    }
    pthread_mutex_unlock(&components_mutex);
}

void attachSeats() {
    pthread_mutex_lock(&components_mutex);
    printf("C attaches seats\n");
    isSeatAttached = TRUE;
    if (isEngineAssembled && isTiresAttached) {
        isComponentComplete = TRUE;
    }
    pthread_mutex_unlock(&components_mutex);
}

void assembleEngine() {
    pthread_mutex_lock(&components_mutex);
    printf("D places engine\n");
    isEngineAssembled = TRUE;
    if (isSeatAttached && isTiresAttached) {
        isComponentComplete = TRUE;
    }
    pthread_mutex_unlock(&components_mutex);
}

void addTopCover() {
    isCoverAttached = TRUE;
    printf("D assembles top cover\n");
}

void paintJob() {
    printf("A paints\n\n");
    isCarReady = TRUE;
}

void putChasis() {
    isChasisReady = TRUE;
    printf("Car #%d\n", ++number);
    printf("B puts chassis on the conveyor\n");
}

void sendCar() {
    isComponentComplete = FALSE;
    isSeatAttached = FALSE;
    isEngineAssembled = FALSE;
    isTiresAttached = FALSE;
    isCoverAttached = FALSE;
    isCarReady = FALSE;
    isChasisReady = FALSE;
}

void *A(void *arg) {
    pthread_mutex_lock(&lock);
    while (TRUE) {
        while (!isChasisReady || isTiresAttached) {
            pthread_cond_wait(&components, &lock);
        }
        addTires();
        sleep(1);
        pthread_cond_signal(&topcover);

        while (!isCoverAttached) {
            pthread_cond_wait(&painting, &lock);
        }
        paintJob();
        sleep(1);
        // printf("wake done");
        pthread_cond_signal(&done);
    }
}

void *B(void *arg) {
    pthread_mutex_lock(&lock);
    while (TRUE) {
        // printf("%d\n", isChasisReady);
        while (isChasisReady) {
            pthread_cond_wait(&chasis, &lock);
        }
        putChasis();
        sleep(1);
        pthread_cond_broadcast(&components);

        while (!isCarReady) {
            // printf("done wait\n");
            pthread_cond_wait(&done, &lock);
            // printf("done wake\n");
        }
        // printf("sendCar\n");
        sendCar();
        sleep(1);
    }
}

void *C(void *arg) {
    pthread_mutex_lock(&lock);
    while (1) {
        while (!isChasisReady || isSeatAttached) {
            pthread_cond_wait(&components, &lock);
        }
        attachSeats();
        sleep(1);
        pthread_cond_signal(&topcover);
    }
}

void *D(void *arg) {
    pthread_mutex_lock(&lock);
    while (1) {
        while (!isChasisReady || isEngineAssembled) {
            pthread_cond_wait(&components, &lock);
        }
        assembleEngine();
        sleep(1);
        pthread_cond_signal(&topcover);

        while (!isComponentComplete) {
            pthread_cond_wait(&topcover, &lock);
        }
        addTopCover();
        sleep(1);
        pthread_cond_signal(&painting);
    }
}

int main(int argc, char *argv[]) {
    pthread_mutex_init(&lock, 0);
    pthread_mutex_init(&components_mutex, 0);
    pthread_cond_init(&chasis, 0);
    pthread_cond_init(&components, 0);
    pthread_cond_init(&topcover, 0);
    pthread_cond_init(&painting, 0);
    pthread_cond_init(&done, 0);
    pthread_t thread1, thread2, thread3, thread4;
    pthread_create(&thread1, NULL, A, NULL);
    pthread_create(&thread2, NULL, B, NULL);
    pthread_create(&thread3, NULL, C, NULL);
    pthread_create(&thread4, NULL, D, NULL);

    while (1) {
        sleep(1000);
    }
    return 0;
}
