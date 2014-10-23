
CC ?= arm-eabi-gcc
LD ?= arm-eabi-ld

gos_objs = gos.o \
	gos_main.o \
	gos_vsprintf.o

linkscript = gos.lds
gos_img = gos.elf

.S.o:
	echo "Compiling $< $(CC) -c $<"
	$(CC) -I./ -c $<

.c.o:
	echo "Compiling $< $(CC) -c $<"
	$(CC) -I./ -c $<

all:	$(gos_objs)
	echo "Link all program"
	$(LD) -T $(linkscript) -o $(gos_img) $(gos_objs)

clean:
	echo "Remove gos.elf and *.o"
	rm $(gos_img) *.o

