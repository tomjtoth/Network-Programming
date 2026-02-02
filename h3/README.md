# t1

```sh
for prog in t1_*.c; do
    wo_prefix=${prog#t1_}
    gcc -Wall -Wextra -O2 $prog -o ${wo_prefix%.c}
done

# launch in the below order:
./converter
./reader
./doubler

# terminate reader first
```
