#ifndef __THREAD_POOL_H_
#define __THREAD_POOL_H_
#include <pthread.h>
#include <stdlib.h>

typedef struct pthread_work_type pth_work_t;
typedef void *(* pth_work_cb)(void *);
typedef struct pthread_pool pth_pool_t;

struct pthread_work_type {
	void *arg;
	pth_work_cb cb;
	pth_work_t *next;
};
struct pthread_pool {
	int max_pth_num;
	int queue_work_num;
	pthread_t *tid;
	pth_work_t *head;
	pthread_mutex_t queue_lock;                    
	pthread_cond_t  queue_ready; 
	
};

enum {
	MALLOC_ERR = -1,
	CREATE_PTH = -2,
	POINT_NULL = -3,
};
int pthread_pool_create(pth_pool_t **t_pool ,int max);
int pthread_pool_work_add(pth_work_cb cb,void *arg) ;
void *thread_routine(void *arg);
#endif
