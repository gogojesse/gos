CC ?= arm-eabi-gcc
LD ?= arm-eabi-ld

gos_objs = gos.o \
	gos_main.o \
	gos_vsprintf.o \
	gos_timer.o \
	task.o \
	spinlock.o \
	./libc/gos_syscalls.o

gos_cpu_flags = -mcpu=arm926ej-s -g
#gos_cpu_flags = -mcpu=cortex-a8 -g

NewLibc = ./libc/libc.a
INC_DIR = ./libc/include

linkscript = gos.lds
gos_img = gos.elf

all:	./inc/asm_defines.h $(gos_objs)
	$(LD) -T $(linkscript) -o $(gos_img) $(gos_objs) $(NewLibc) $(ToolChainLib)

./inc/asm_defines.h: asm_defines.c inc/task.h
	$(CC) $(gos_cpu_flags) -S $< -o - | sed 's/#//g' | awk '($$1 == "->") { print "#define " $$2 " " $$3 }' > $@

.S.o:	./inc/asm_defines.h
	$(CC) $(gos_cpu_flags) -I./ -I$(INC_DIR) -c $< -o $*.o

.c.o:
	$(CC) $(gos_cpu_flags) -I./ -I$(INC_DIR) -c $< -o $*.o

clean: 
	rm $(gos_img) $(gos_objs) inc/asm_defines.h

