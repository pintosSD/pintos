#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "lib/user/syscall.h"
void syscall_init (void);

/* 20121622 10/14*/
#define USER_PROG_ERROR -1
#define VOID_POINTER_SIZE sizeof(void *)
void checkVaddr(const void *vaddr, int argc);

void halt (void);
void exit (int status);
pid_t exec (const char *cmd_line);
int read (int fd, void *buffer, unsigned size);
int write (int fd, const void *buffer, unsigned size);

/* */
#endif /* userprog/syscall.h */
