#include "threadpool.h"

Task taskQueue[MAX_TASKS];
int taskCount = 0;
pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;


void ExecuteTask(Task* task)
{
    task->taskFunction(task->arg);
}


void* startThread(void* args)
{
    for(;;)
    {
        Task task;
        pthread_mutex_lock(&mutexQueue);
        while(taskCount == 0)
        {
            pthread_cond_wait(&condQueue, &mutexQueue);
        }

        task = taskQueue[0];
        for (int i = 0; i < taskCount - 1; i++)
        {
            taskQueue[i] = taskQueue[i + 1];
        }
        taskCount --;

        pthread_mutex_unlock(&mutexQueue);
        ExecuteTask(&task);
    }
}

void AddTask(Task task)
{
    pthread_mutex_lock(&mutexQueue);
    taskQueue[taskCount] = task;
    taskCount ++;
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue);
}

void PoolInit(pthread_t *threads)
{
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&condQueue, NULL);
    for(int i = 0; i < MAX_THREADS; i ++)
    {
        if (pthread_create(&threads[i], NULL, &startThread, NULL) < 0)
        {
            perror("thread");
            exit(EXIT_FAILURE);
        }
    }
}

void PoolDestroy(pthread_t *threads)
{
    for(int i = 0; i < MAX_THREADS; i ++)
    {
        if (pthread_join(threads[i], NULL) != 0)
        {
            perror("join");
            exit(EXIT_FAILURE);
        }
    }

    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);
}
