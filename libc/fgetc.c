#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int fgetc(FILE *fp)
{
	char c;
	if (read (fp->fd, &c, 1) == 0)
		return (EOF);
	return (c);
}
