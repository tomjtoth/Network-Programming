# 1

After receiving the 1st UDP datagram, the **Server sends back an ICMP error**, because nothing is listening on the port in question.

In case the Client called `connect()` on the socket, it can make calls to `write` and `read`.
The kernel, locally associates the returning ICMP error to the app and delivers it on next invokation (e.g. write operation fails).

In case the Client did not call connect, it does not see failure as the kernel ignores those ICMP erros, I simply got no echoes in my program.

# 2&3 Unix system logging

System logs on my laptop (sysmted-based) are located in `/var/log/journal/<hash>` and are not forwarded to any server:
![40+ megs of logs](./ncdu_var_log_journal_hash.png)

These logs are appended to in a structured manner - retaining timestamps, PIDs and other descriptive information - and can quickly grow to gigabytes in size in case you have a driver issue, or some installed package starts logging verbosely after an update.

I could trim them with one of the following:

```sh
journalctl --vacuum-size=10M
journalctl --vacuum-time=2weeks
```

But I already have an override in place, that keeps them compact sized:

```sh
$ cat /etc/systemd/journald.conf.d/00-journal-size.conf
[Journal]
SystemMaxUse=50M
```

The command `journalctl -t h6t2 -f` follows the output of this [small app](./t2.c) that makes delayed calls to syslog with different priorities. The `-p N` or `-p N..N` args to journalctl filter lines based on these priorities.

From the [man page](https://man7.org/linux/man-pages/man3/syslog.3.html) we can see the whole list of priorities.

# 6 Course feedback
