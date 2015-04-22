CC ?= arm-eabi-gcc
LD ?= arm-eabi-ld

os_objs = startup.o \
	main.o \
	irq.o \
	vic.o \
	rtc.o \
	time.o \
	app.o \
	isr.o \
	idle_task.o \
	timer.o \
	task.o \
	spinlock.o \
	libc/unistd.o \
	libc/syscalls.o

os_cpu_flags = -mcpu=arm926ej-s -g
#os_cpu_flags = -mcpu=cortex-a8 -g

NewLibc = ./libc/libc.a
INC_DIR = ./libc/include

linkscript = startup.lds
os_img = os.elf

all:	./inc/asm_defines.h $(os_objs)
	$(LD) -T $(linkscript) -o $(os_img) $(os_objs) $(NewLibc) $(ToolChainLib)

./inc/asm_defines.h: asm_defines.c inc/task.h
	$(CC) $(os_cpu_flags) -S $< -o - | sed 's/#//g' | awk '($$1 == "->") { print "#define " $$2 " " $$3 }' > $@

.S.o:	./inc/asm_defines.h
	$(CC) $(os_cpu_flags) -I./ -I$(INC_DIR) -c $< -o $*.o

.c.o:
	$(CC) $(os_cpu_flags) -I./ -I$(INC_DIR) -c $< -o $*.o

clean: 
	rm -f $(os_img) $(os_objs) inc/asm_defines.h

