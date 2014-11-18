CC ?= arm-eabi-gcc
LD ?= arm-eabi-ld

gos_objs = gos.o \
	gos_main.o \
	gos_vsprintf.o \
	gos_timer.o \
	task.o \
	./libc/gos_syscalls.o

gos_cpu_flags = -mcpu=arm926ej-s -g
#gos_cpu_flags = -mcpu=cortex-a8 -g

NewLibc = ./libc/libc.a
INC_DIR = ./libc/include

linkscript = gos.lds
gos_img = gos.elf

.S.o:
	$(CC) $(gos_cpu_flags) -I./ -I$(INC_DIR) -c $< -o $*.o

.c.o:
	$(CC) $(gos_cpu_flags) -I./ -I$(INC_DIR) -c $< -o $*.o

all:	$(gos_objs)
	$(LD) -T $(linkscript) -o $(gos_img) $(gos_objs) $(NewLibc) $(ToolChainLib)

clean: 
	rm $(gos_img) $(gos_objs)

