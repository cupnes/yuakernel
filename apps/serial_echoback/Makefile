TARGET = e.serial_echoback
CFLAGS = -Wall -Wextra -Iinclude -fPIE
ifeq ($(RUN_LOCAL),y)
CFLAGS += -DRUN_LOCAL -g
else
CFLAGS += -nostdinc -nostdlib -fno-builtin -fno-common
endif
LDFLAGS = -Map app.map -pie -s -x -T apps.ld
OBJS = main.o lib.o

$(TARGET): $(OBJS)
ifeq ($(RUN_LOCAL),y)
	gcc -g -o $@ $+
else
	ld $(LDFLAGS) -o $@ $+
endif

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
