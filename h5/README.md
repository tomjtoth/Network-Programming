# task 1

I got the below error while trying to compile libfree according to the README:

```sh
tjtoth@np1:~/h5/unpv13e/libfree$ make
gcc -I../lib -g -O2 -D_REENTRANT -Wall   -c -o inet_ntop.o inet_ntop.c
inet_ntop.c: In function ‘inet_ntop’:
inet_ntop.c:60:8: error: argument ‘size’ doesn’t match prototype
   60 | size_t size;
      |        ^~~~
In file included from inet_ntop.c:27:
/usr/include/arpa/inet.h:64:20: error: prototype declaration
   64 | extern const char *inet_ntop (int __af, const void *__restrict __cp,
      |                    ^~~~~~~~~
inet_ntop.c: In function ‘inet_ntop4’:
inet_ntop.c:87:9: warning: old-style function definition [-Wold-style-definition]
   87 |         inet_ntop4(src, dst, size)
      |         ^~~~~~~~~~
inet_ntop.c: In function ‘inet_ntop6’:
inet_ntop.c:112:9: warning: old-style function definition [-Wold-style-definition]
  112 |         inet_ntop6(src, dst, size)
      |         ^~~~~~~~~~
make: *** [<builtin>: inet_ntop.o] Error 1
```

Changing the referenced variable's type to `socklen_t size;` solved the problem.

```sh
tjtoth@np1:~/h5/unpv13e/intro$ ./daytimetcpcli 127.0.0.1
17 FEB 2026 10:56:37 EET
```
