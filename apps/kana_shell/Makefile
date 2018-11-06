TARGET = ksh
CFLAGS = -Wall -Wextra -nostdinc -nostdlib -fno-builtin -fno-common -Iinclude -fPIE
LDFLAGS = -Map app.map -s -x -T apps.ld -pie
OBJS = app.o lib.o

$(TARGET): $(OBJS)
	ld $(LDFLAGS) -o $@ $+

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<
%.o: %.s
	gcc $(CFLAGS) -c -o $@ $<

deploy: $(TARGET)
	../../tools/create_fs.sh $(TARGET)
	mv fs.img ../../../fs/

clean:
	rm -f *~ *.o *.map $(TARGET) include/*~

.PHONY: clean
