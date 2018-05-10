#include "worker.h"

using namespace std;

void *worker_thread(void *arg){
  Worker* worker = (Worker*) arg;

  while((!worker->threadExit)){
    sem_wait(worker->semId);
    cout << "Worker " << worker->id << " starting" <<endl;
    for(int i = 0; i<worker->workPerJob; ++i){
      int** mat = worker->doWork(rand());

      worker->delete_mat(mat);

    }

    worker->jobsDone++;
    cout << "Job done for " << worker->id << endl;
  }

  pthread_exit(NULL);
}

Worker::Worker(int i){
  id = i;
  jobsDone = 0;
  threadExit = false;
  workPerJob = jobRate[id];
  string stringSemName = "worker_sem_" + to_string(id);
  semName = stringSemName.c_str();
  semId = sem_open(semName, O_CREAT, 0600, 0);
}

Worker::~Worker(){
  sem_unlink(semName);
}

int Worker::get_id(){
  return id;
}

int Worker::get_completed_jobs(){
  return jobsDone;
}

void Worker::add_job(){
  sem_post(semId);
}

int Worker::get_remaining_jobs(){
  int remaining;
  sem_getvalue(semId, &remaining);
  return remaining;
}

bool Worker::is_busy(){
  return (get_remaining_jobs() != 0);
}

void Worker::set_exit(){
  threadExit = true;
}

int** Worker::doWork(int n){
  int **mat = new int*[MAT_ROWS];
  //memory allocation
  for(int i=0; i<MAT_ROWS;++i){
    mat[i] = new int[MAT_COLS];
  }
  //Assigning Values
  for (int i = 0; i < MAT_ROWS; ++i) {
      for (int j = 0; j < MAT_COLS; ++j) {
        mat[i][j] = 1;
      }
    }
    //Multiplication
    for(int i =0; i< MAT_COLS; ++i){
      int currCol = col_order[i];
      for(int j=0; j<MAT_ROWS; ++j){
        mat[j][currCol] *= n;
      }
    }
    return mat;
}

void Worker::delete_mat(int** mat){
  for(int i = 0; i<MAT_ROWS; ++i){
    delete [] mat[i];
  }
  delete [] mat;
}
