# t1

```sh
for prog in t1_*.c; do
    gcc -Wall -Wextra -O2 $prog -o ${prog%.c}
done

# launch in the below order:
./t1_converter
./t1_reader
./t1_doubler
```

# t4

```sh
for prog in t4_*.c; do
    gcc -Wall -Wextra -O2 $prog -o ${prog%.c}
done
```

# t5

```sh
for prog in t5_*.c; do
    # used the -DDEWBUG=1 flag during dev
    gcc -Wall -Wextra -O2 $prog -o ${prog%.c}
done
```

# t6

In both cases server was run with arg `256` and client with args `<SERVER_IP_ADDR> 10485760 5` (10MB in chunks of 5B)

When running both server and client on `melkki` times were:

- server:
  - between 1st and last reads: 2146891, 2175796, 2106449, 2326140, 2164299
- client:
  - establishing conneciton: 146, 149, 104, 99, 187
  - between 1st and last writes: 2147675, 2176391, 2106876, 2326503, 2164530

When running server at `np1` and client at `melkki` times were:

- server:
  - between 1st and last reads: 1088468, 1097351, 1139518
- client:
  - establishing conneciton: 396, 508, 535
  - between 1st and last writes: 1089001, 1097807, 1139724

I found no relevant differences between server and client side measurements when running locally vs in HY hosts.
The server finishes reading marginally sooner in both cases.
Establishing connection took understandably longer between 2 hosts.
On the other hand transferring the same 10MB took less time between the 2 hosts, which I cannot explain.
