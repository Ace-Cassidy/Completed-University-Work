int umenu() {
  uprintf("-----------------------------------------------\n");
  uprintf("getpid getppid ps chname switch kfork exit wait\n");
  uprintf("-----------------------------------------------\n");
}

int getpid() {
  int pid;
  pid = syscall(0, 0, 0, 0);
  return pid;
}

int getppid() { return syscall(1, 0, 0, 0); }

int ugetpid() {
  int pid = getpid();
  uprintf("pid = %d\n", pid);
}

int ugetppid() {
  int ppid = getppid();
  uprintf("ppid = %d\n", ppid);
}

int ups() { return syscall(2, 0, 0, 0); }

int uchname() {
  char s[32];
  uprintf("input a name string : ");
  ugetline(s);
  printf("\n");
  return syscall(3, s, 0, 0);
}

int uswitch() { return syscall(4, 0, 0, 0); }

int ukfork() {
  char s[32] = "";
  while(strcmp(s, "u1") && strcmp(s, "u2")){
    uprintf("input a filename [u1 or u2]: ");
    ugetline(s);
    printf("\n");
  }
  return syscall(5, s, 0, 0);
}

int uexit() {
  char s[32];
  int num;
  uprintf("input exit code: ");
  ugetline(s);
  printf("\n");
  num = atoi(s);
  return syscall(6, s, 0, 0);
}

int uwait() {
  char s[32];
  int num;
  uprintf("input status to wait on: ");
  ugetline(s);
  printf("\n");
  num = atoi(s);
  return syscall(7, s, 0, 0);
}

int ugetc() { return syscall(90, 0, 0, 0); }

int uputc(char c) { return syscall(91, c, 0, 0); }

int getPA() { return syscall(92, 0, 0, 0); }
