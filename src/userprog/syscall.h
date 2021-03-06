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
int wait (pid_t pid);
int read (int fd, void *buffer, unsigned size);
int write (int fd, const void *buffer, unsigned size);

/* 11/06 20121622 */
bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int open (const char *file);
int filesize (int fd);
void seek (int fd, unsigned position);
unsigned tell (int fd);
void close (int fd);


/* 10/18 20121622 */
int pibonacci (int n);
int sum (int a, int b, int c, int d);
/* */
#endif /* userprog/syscall.h */
