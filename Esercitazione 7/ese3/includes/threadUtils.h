void lock(pthread_mutex_t *mtx);
void unlock(pthread_mutex_t *mtx);
void wait(pthread_cond_t *cond, pthread_mutex_t *mtx);
void signal(pthread_cond_t *cond);
