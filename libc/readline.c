#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0
#define MAX_BUF_SIZE 4

/** @ Implementation of readline utility function for reading input line by line
      As we don't know the actual length of the line. We do it in chunks of 256 bytes.
      if read bytes equals 256 bytes then possible there is more data to be read. So, we
      continue reading until the read bytes is less than 256 * readCount.
*/
char *readline(const char *prompt)
{
  char *data = malloc(MAX_BUF_SIZE);
  char * currentBufPtr = data;

  int readCount = 1;
  int readBytes = 0;
  int totalReadBytes = 0;
  int remainingSpaceInBuf = MAX_BUF_SIZE;

 do 
 {
       readBytes = read(0, currentBufPtr, remainingSpaceInBuf);
       ++readCount;

       // if the readBytes is less than the buffer or the last read character is '\n'
       // then there is nothing more to read. So, we exit the loop.
       if(readBytes == -1 || readBytes < MAX_BUF_SIZE)
       {
         break;
       }

       totalReadBytes += readBytes;
       remainingSpaceInBuf -= readBytes;

       if(data[totalReadBytes-1] == '\n')
       {
         break;
       }

       // if read bytes is ame as buffer size then
       // read again. Probably, there is more data 
       // to be read.
       // TO DO: Implement realloc() and plugin here.
       if(remainingSpaceInBuf == 0)
       {
           char* newData = malloc(MAX_BUF_SIZE * readCount);
           memcpy(newData, data, totalReadBytes);
           free(data);
           data = newData;
           currentBufPtr = data + totalReadBytes;
           remainingSpaceInBuf = (MAX_BUF_SIZE * readCount) - totalReadBytes;
       }
   }while(readBytes != -1 || data[totalReadBytes -1 ] != '\n');

  return data;
}
