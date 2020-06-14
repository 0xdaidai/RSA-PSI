CC = gcc
store = MurmurHash2.c bloom.c
res = client server
CFLAGS = -DDBG -Wall -std=gnu99

all:$(res)
$(res):%:%.c
	$(CC) $(CFLAGS) $(store) $< -o $@ -lgmp -lm -lcjson

$(res):init.h rand.h bloom.h phase.h murmurhash2.h file.h
.PHONY : clean

clean :
	rm -f $(res)
