#include "os.h"
#include <string.h>

#define MAX_TASKS 5

static TCB task_list[MAX_TASKS];
static int task_index;
static int current_task;
volatile uint32_t system_ticks;

int os_init(){
    task_index = 0;
    current_task = -1;
    return 0;
}

int os_task_create(void (*task) (void), int priority, char* name){
    if(task_index >= MAX_TASKS){
        return -1;
    }
    task_list[task_index].task = task;
    task_list[task_index].priority = priority;
    task_list[task_index].state = READY;
    strcpy(task_list[task_index].name, name);
    task_index++;
    return 0;
}

void os_sleep(int ticks){
    task_list[current_task].state = SLEEPING;
    task_list[current_task].wakeup_time = HAL_GetTick() + ticks;
    return;
}

TCB* pick_best_task(){
    TCB* best_task = NULL;

    for(int i=0; i < task_index; i++){
        if(task_list[i].state == SLEEPING && task_list[i].wakeup_time <= os_now()){
            task_list[i].state = READY;
        }
        if(task_list[i].state == READY){
            if(best_task == NULL || task_list[i].priority > best_task->priority){
                best_task = &task_list[i];
            }
        }
    }
    return best_task;
}

uint32_t os_now(void)
{
    return HAL_GetTick();
}

int os_yield(void)
{
    return 1;
    // task_list[current_task].state = READY;
}

void sem_init(BinarySemaphore* sem)
{
    sem->value = 1;
    sem->task = -1;
}

int sem_wait(BinarySemaphore* sem)
{
    if(sem->value == 1)
    {
        sem->value = 0;
        return 1;
    }

    task_list[current_task].state = BLOCKED;
    sem->task = current_task;

    return 0;
}

void sem_post(BinarySemaphore* sem)
{
    sem->value = 1;
    if(sem->task >= 0){
        task_list[sem->task].state = READY;
        sem->task = -1;
        return;
    }
}

void os_start()
{
    while(1)
    {

        TCB* task = pick_best_task();

        if(task)
        {
            current_task = task - task_list;

            task->state = RUNNING;

            task->task();

            if(task->state == RUNNING)
                task->state = READY;
        }
    }
}

void queue_init(Queue *q)
{
    q->head = 0;
    q->tail = 0;
    q->count = 0;
}

int enqueue(Queue *q, int value){
    if(q->count == QUEUE_SIZE){
        return -1;
    }
    q->buffer[q->tail] = value;
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    q->count++;

    return 0;
}

int dequeue(Queue *q, int *value){
    if(q->count == 0){
        return -1;
    }

    *value = q->buffer[q->head];
    q->head = (q->head + 1) % QUEUE_SIZE;
    q->count--;

    return 0;
}

TCB* os_get_task_list(void)
{
    return task_list;
}

int os_get_task_count(void)
{
    return task_index;
}