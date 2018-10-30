TARGET = syscall
CFLAGS = -Wall -Wextra -nostdinc -nostdlib -fno-builtin -fno-common -Iinclude
LDFLAGS = -Map app.map -s -x -T apps.ld
OBJS = app.o

$(TARGET): $(OBJS)
	ld $(LDFLAGS) -o $@ $+

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<
%.o: %.s
	gcc $(CFLAGS) -c -o $@ $<

clean:
	rm -f *~ *.o *.map $(TARGET)

.PHONY: clean
