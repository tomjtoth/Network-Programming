# 1

After receiving the 1st UDP datagram, the **Server sends back an ICMP error**, because nothing is listening on the port in question.

In case the Client called `connect()` on the socket, it can make calls to `write` and `read`.
The kernel, locally associates the returning ICMP error to the app and delivers it on next invokation (e.g. write operation fails).

In case the Client did not call connect, it does not see failure as the kernel ignores those ICMP erros, I simply got no echoes in my program.
