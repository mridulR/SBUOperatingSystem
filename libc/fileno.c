#include <sys/types.h>

int fileno (FILE *fp)
{
  return (fp->fd);
}
