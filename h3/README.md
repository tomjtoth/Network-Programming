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
