/******************************************************************
 * Header file for the helper functions. This file includes the
 * required header files, as well as the function signatures and
 * the semaphore values (which are to be changed as needed).
 ******************************************************************/


# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/shm.h>
# include <sys/sem.h>
# include <sys/time.h>
# include <math.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <signal.h>
#include <iostream>

#define SEM_KEY 0x1E // Change this number as needed

#define MUTEX 0
#define COUT_MUTEX 1
#define QUEUE_SPACE 2
#define QUEUE_ITEM 3

union semun {
  int val;               /* used for SETVAL only */
  struct semid_ds *buf;  /* used for IPC_STAT and IPC_SET */
  ushort *array;         /* used for GETALL and SETALL */
};

//struct timespec {
//time_t tv_sec;
//long tv_nsec;
//};

int check_arg (char *);
int sem_create (key_t, int);
void sem_init (int, int, int);
void sem_wait (int, short unsigned int);
int sem_timed_wait (int, short unsigned int);
void sem_signal (int, short unsigned int);
void sem_close (int);
char const* semop_error(int error);
char const* semget_error(int error);
char const* semctl_error(int error);
char const* pthread_create_error(int error);
char const* pthread_join_error(int error);
