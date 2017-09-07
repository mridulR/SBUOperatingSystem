#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/defs.h>

char *fgets(char *s, int size, FILE *stream)
{
  int c;
  char *ch = s;

  for (size--; size > 0; size--) {
	if ((c = fgetc(stream)) == (EOF))
		break;
	*ch++ = c;
	if (c == '\n')
		break;
  }

  *ch = 0;
  if (ch == s || c == (EOF)){
	return NULL;
  }
return ch;
}
