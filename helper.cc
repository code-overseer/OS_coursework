/******************************************************************
 * The helper file that contains the following helper functions:
 * check_arg - Checks if command line input is a number and returns it
 * sem_create - Create number of sempahores required in a semaphore array
 * sem_init - Initialise particular semaphore in semaphore array
 * sem_wait - Waits on a semaphore (akin to down ()) in the semaphore array
 * sem_signal - Signals a semaphore (akin to up ()) in the semaphore array
 * sem_close - Destroy the semaphore array
 ******************************************************************/

# include "helper.h"
using namespace std;

int check_arg (char *buffer)
{
  int i, num = 0, temp = 0;
  if (strlen (buffer) == 0)
    return -1;
  for (i=0; i < (int) strlen (buffer); i++)
  {
    temp = 0 + buffer[i];
    if (temp > 57 || temp < 48)
      return -1;
    num += pow (10, strlen (buffer)-i-1) * (buffer[i] - 48);
  }
  return num;
}

int sem_create (key_t key, int num) {
  int id;
  if ((id = semget (key, num,  0666 | IPC_CREAT | IPC_EXCL)) < 0) {
    
    throw -1;
  }
  return id;
}

void sem_init (int id, int num, int value) {
  union semun semctl_arg;
  semctl_arg.val = value;
  if (semctl (id, num, SETVAL, semctl_arg) < 0) {
    cerr<<semctl_error(errno)<<endl;
    throw -1;
  }
  return;
}

void sem_wait (int id, short unsigned int num) {
  struct sembuf op[] = {
    {num, -1, SEM_UNDO}
  };
  if (semop (id, op, 1)<0) {
    cerr<<semop_error(errno)<<endl;
    throw -1;
  }
  return;
}

void sem_signal (int id, short unsigned int num) {
  struct sembuf op[] = {
    {num, 1, SEM_UNDO}
  };
  if (semop (id, op, 1)<0) {
    cerr<<semop_error(errno)<<endl;
    throw -1;
  }
  return;
}

void sem_close (int id) {
  if (semctl (id, 0, IPC_RMID, 0) < 0) {
    cerr<<semctl_error(errno)<<endl;
    throw -1;
  }
  return;
}

int sem_timed_wait (int id, short unsigned int num) {
  struct sembuf op[] = {
    {num, -1, SEM_UNDO}
  };
  struct timespec ts[] = {
    {20,0}
  };

  if (semtimedop (id, op, 1, ts)<0) {
    if (errno==EAGAIN) return -1;
    cerr<<semop_error(errno)<<endl;
    throw -1;
  }
  return 0;
}

char const* semget_error(int error) {
  switch (error) {
  case EACCES:
    return "Semaphore with the specified key already exists; access permission denied";
  case EEXIST:
    return "Semaphore with the specified key already exists";
  case EINVAL:
    return "The number of semaphores requested is out of range";
  case ENOENT:
    return "IPC_CREAT not specified";
  case ENOMEM:
    return "System has insufficient memory";
  case ENOSPC:
    return "System already has too many semaphores";
  default:
    return "Undefined Error Detected";
  }
}

char const* semctl_error(int error) {
  switch (error) {
  case EACCES:
    return "Operation failed, permission denied";
  case EFAULT:
    return "Segmentation fault, semctl_arg member inaaccessible";
  case EIDRM:
    return "Semaphore set does not exist";
  case EINVAL:
    return "Invalid semaphore ID or command argument detected";
  case EPERM:
    return "User does not have permission to access the semaphore set";
  case ERANGE:
    return "The number of semaphores requested is out of range";
  default:
    return "Undefined Error Detected";
  }
}

char const* semop_error(int error) {
  switch (error) {
  case E2BIG:
    return "Too many number of operations specified";
  case EACCES:
    return "Semaphore operation permission denied";
  case EAGAIN:
    return "Operation cannot proceed immediately; IPC_NOWAIT was specified";
  case EFAULT:
    return "Segmentation fault; address of one of the arguments is inaccessible";
  case EFBIG:
    return "Semaphore index out of range";
  case EIDRM:
    return "Semaphore set has been removed";
  case EINTR:
    return "Thread interrupted by a signal";
  case EINVAL:
    return "The semaphore set does not exist or invalid"
      "semaphore ID or number of operations is supplied";
  case ENOMEM:
    return "Insufficient Memory";
  case ERANGE:
    return "The resulting semaphore value followed by the operation specified is too large";
  default:
    return "Undefined Error Detected";
  }
}

char const* pthread_create_error(int error) {
  switch (error) {
  case EAGAIN:
    return "Thread limit reached";
  case EINVAL:
    return "Invalid argument passed";
  case EPERM:
    return "No permission to set the scheduling policy and parameters specified";
  default:
    return "Undefined Error Detected";
  }
}

char const* pthread_join_error(int error) {
  switch (error) {
  case EDEADLK:
    return "Deadlock detected";
  case EINVAL:
    return "The thread specified is not joinable or another thread is waiting to join with it";
  case ESRCH:
    return "Thread ID not found";
  default:
    return "Undefined Error Detected";
  }
}
