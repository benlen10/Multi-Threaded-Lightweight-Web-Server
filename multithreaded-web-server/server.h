// Author: Benjamin Lenington
// UW Email: lenington@wisc.edu
// Course: CS537 - Intro to Operating Systems
// Project: P4A - Scalable Web Server

// Declare mutex lock var
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// Declare empty and fill conditions
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;

// Function prototypes
void * threadHelper(void * void_arg);
void getargs(int * port_number, int * thread_count, int * buffer_count, int argc, char *argv[]);

// Global vars
int full_count;
int buffer_fill;
int buffer_cur;
int max_buffer_count;
int * current_buffer;

// Pthread wrappers

int Pthread_create(pthread_t * thread, const pthread_attr_t * attr, void * (*start_routine) (void *), void * arg){
  int result_code;
  if ((result_code = pthread_create(thread, attr, start_routine, arg)) != 0){
      unix_error("pthread_create");
  }
  return result_code;
}

int Pthread_mutex_lock(pthread_mutex_t * mutex){
    int result_code = pthread_mutex_lock(mutex);
    if (result_code != 0){
        unix_error("Pthread_mutex_lock error");
    }
    return result_code;
}

int Pthread_mutex_unlock(pthread_mutex_t * mutex) {
   int result_code = pthread_mutex_unlock(mutex);
   if (result_code != 0){
       unix_error("Pthread_mutex_unlock error");
   }
   return result_code;
}

int Pthread_cond_signal(pthread_cond_t * condition){
  int result_code = pthread_cond_signal(condition);
  if (result_code != 0){
      unix_error("Pthread_cond_signal error");
  }
  return result_code;
}

int Pthread_cond_wait(pthread_cond_t * condition, pthread_mutex_t  *mutex){
  int result_code = pthread_cond_wait(condition, mutex);
  if (result_code != 0){
      unix_error("Pthread_cond_wait error");
  }
  return result_code;
}

