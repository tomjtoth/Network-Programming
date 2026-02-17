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

# task 2

I made use of the Makefile and renamed the appropriate files to `daytimetcp{srv,cli}.c` and ran `make` on the server.

## part A

- changing the PORT is necessary because the original port is already in use by the original service and only 1 app can bind to 1 port
- also processes binding ports below 1024 would require to be run as root, and I'm not in sudoers...

  ```sh
  tjtoth@np1:~$ sudo cat
  [sudo] password for tjtoth:
  tjtoth is not in the sudoers file.
  This incident has been reported to the administrator.
  ```

## part B

I get the below error when starting up the server.

```sh
tjtoth@np1:~/h5/unpv13e/intro$ ./daytimetcpsrv1
accept error: Invalid argument
```

The call to `listen()` transitions the port from CLOSED to LISTEN state on the server.
`accept()` requires a port that's actually in LISTEN state.

Regardless when the client calls `connect()`, it would receive an `RST` packet from the server, as the port is CLOSEd.

## part C

When the call to `bind` is skipped, but `listen` is called, we get an implicit bind to a random port, so the server is up and running, only not on `PORT=50000`, but at some random port the kernel assigned.

The client receives `RST` as nothing is actually listening on `PORT=50000`.

```sh
tjtoth@np1:~/h5/unpv13e/intro$ ./daytimetcpcli 127.0.0.1
connect error: Connection refused
```

## part D

Redireceted the output of the server [to a file](./2d_server.log), and launched 3 instances of the client in parallel, each with 20,000 iterations via the below command:

```sh
for x in 1 1 1; do ./daytimetcpcli 127.0.0.1 20000; done
```

The logged port numbers lap over 32770 and 60990 ranges several times

```sh
36920 -> 60986
32770 -> 60986
32770 -> 60996
32778 .....
```

# task 5

No changes were necessary to get the test app up and running.

```sh
tjtoth@np1:~/h5/unpv22e/lib$     cd ../pipe     # build and test a simple program
tjtoth@np1:~/h5/unpv22e/pipe$     make pipeconf
gcc -g -O2 -D_REENTRANT -Wall   -c -o pipeconf.o pipeconf.c
gcc -g -O2 -D_REENTRANT -Wall -o pipeconf pipeconf.o ../libunpipc.a -lrt -lpthread
/usr/bin/ld: ../libunpipc.a(wrapunix.o): in function `Mktemp':
/home/tjtoth/h5/unpv22e/lib/wrapunix.c:184:(.text+0x409): warning: the use of `mktemp' is dangerous, better use `mkstemp' or `mkdtemp'
tjtoth@np1:~/h5/unpv22e/pipe$     ./pipeconf /tmp
PIPE_BUF = 4096, OPEN_MAX = 1024
```

# task 6

The theoritical maximum for the UDP datagram is _65,535 bytes_,
which includes the 20 bytes long IP header,
and the 8 byte long UDP header,
reducing the useful maximum payload to _65,507 bytes_.

I took the [server](../h4/t1_server.c) from last week, trimmed it.
Modified the [client](../h4/t1_client.c) to send incrementally larger sized data.
Set both side's `BUFSIZE=65535` and added logging of sent/received size.

```sh
$ ./cli
sent 0 bytes
sent 1 bytes

...

sent 65507 bytes
sendto failed: Message too long
```

The last successfully sent chunk was the same size as calculated above.
