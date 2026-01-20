# t1c

Making syscalls takes around 10us (or 2..3 in melkki), while printing env vars takes 100...300 us (or 40..100 in melkki).

# t3b

Skipped.

# t4

Skipped.

# t5

From the [man page](https://man7.org/linux/man-pages/man3/exec.3.html):

```c
#include <unistd.h>

extern char **environ;

int execl(const char *path, const char *arg, ...
                /*, (char *) NULL */);
int execlp(const char *file, const char *arg, ...
                /*, (char *) NULL */);
int execle(const char *path, const char *arg, ...
                /*, (char *) NULL, char *const envp[] */);

int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execvpe(const char *file, char *const argv[], char *const envp[]);
```

Flags:

- `l`: args are passed as a _list_, the last arg must be `(char *) NULL` to denote the end of args
- `v`: args are passed as an _array_: `char *const argv[]`, the array must also be terminated with a null pointer
- `e`: defines the _environment_ of the new image, `envp` must be terminated by a null pointer
  - variants without this flag define it using the `environ` variable
- `p`: the command (arg1) is searched for in the _PATH_ environment variable (if it doesn't containt a "/")

# t5b

Skipped.

# t6

- all your processes that are currently running `ps -u $USER` or use `htop` and filter (shortcut _u_) to the username in question

- `ps -C firefox -o pid,ppid,cmd` or use `htop` and filter (shortcut _F4_) to the program name

- ```sh
  # A) kill certain process (nicely)

  kill PID

  # forcefully

  kill -9 PID

  # B) kill all my processes running firefox

  pkill -u "$USER" firefox

  # dry-run the above
  pgrep -u "$USER" firefox

  # C) kill all my processes.
  pkill -u "$USER"
  ```
