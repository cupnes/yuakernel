TARGET = kernel.bin
FS_DIR = ../fs
CFLAGS = -Wall -Wextra -nostdinc -nostdlib -fno-builtin -fno-common -Iinclude
LDFLAGS = -Map kernel.map -s -x -T kernel.ld
OBJS = main.o syscall.o fbcon.o fb.o font.o kbc.o x86.o ap.o cmos.o	\
	intr.o serial.o pic.o apic.o sched.o hpet.o acpi.o pci.o	\
	nic.o hda.o handler.o fs.o common.o
ifdef NO_GRAPHIC
	QEMU_ADDITIONAL_ARGS += --nographic
endif
ifdef SMP
	QEMU_ADDITIONAL_ARGS += -smp ${SMP}
endif

$(TARGET): $(OBJS)
	ld $(LDFLAGS) -o $@ $+

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<
%.o: %.s
	gcc $(CFLAGS) -c -o $@ $<

deploy: $(TARGET)
	cp $(TARGET) $(FS_DIR)

run: deploy
	qemu-system-x86_64 -m 4G -enable-kvm \
	-drive if=pflash,format=raw,readonly,file=$(HOME)/OVMF/OVMF_CODE.fd \
	-drive if=pflash,format=raw,file=$(HOME)/OVMF/OVMF_VARS.fd \
	-drive dir=$(FS_DIR),driver=vvfat,rw=on \
	$(QEMU_ADDITIONAL_ARGS)

clean:
	rm -f *~ *.o *.map $(TARGET) include/*~

.PHONY: run clean
