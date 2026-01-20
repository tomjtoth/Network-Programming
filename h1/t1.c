#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

long elapsed_time(struct timeval start, struct timeval end)
{
   return (end.tv_sec - start.tv_sec) * 1000000L +
          (end.tv_usec - start.tv_usec);
}

int main(int argc, char *argv[], char *envp[])
{

   // silence warnings
   (void)argc;
   (void)argv;

   // declare vars of same type
   pid_t pid, ppid, pgid;
   long diff_sys, diff_env;
   struct timeval t1, t2;

   gettimeofday(&t1, NULL);

   // get IDs
   pid = getpid();
   ppid = getppid();
   pgid = getpgrp();
   uid_t uid = getuid();

   gettimeofday(&t2, NULL);
   diff_sys = elapsed_time(t1, t2);

   printf("Process info:\n");
   printf("  PID  = %d\n", pid);
   printf("  PPID = %d\n", ppid);
   printf("  PGID = %d\n", pgid);
   printf("  UID  = %d\n\n", uid);

   gettimeofday(&t1, NULL);

   printf("Environment variables:\n");
   // printing each elemnt of th eNULL terminated envp array
   for (char **env = envp; *env != NULL; env++)
   {
      printf("%s\n", *env);
   }

   gettimeofday(&t2, NULL);
   diff_env = elapsed_time(t1, t2);

   printf("\nTimes [microseconds] it took to:\n");
   printf("   make syscalls: %ld\n", diff_sys);
   printf("  print env vars: %ld\n", diff_env);

   return 0;
}
