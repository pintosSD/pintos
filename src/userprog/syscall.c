#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

/* 10/15 20121622 */
#include <debug.h>
#include "threads/vaddr.h"
/* */

static void syscall_handler (struct intr_frame *);

  void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

  static void
syscall_handler (struct intr_frame *f) 
{
  int systemCallNumber;
  uint32_t *ESP;
  //printf("\nsystem call number : %d\n", *(uint32_t *)f->esp);
  systemCallNumber = *(uint32_t *)f->esp;
  ESP = (uint32_t *)f->esp;
  //hex_dump((int)ESP, ESP, 100, true);
  
  switch(systemCallNumber) {
    case SYS_HALT:
      halt();
      break;
    case SYS_EXIT:
      checkVaddr(f->esp + 4);
      exit((int)*(uint32_t*)(f->esp + 4));
      break;
    case SYS_EXEC:
      break;
    case SYS_WAIT:
      checkVaddr(f->esp + 4);
      wait((int)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_CREATE:
      break;
    case SYS_REMOVE:
      break;
    case SYS_OPEN:
      break;
    case SYS_FILESIZE:
      break;
    case SYS_READ:
      break;
    case SYS_WRITE:
      checkVaddr(f->esp + 4);
      write((int)*(uint32_t *)(f->esp + 4), (const void *)*(uint32_t *)(f->esp + 8), (unsigned)*(uint32_t *)(f->esp + 12));
      break;
    case SYS_SEEK:
      break;
    case SYS_TELL:
      break;
    case SYS_CLOSE:
      break;
  }
  //thread_exit ();
}

void checkVaddr(const void *vaddr) {
  if (!is_user_vaddr(vaddr))  exit(-1) ; 
}

void halt (void) {
  shutdown_power_off();
}
void exit (int status) {
  printf("%s: exit(%d)\n", thread_name(), status);
  thread_current()->exitStatus = status;
  thread_exit ();
}
pid_t exec (const char *cmd_line) {
  
}
int wait (pid_t pid) {
  return process_wait(pid);
}
int read (int fd, void *buffer, unsigned size) {
  int status = USER_PROG_ERROR;

  if (fd != 0) {
    input_getc();
  }

}
int write(int fd, const void *buffer, unsigned size) {
  int status = USER_PROG_ERROR;

  if (fd == 1) {
    putbuf(buffer, size);
    status = size;
  }
  return status;
}
