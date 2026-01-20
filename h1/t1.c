#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[], char *envp[])
{

   pid_t pid, ppid, pgid;
   uid_t uid;

   pid = getpid();
   ppid = getppid();
   pgid = getpgid(0);
   uid = getuid();

   printf("Process info:\n");
   printf("  PID  = %d\n", pid);
   printf("  PPID = %d\n", ppid);
   printf("  PGID = %d\n", pgid);
   printf("  UID  = %d\n\n", uid);

   printf("Environment variables:\n");
   for (char **env = envp; *env != NULL; env++)
   {
      printf("%s\n", *env);
   }

   return 0;
}