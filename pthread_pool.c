#include "pthread_pool.h"

pth_pool_t *t_pool;


int pthread_pool_create(pth_pool_t **t_pool ,int max)
{
	(*t_pool) = (pth_pool_t *)malloc(sizeof(pth_pool_t));
	if ((*t_pool) == NULL) 
		return MALLOC_ERR;

	pthread_mutex_init(&((*t_pool)->queue_lock), NULL);  
   	pthread_cond_init(&((*t_pool)->queue_ready), NULL);  
	(*t_pool)->head = NULL;
	(*t_pool) -> max_pth_num = max;
	(*t_pool) -> tid = (pthread_t *)malloc(sizeof(pthread_t) * max); 
	if ((*t_pool)->tid == NULL)
		return MALLOC_ERR;

	int i = 0 ;

	for (;i < max;i++) {
		pthread_create(&((*t_pool)->tid[i]),NULL,thread_routine,NULL);
	}
	
	return 0;
}
int pthread_pool_work_add(pth_work_cb cb,void *arg) 
{
	pth_work_t* work = (pth_work_t *)malloc(sizeof(pth_work_t));
	if (work == NULL)
		return MALLOC_ERR;

	work->cb = cb;
	work->arg = arg;
	work->next = NULL;
	pthread_mutex_lock (&(t_pool->queue_lock));
	work->next = t_pool->head;	
	t_pool->head = work;
	pthread_mutex_unlock(&(t_pool->queue_lock));

	pthread_cond_signal(&(t_pool->queue_ready)); 
	return 0;

}

void *thread_routine(void *arg)
{
	pth_work_t *work  = NULL;
	pthread_detach(pthread_self());
	
	while (1) {
		
		pthread_mutex_lock (&(t_pool->queue_lock));

		while (t_pool->head == NULL) 
			pthread_cond_wait(&(t_pool->queue_ready), &(t_pool->queue_lock));
	/*
		if (t_pool->head == NULL) {
			pthread_mutex_unlock (&(t_pool->queue_lock));
			continue;
		}
	*/	

		work = t_pool->head;
		t_pool->head = t_pool->head->next;

		pthread_mutex_unlock(&(t_pool->queue_lock));
		(work->cb)(work->arg);
		free(work);
		work = NULL;
	}
	
	pthread_exit(NULL);
}

int thread_pool_destroy(pth_pool_t *t_pool)
{
	if (!t_pool )
		return POINT_NULL;
	pthread_mutex_destroy(&(t_pool->queue_lock));  
	pthread_cond_destroy(&(t_pool->queue_ready));  

	pth_work_t* work = t_pool->head;
	while (work) {
		t_pool->head = t_pool->head->next;
		free(work);
		work = t_pool->head;
	}

	free(t_pool);
	free (t_pool->tid);  
	
	t_pool = NULL;
	return 0;
	
}


