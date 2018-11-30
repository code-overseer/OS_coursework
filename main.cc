/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/

#include "helper.h"
#include <queue>
using namespace std;

void *producer (void *id);
void *consumer (void *id);

struct Job {
  Job(int id=0, int time=0):job_id(id),job_duration(time){};
  int job_id;
  int job_duration;
};

namespace var {
  int SID=0;
  int job_count=0;
  int queue_size=0;
  int job_num=0;
  int producer_num=0;
  int consumer_num=0;
  volatile int PID=0;
  volatile int CID=0;  
}


int main (int argc, char **argv) {
  try {
    if (argc!=5) {
      cerr<<"Insufficient arguments supplied"<<endl;
      throw -2;
    }
    var::queue_size = check_arg(argv[1]);
    var::job_num = check_arg(argv[2]);
    var::producer_num = check_arg(argv[3]);
    var::consumer_num = check_arg(argv[4]);

    if (var::queue_size<0 ||var::job_num<0||
	var::producer_num<0||var::consumer_num<0) {
      cerr<<"Non-numeric character detected in the command line arguments supplied"<<endl;
      throw -2;
    }
    int *statCreate_p=new int[var::producer_num];
    int *statCreate_c=new int[var::consumer_num];
    int *statJoin_p=new int[var::producer_num];
    int *statJoin_c=new int[var::consumer_num];
    pthread_t producerid[var::producer_num];
    pthread_t consumerid[var::consumer_num];
    vector<Job> job_queue;
 
    srand(time(NULL));
    var::SID=sem_create(SEM_KEY, 4);

    sem_init(var::SID, MUTEX, 1);
    sem_init(var::SID, COUT_MUTEX, 1);
    sem_init(var::SID, QUEUE_SPACE, var::queue_size);
    sem_init(var::SID, QUEUE_ITEM, 0);
    
    for (int i=0; i<var::producer_num; i++) {
      statCreate_p[i]= pthread_create (&producerid[i], NULL, producer, (void *) &job_queue);
    }
    for (int j=0; j<var::consumer_num; j++) {
      statCreate_c[j]=pthread_create (&consumerid[j], NULL, consumer, (void *) &job_queue);
    }
    for (int i=0; i<var::producer_num; i++) {
      statJoin_p[i]=pthread_join (producerid[i], NULL);
      if (statCreate_p[i]) {
	cerr<<pthread_create_error(statCreate_p[i])<<endl;
      }
      if (statJoin_p[i]) {
	cerr<<pthread_join_error(statJoin_p[i])<<endl;
      }
    }
    for (int j=0; j<var::consumer_num; j++) {
      statJoin_c[j]=pthread_join (consumerid[j], NULL);
      if (statCreate_c[j]) {
	cerr<<pthread_create_error(statCreate_c[j])<<endl;
      }
      if (statJoin_p[j]) {
	cerr<<pthread_join_error(statJoin_c[j])<<endl;
      }
    }

    sem_close(var::SID);
    return 0;
    
  } catch (int error) {

    if (error==-1) sem_close(var::SID);

    return -1;
  }  
}

void *producer (void *parameter) {
  vector<Job>* param = ((vector<Job> *) parameter);
  int rc=0;
  int ID=++var::PID;
  for (int i=0; i<var::job_num; i++) {

    if (!semctl(var::SID,QUEUE_SPACE,GETVAL)) {
      sem_wait(var::SID, COUT_MUTEX);
      cout<<"producer("<<ID<<"): Job queue full, waiting for space.."<<endl;
      sem_signal(var::SID, COUT_MUTEX);
    }
    rc=sem_timed_wait(var::SID, QUEUE_SPACE);
    if (rc==-1) {
      sem_wait(var::SID, COUT_MUTEX);
      cout<<"producer("<<ID<<"): Exiting"<<endl;
      sem_signal(var::SID, COUT_MUTEX);
      pthread_exit(0);
    }    
  
    sleep(rand()%4+1);
    sem_wait(var::SID, MUTEX);
    int dur =rand()%9+1;
    var::job_count%=var::queue_size;
    int jid = var::job_count+1;
    var::job_count++;
    Job new_job(jid,dur);
    param->push_back(new_job);
    sem_signal(var::SID, MUTEX);
    sem_signal(var::SID, QUEUE_ITEM);

    sem_wait(var::SID, COUT_MUTEX);    
    cout<<"producer("<<ID<<"): Produced job with ID ";
    cout<<jid<<" and duration of "<<dur<<" seconds"<<endl;
    sem_signal(var::SID, COUT_MUTEX);

  }
  sem_wait(var::SID, COUT_MUTEX);
  cout<<"producer("<<ID<<"): No more jobs to generate"<<endl;
  sem_signal(var::SID, COUT_MUTEX);
  pthread_exit(0);
}

void *consumer (void *parameter) {
  vector<Job>* param = ((vector<Job> *) parameter);
  int rc=0, jid=0, time=0, count=0;
  int ID=++var::CID;
  while (true) {
    if (!semctl(var::SID, QUEUE_ITEM, GETVAL) && count) {
      sem_wait(var::SID, COUT_MUTEX);
      cout<<"consumer("<<ID;
      cout<<"): Job queue empty, waiting for new job.."<<endl;
      sem_signal(var::SID, COUT_MUTEX);
    }
    count++;
    rc=sem_timed_wait(var::SID, QUEUE_ITEM);
    if (rc==-1) {
      sem_wait(var::SID, COUT_MUTEX);
      cout<<"consumer("<<ID<<"): Exiting"<<endl;
      sem_signal(var::SID, COUT_MUTEX);
      pthread_exit(0);
    }
    
    sem_wait(var::SID,MUTEX);
    jid = param->front().job_id;
    time = param->front().job_duration;
    param->erase(param->begin());
    sem_signal(var::SID,MUTEX);
    sem_signal(var::SID, QUEUE_SPACE);

    sem_wait(var::SID, COUT_MUTEX);
    cout<<"consumer("<<ID<<"): Processing job ID ";
    cout<<jid<<", completing in "<<time<<" seconds"<<endl;
    sem_signal(var::SID, COUT_MUTEX);

    sleep(time);
    
    sem_wait(var::SID, COUT_MUTEX);
    cout<<"consumer("<<ID<<"): Job ID ";
    cout<<jid<<" completed"<<endl;
    sem_signal(var::SID, COUT_MUTEX);
  }
}

