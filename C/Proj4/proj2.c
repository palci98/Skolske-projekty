#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <time.h>
#include <signal.h>

#define SEMAPHORE1_NAME "/semaphore_synchronization1"
#define SEMAPHORE2_NAME "/semaphore_synchronization2"
#define SEMAPHORE3_NAME "/semaphore_synchronization3"
#define SEMAPHORE4_NAME "/semaphore_synchronization4"
#define SEMAPHORE5_NAME "/semaphore_synchronization5"
#define LOCKED 0
#define UNLOCKED 1

FILE *fp;
sem_t *semaphore1 = NULL;
sem_t *semaphore2 = NULL;
sem_t *semaphore3 = NULL;
sem_t *semaphore4 = NULL;
sem_t *semaphore5 = NULL;
int waitingId=0;
int *waiting=NULL;
int allridersId=0;
int *allriders=NULL;
int *sharedCounter = NULL;
int sharedCounterId = 0;
void create_resources() {
    fp = fopen("proj2.out","w");

    if ((sharedCounterId = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1)
    {
        // handle error
    }

    if ((sharedCounter = shmat(sharedCounterId, NULL, 0)) == NULL)
    {
        // handle error
    }
    if ((waitingId = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1)
    {
        // handle error
    }

    if ((waiting = shmat(waitingId, NULL, 0)) == NULL)
    {
        // handle error
    }

    if ((allridersId = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1)
    {
        // handle error
    }

    if ((allriders = shmat(allridersId, NULL, 0)) == NULL)
    {
        // handle error
    }

    if ((semaphore1 = sem_open(SEMAPHORE1_NAME, O_CREAT | O_EXCL, 0666, LOCKED)) == SEM_FAILED)
    {
        // handle error
    }

    if ((semaphore2 = sem_open(SEMAPHORE2_NAME, O_CREAT | O_EXCL, 0666, LOCKED)) == SEM_FAILED)
    {
        // handle error
    }
    if ((semaphore3 = sem_open(SEMAPHORE3_NAME, O_CREAT | O_EXCL, 0666, UNLOCKED)) == SEM_FAILED)
    {
        // handle error
    }

    if ((semaphore4 = sem_open(SEMAPHORE4_NAME, O_CREAT | O_EXCL, 0666, UNLOCKED)) == SEM_FAILED)
    {
        // handle error
    }
    if ((semaphore5 = sem_open(SEMAPHORE5_NAME, O_CREAT | O_EXCL, 0666, LOCKED)) == SEM_FAILED)
    {
        // handle error
    }
}
void clean_resources()
{
    sem_unlink(SEMAPHORE1_NAME);
    sem_unlink(SEMAPHORE2_NAME);
    sem_unlink(SEMAPHORE3_NAME);
    sem_unlink(SEMAPHORE4_NAME);
    sem_unlink(SEMAPHORE5_NAME);


    sem_close(semaphore1);
    sem_close(semaphore2);
    sem_close(semaphore3);
    sem_close(semaphore4);
    sem_close(semaphore5);

    shmctl(allridersId, IPC_RMID, NULL);
    shmctl(waitingId, IPC_RMID, NULL);
    shmctl(sharedCounterId, IPC_RMID, NULL);
    fclose(fp);
}
void processrider(int j) {
    sem_wait(semaphore4);
    (*sharedCounter)++;
    fprintf(fp,"%d        :RID %d       :start \n",*sharedCounter,j);
    sem_post(semaphore4);
    (*waiting)++;
    (*allriders)--;
    sem_wait(semaphore4);
    (*sharedCounter)++;
    fprintf(fp,"%d        :RID %d        :enter:%d\n",*sharedCounter,j,j);
    sem_post(semaphore4);
    if (*waiting > 0) {
        sem_wait(semaphore4);
        (*sharedCounter)++;
        fprintf(fp,"%d        :RID %d       :boarding\n",*sharedCounter,j);
        sem_post(semaphore4);
    }
    sem_wait(semaphore4);
    (*sharedCounter)++;
    fprintf(fp,"%d        :RID %d        :finish\n",*sharedCounter,j);
    sem_post(semaphore4);
}
void processbus(int T){
    sem_wait(semaphore4);
    (*sharedCounter)++;
    fprintf(fp,"%d        :BUS        :start\n",*sharedCounter);
    sem_post(semaphore4);
    while(*allriders > 0){
        sem_wait(semaphore2);
        if(*waiting > 0){
            sem_wait(semaphore4);
            (*sharedCounter)++;
            fprintf(fp,"%d        :bus        :arrival\n",*sharedCounter );
            sem_post(semaphore4);
            sem_wait(semaphore4);
            (*sharedCounter)++;
            fprintf(fp,"%d        :bus        :start\n",*sharedCounter);
            sem_post(semaphore4);
            sem_wait(semaphore4);
            (*sharedCounter)++;
            fprintf(fp,"%d        :bus        :depart\n",*sharedCounter);
            sem_post(semaphore4);
            sem_post(semaphore3);
        }
        else{
            sem_wait(semaphore4);
            (*sharedCounter)++;
            fprintf(fp,"%d        :bus        :arrival\n",*sharedCounter);
            sem_post(semaphore4);
            sem_wait(semaphore4);
            (*sharedCounter)++;
            fprintf(fp,"%d        :bus        :depart\n",*sharedCounter);
            sem_post(semaphore4);
            usleep(rand() %T*1000);
            sem_wait(semaphore4);
            (*sharedCounter)++;
            fprintf(fp,"%d        :BUS        :end\n",*sharedCounter);
            sem_post(semaphore4);
        }
    }
    sem_wait(semaphore4);
    (*sharedCounter)++;
    fprintf(fp,"%d        :BUS        :finish\n",*sharedCounter);
    sem_post(semaphore4);
}
void ridersgen(int sleeping) {
    for (int i = 0; i < *allriders; i++) {
        int I=i+1;
        pid_t RC = fork();
        if (RC == 0) {
            usleep(rand() %sleeping*1000);
            processrider(I);
        }

        if(RC > 0){

        }

        if(RC < 0) {
            fprintf(stderr,"Chyba\n");
            exit(1);
        }
    }
}

int main(int argc, char* argv[]) {
    create_resources();
    int narg = argc;
    if (narg != 5) {
        fprintf(stderr, "Error, number of arguments must be 4");
        return (EXIT_FAILURE);
    }
    setbuf(fp, NULL);
    *allriders = atoi(argv[1]);
    int capacity = atoi(argv[2]);
    int timegen = atoi(argv[3]);
    int timer = atoi(argv[4]);

    if (*allriders <= 0) {
        fprintf(stderr, "Error, 1st argument must be >0\n");
        return 1;
    }
    if (capacity <= 0) {
        fprintf(stderr, "Error, 2nd argument must be >0\n");
        return 1;
    }
    if (timegen < 0 || timegen > 1000) {
        fprintf(stderr, "Error, 3rd argument must be in interval <0,1000>\n");
        return 1;
    }
    if (timer < 0 || timer > 1000){
        fprintf(stderr, "Error, 4th argument must be in interval <0,1000>\n");
        return 1;
    }
    pid_t rider=fork();
    if(rider > 0){

    }
    if (rider <0){
        perror("Fork failed");
    }
    if(rider == 0){
        ridersgen(timegen);
        }


    pid_t bus= fork();
    if(bus < 0){
        perror("Fork failed");
    }
    if(bus == 0) {
        processbus(timer);
    }
    if(bus > 0){

    }

    clean_resources();
    return 0;
 }
