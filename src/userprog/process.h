#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

#endif /* userprog/process.h */

// 20121622
#define CHAR_POINTER_SIZE sizeof(char *)
#define CHAR_DOUBLE_POINTER_SIZE sizeof(char **)
#define INT_SIZE sizeof(int)
#define VOID_POINTER_SIZE sizeof(void *)
