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
  //printf("[%d] %x\n",systemCallNumber, ESP);
  //hex_dump((int)ESP, ESP, 100, true);
  if (systemCallNumber == SYS_OPEN || systemCallNumber == SYS_READ || systemCallNumber == SYS_WRITE) {
    lock_acquire(&thread_current()->root->fileSync);
  }
  switch(systemCallNumber) {
    case SYS_HALT:
      halt();
      break;
    case SYS_EXIT:
      checkVaddr(f->esp, 1);
      exit((int)*(uint32_t*)(f->esp + 4));
      break;
    case SYS_EXEC:
      //checkVaddr(f->esp, 1);
      f->eax = exec(pagedir_get_page(thread_current()->pagedir,((int)*(uint32_t*)(f->esp+4))));
      break;
    case SYS_WAIT:
      checkVaddr(f->esp, 1);
      f->eax = wait((pid_t)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_CREATE:
      checkVaddr(f->esp, 2);
      f->eax = create((const char *)*(uint32_t *)(f->esp + 4), (unsigned)*(uint32_t *)(f->esp + 8));
      break;
    case SYS_REMOVE:
      checkVaddr(f->esp, 1);
      f->eax = remove((const char *)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_OPEN:
      checkVaddr(f->esp, 1);
      f->eax = open((const char *)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_FILESIZE:
      checkVaddr(f->esp, 1);
      f->eax = filesize((int)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_READ:
      checkVaddr(f->esp, 3);
      f->eax = read((int)*(uint32_t *)(f->esp + 4), (void *)*(uint32_t *)(f->esp + 8), (unsigned)*(uint32_t *)(f->esp + 12));
      break;
    case SYS_WRITE:
      checkVaddr(f->esp, 3);
      f->eax = write((int)*(uint32_t *)(f->esp + 4), (const void *)*(uint32_t *)(f->esp + 8), (unsigned)*(uint32_t *)(f->esp + 12));
      break;
    case SYS_SEEK:
      checkVaddr(f->esp, 2);
      seek((int)*(uint32_t *)(f->esp + 4), (unsigned)*(uint32_t *)(f->esp + 8));
      break;
    case SYS_TELL:
      checkVaddr(f->esp, 1);
      f->eax = tell((unsigned)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_CLOSE:
      checkVaddr(f->esp, 1);
      close((int)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_PIBONACCI:
      checkVaddr(f->esp, 1);
      f->eax = pibonacci((int)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_SUM:
      checkVaddr(f->esp, 4);
      f->eax = sum((int)*(uint32_t *)(f->esp + 4), (int)*(uint32_t *)(f->esp + 8), (int)*(uint32_t *)(f->esp + 12), (int)*(uint32_t *)(f->esp + 16));
      break;
    default:
      printf("SYSCALL ERROR\n");
  }
  if (systemCallNumber == SYS_OPEN || systemCallNumber == SYS_READ || systemCallNumber == SYS_WRITE) {
    lock_release(&thread_current()->root->fileSync);
  }

}

void checkVaddr(const void *vaddr, int argc) {
  int count = argc;

  while (count--) {
    if (is_kernel_vaddr(vaddr + (argc - count + 1) * VOID_POINTER_SIZE)) {
      exit(-1);
    }
  }
}

void halt (void) {
  shutdown_power_off();
}
void exit (int status) {
  int i;
  struct thread *t = thread_current();

  printf("%s: exit(%d)\n", thread_name(), status);
  t->exitStatus = status;
  for (i = 3; i < 128; ++i) {
    if (t->fd[i] != NULL) close(i);
  }
  if (thread_current()->root->fileSync.holder == thread_current()->root) lock_release(&thread_current()->root->fileSync);
  thread_exit ();
}
pid_t exec (const char *cmd_line) {
  return process_execute(cmd_line); 
}
int wait (pid_t pid) {
  return process_wait(pid);
}
int read (int fd, void *buffer, unsigned size) {
  int i = 0;
  int returnValue;
  if (is_kernel_vaddr(buffer)) exit(-1);

  if (fd == 0) {
    while (*((char *)buffer + i++) = input_getc() && i < size);
    *(char *)buffer = '\0';

    return i;
  }
  else {
    returnValue = file_read(thread_current()->fd[fd], buffer, size);
  }
  return returnValue;
}
int write(int fd, const void *buffer, unsigned size) {
  int status = USER_PROG_ERROR;
  if (fd == 1) {
    putbuf(buffer, size);
    status = size;
  }
  else {
    status = file_write(thread_current()->fd[fd], buffer, size);  
  }
  return status;
}

bool create (const char *file, unsigned initial_size) {
  if (file == NULL) exit(-1);
  return filesys_create(file, initial_size);
}

bool remove (const char *file) {
  return filesys_remove(file);
}

int open (const char *file) {
  if (file == NULL) exit(-1);
  struct file *fp = filesys_open(file);
  struct thread *t = thread_current();
  int i;

  if (fp == NULL) goto fail;
  else {
    for (i = 3; i < 128; ++i) {
      if (t->fd[i] == NULL) {
        t->fd[i] = fp;
        return i;
      }
    }
  }
fail:
  return -1;
}

int filesize (int fd) {
  return file_length(thread_current()->fd[fd]);
}

void seek (int fd, unsigned position) {
  file_seek(thread_current()->fd[fd], position);
}

unsigned tell (int fd) {
  return file_tell(thread_current()->fd[fd]);
}

void close (int fd) {
  struct thread *t = thread_current();

  file_close(t->fd[fd]);
  t->fd[fd] = NULL;
}

int pibonacci (int n) {
  int pNum = 1;
  int ppNum = 0;
  int ans = 0;
  int i = 0;
  if (n == 0) return 0;
  else if (n == 1) return 1;

  for (i = 2; i <= n; ++i) {
    ans = pNum + ppNum;
    ppNum = pNum;
    pNum = ans;
  }

  return ans;
}

int sum (int a, int b, int c, int d) {
  return a + b + c + d;
}
