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

   (void)argc;
   (void)argv;

   pid_t pid, ppid, pgid;
   uid_t uid;
   struct timeval t1, t2;

   gettimeofday(&t1, NULL);

   pid = getpid();
   ppid = getppid();
   pgid = getpgrp();
   uid = getuid();

   gettimeofday(&t2, NULL);
   long syscall_time = elapsed_time(t1, t2);

   printf("Process info:\n");
   printf("  PID  = %d\n", pid);
   printf("  PPID = %d\n", ppid);
   printf("  PGID = %d\n", pgid);
   printf("  UID  = %d\n\n", uid);

   gettimeofday(&t1, NULL);

   printf("Environment variables:\n");
   for (char **env = envp; *env != NULL; env++)
   {
      printf("%s\n", *env);
   }

   gettimeofday(&t2, NULL);
   long env_print_time = elapsed_time(t1, t2);

   printf("\nTimes [microseconds] it took to:\n");
   printf("   make syscalls: %ld\n", syscall_time);
   printf("  print env vars: %ld\n", env_print_time);

   return 0;
}
