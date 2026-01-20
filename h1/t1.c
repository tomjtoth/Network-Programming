#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void)
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

   return 0;
}