# Simple Cooperative RTOS Simulator

A lightweight cooperative Real-Time Operating System (RTOS) simulator written in C for STM32. The project demonstrates a priority-based cooperative scheduler, task management, binary semaphores, message queues, and a UART-based command interface.

---

## Features

- Cooperative priority-based scheduler
- Task Control Block (TCB)
- Task states
  - READY
  - RUNNING
  - SLEEPING
  - BLOCKED
- Binary Semaphore
- Circular Message Queue
- UART command interface
- Heartbeat task using LED and UART

---

## Scheduler

The scheduler selects the highest-priority task that is in the READY state.
Tasks execute cooperatively, meaning each task runs until it:
- returns,
- calls `os_sleep()`, or
- blocks on a semaphore.

---

## Kernel Primitives

### Binary Semaphore

Used for synchronization between tasks.

Functions:
- `sem_init()`
- `sem_wait()`
- `sem_post()`

### Message Queue

Simple circular queue for passing data between tasks.

Functions:
- `queue_init()`
- `enqueue()`
- `dequeue()`

---

## Demo Application

The demo application consists of four tasks:

### Heartbeat Task
- Toggles the onboard LED.
- Sends a heartbeat message over UART.
- Sleeps periodically.

### Producer Task
- Generates incrementing integer values.
- Enqueues data into the message queue.
- Signals the consumer using a binary semaphore.

### Consumer Task
- Waits on the semaphore.
- Dequeues data from the queue.
- Sends the received value over UART.

### UART Command Task
Provides a simple serial command interface.

Supported commands:


`help` : Show available commands 
`status` : Display system tick, queue count, and semaphore state 
`tasks` : List registered tasks 
`state` : Show task states and wake-up times 
`tick` : Display current system tick 
`queue` : Display queue information 
`about` : Display project information 


---

## Design Notes

- Cooperative scheduling
- Priority-based task selection
- Uses `HAL_GetTick()` as the system tick source
- Designed for STM32 microcontrollers

---

## Limitations

- No context switching
- No time slicing


---

## Hardware

Tested using:
- STM32 F103C8T6 (blue pill) microcontroller
- USART1 for serial communication
- Onboard LED for heartbeat indication

## AI Usage

This project was developed with assistance from ChatGPT as an AI assistant.

AI was primarily used for:

- Discussing concepts of OS requirements design decisions.
- Clarify STM32 HAL APIs and project structure.
- Understanding and writing code for individual functionality.
- Suggesting improvements to the cooperative scheduler implementation.
- Understanding building and flashing of STM board.
- Refining the UART command interface.

## Practice Code

As this was my first STM32-based project, the submission includes a separate folder containing practice implementations (UART communication and LED blinking). These were developed to understand the STM32 hardware and validate individual components before integrating them into the final OS implementation.

## Learning & Coding
- I also referred to STM32 HAL documentation and online tutorial videos to understand peripheral initialization, UART communication, GPIO configuration, and the build/flash process.
- Discussing concepts of OS requirements design decisions with AI.
- The implementation was written and integrated manually.
