#ifndef OS_H
#define OS_H

#include <stdint.h>
#include "stm32f1xx_hal.h"

#define STACK_SIZE 128
#define QUEUE_SIZE 16

typedef enum {
    READY,
    RUNNING,
    SLEEPING,
    BLOCKED
} State;

typedef struct{
    void (*task) (void);
    int priority;
    State state;
    unsigned int wakeup_time;
    char name[20];
    uint32_t *sp;
    uint32_t stack[STACK_SIZE];
} TCB;

typedef struct {
    int value;
    int task;
} BinarySemaphore;

typedef struct {
    int buffer[QUEUE_SIZE];
    int head;
    int tail;
    int count;
} Queue;


int os_init();
int os_task_create(void (*task) (void), int priority, char* name);
void os_sleep(int ticks);
TCB* pick_best_task();
void os_start();
void sem_init(BinarySemaphore* sem);
int sem_wait(BinarySemaphore* sem);
void sem_post(BinarySemaphore* sem);
uint32_t os_now (void);
void os_yield(void);
void queue_init(Queue* q);
int enqueue(Queue* q, int value);
int dequeue(Queue* q, int* value);

TCB* os_get_task_list(void);
int os_get_task_count(void);

#endif