#include <stdio.h>
#include "sys/stat.h"

#include "serial_hw/serial_hw.h"

#include "inc/spinlock.h"
spinlock_t malloc_lock = spinlock_unlocked;

#define GOS_UART01x_DR(paddr) (*(volatile unsigned int *)(paddr))
#define GOS_UART01x_FR(paddr) (*(((volatile unsigned int *)(paddr))+6))
 
int _close(int file) { return -1; }
 
int _fstat(int file, struct stat *st) {
	st->st_mode = S_IFCHR;
	return 0;
}
 
int _isatty(int file) { return 1; }
 
int _lseek(int file, int ptr, int dir) { return 0; }
 
int _open(const char *name, int flags, int mode) { return -1; }
 
int _read(int file, char *ptr, int len) {
	int todo;
	if(len == 0)
		return 0;
	while(GOS_UART01x_FR(UART01x_ADDR) & UART01x_FR_RXFE);
	*ptr++ = GOS_UART01x_DR(UART01x_ADDR);
	for(todo = 1; todo < len; todo++) {
		if(GOS_UART01x_FR(UART01x_ADDR) & UART01x_FR_RXFE) {
			break;
		}
		*ptr++ = GOS_UART01x_DR(UART01x_ADDR);
	}
	return todo;
}
 
char *heap_end = 0;
caddr_t _sbrk(int incr) {
	extern char heap_low; /* Defined by the linker */
	extern char heap_top; /* Defined by the linker */
	char *prev_heap_end;
 
	if (heap_end == 0) {
		heap_end = &heap_low;
	}
	prev_heap_end = heap_end;
 
	if (heap_end + incr > &heap_top) {
		/* Heap and stack collision */
		return (caddr_t)0;
	}
 
	heap_end += incr;
	return (caddr_t) prev_heap_end;
}
 
int _write(int file, char *ptr, int len) {

	int todo;

	for (todo = 0; todo < len; todo++) {
		GOS_UART01x_DR(UART01x_ADDR) = *ptr++;
	}

	return len;
}

void
__malloc_lock (ptr)
     struct _reent *ptr;
{
	spinlock_lock(&malloc_lock);
}

void
__malloc_unlock (ptr)
     struct _reent *ptr;
{
	spinlock_unlock(&malloc_lock);
}


