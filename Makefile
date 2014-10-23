
CC ?= arm-eabi-gcc
LD ?= arm-eabi-ld

gos_objs = gos.o \
	gos_main.o \
	gos_vsprintf.o

#gos_cpu_flags = -march=armv6
gos_cpu_flags = -march=armv5te

linkscript = gos.lds
gos_img = gos.elf

.S.o:
	echo "Compiling $< $(CC) $(gos_cpu_flags) -I./ -c $<"
	$(CC) $(gos_cpu_flags) -I./ -c $<

.c.o:
	echo "Compiling $< $(CC) $(gos_cpu_flags) -I./ -c $<"
	$(CC) $(gos_cpu_flags) -I./ -c $<

all:	$(gos_objs)
	echo "Link all program"
	$(LD) -T $(linkscript) -o $(gos_img) $(gos_objs)

clean:
	echo "Remove gos.elf and *.o"
	rm $(gos_img) *.o

