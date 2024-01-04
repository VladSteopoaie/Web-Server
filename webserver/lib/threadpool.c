#include "threadpool.h"

Task taskQueue[MAX_TASKS];
int taskCount = 0;

pthread_mutex_t mutexQueue; // used when adding a task to the task queue
pthread_mutex_t mutexRequest; // used when accessing and mapping the memory of a file in ManageRequest function
pthread_cond_t condQueue; // used to wait for a task to be added to the queue

void ExecuteTask(Task* task)
{
    task->taskFunction(task->arg);
}

// thread routine
void* startThread(void* args)
{
    for(;;)
    {
        Task task;
        pthread_mutex_lock(&mutexQueue); // lock mutex when modifying taskQueue

        while(taskCount == 0) // wait for a task if the queue is empty
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
    pthread_mutex_lock(&mutexQueue); // lock mutex when modifying taskQueue
    taskQueue[taskCount] = task;
    taskCount ++;
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue); // signal that there is a task available
}

void PoolInit(pthread_t *threads)
{
    int size = atoi(MAX_THREADS);
    assert(size > 0);
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_mutex_init(&mutexRequest, NULL);
    pthread_cond_init(&condQueue, NULL);
    for(int i = 0; i < size; i ++)
    {
        int result;
        if ((result = pthread_create(&threads[i], NULL, &startThread, NULL)) < 0)
        {
            perror("thread");
        }

        assert(result >= 0);
    }
}

void PoolDestroy(pthread_t *threads)
{
    for(int i = 0; i < atoi(MAX_THREADS); i ++)
    {
        int result;
        if ((result = pthread_join(threads[i], NULL)) != 0)
        {
            perror("join");
        }

        assert(result >= 0);
    }

    pthread_mutex_destroy(&mutexQueue);
    pthread_mutex_destroy(&mutexRequest);
    pthread_cond_destroy(&condQueue);
}


void LockRequestMutex()
{
    pthread_mutex_lock(&mutexRequest);
}

void UnlockRequestMutex()
{
    pthread_mutex_unlock(&mutexRequest);
}