#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>

#define MAX_THREADS 8
#define MAX_TASKS 256


typedef struct Task {
    void (*taskFunction)(int);
    int arg;
} Task;

/**
    @brief Creates the threads and initializes all mutexes and condition variables.

    @param threads The array of available threads.

*/
void PoolInit(pthread_t *threads);

/**
    @brief Adds a task to the task queue.

    @param task The task to be added to the task queue.
*/
void AddTask(Task task);

/**
    @brief Joins all threads and destroies all mutexes and condition variables.

    @param threads The array of threads to be destoryed
*/

void PoolDestroy(pthread_t *threads);

/**
    @brief Locks mutexRequest
*/
void LockRequestMutex();

/**
    @brief Unlocks mutexRequest
*/
void UnlockRequestMutex();

#endif