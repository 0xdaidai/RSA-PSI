CC = gcc
store = MurmurHash2.c bloom.c
res = client server
CFLAGS = -DDBG -Wall

all:$(res)
$(res):%:%.c
	$(CC) $(CFLAGS) $(store) $< -o $@ -lgmp -lm -lcjson

.PHONY : clean

clean :
	rm -f $(res)
