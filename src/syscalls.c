#include <sys/stat.h>
#include <errno.h>
#undef errno
extern int errno;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

int _fstat(int file, struct stat *st) {
	st->st_mode = S_IFCHR;
	return 0;
}

#ifndef SEMIHOSTING
int _lseek(int file, int offset, int whence) {
	return 0;
}

int _close(int fd) {
	return -1;
}
#endif

int _read(int file, char *ptr, int len) {
	return 0;
}

int _write(int file, char *ptr, int len) {
#if 0
	int written;

	for (written = 0; written < len; written++) {
		/* TODO: add some real output handling. */
	}
#endif
	return len;
}

/* TODO: replace with something like the alternate implementation provided
 * below. We should probably detect hitting the stack. */
void *_sbrk(int incr) {
	extern char _end;
	static unsigned char *heap = 0;
	unsigned char *prev_heap;

	if (heap == 0) {
		heap = (unsigned char *)&_end;  /* defined by the linker */
	}
	prev_heap = heap;

	heap += incr;

	return prev_heap;
}

#if 0
caddr_t _sbrk(int incr) {
	extern char _end;		/* Defined by the linker */
	static char *heap_end;
	char *prev_heap_end;

	if (heap_end == 0) {
		heap_end = &_end;
	}
	prev_heap_end = heap_end;
	if (heap_end + incr > stack_ptr) {
		write (1, "Heap and stack collision\n", 25);
		abort ();
	}

	heap_end += incr;
	return (caddr_t) prev_heap_end;
}
#endif

void _exit(int status) {
	/* TODO: maybe trigger a system reset or at least go into low power mode? */
	while(1);
}

#pragma GCC diagnostic pop
