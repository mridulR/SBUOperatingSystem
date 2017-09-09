#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

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
return s;
}
