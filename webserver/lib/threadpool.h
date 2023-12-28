#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/select.h>
#include <unistd.h>

#define MAX_THREADS 8
#define MAX_TASKS 256

typedef struct Task {
    void (*taskFunction)(int);
    int arg;
} Task;


void AddTask(Task task);
void PoolInit(pthread_t *threads);
void PoolDestroy(pthread_t *threads);

#endif