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

# launch in the below order:
./t4_doubler
./t4_reader
```
