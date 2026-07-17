#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

































union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int semid;
union semun semarg;


int main(void)
{
    int err;
    struct sembuf sops;

    semid = semget(IPC_PRIVATE, 1, 0600);

    semarg.val = 1;
    semctl(semid, 0, SETVAL, semarg);

    sops.sem_num = 0;
    sops.sem_flg = 0;

    if (fork() == 0) { /* child */
        sops.sem_op = -1;
        semop(semid, &sops, 1);

        printf("child got the lock\n");
        sleep(5); /* critical section actions */
        printf("child releases the lock\n");

        sops.sem_op = 1;
        semop(semid, &sops, 1);

        sleep(6);
        printf("try to delete the semaphore from child\n");

        err = semctl(semid, 0, IPC_RMID, semarg);
        if (err == -1) {
            printf("child delete semaphore error\n");
        }
    } else { /* parent */
        sops.sem_op = -1;
        semop(semid, &sops, 1);

        printf("parent got the lock\n");
        sleep(5); /* critical section actions */
        printf("parent releases the lock\n");

        sops.sem_op = 1;
        semop(semid, &sops, 1);

        sleep(6);
        printf("try to delete the semaphore from parent\n");

        err = semctl(semid, 0, IPC_RMID, semarg);
        if (err == -1) {
            printf("parent delete semaphore error\n");
        }
    }

    return 0;
}
