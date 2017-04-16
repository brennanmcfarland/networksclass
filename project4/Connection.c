
#include "Connection.h"


char *getcommand_name(unsigned int command_id)
{
  switch(command_id)
  {
    case 1:
      return "TEST";
      break;
    case 32:
      return "GENERATE-CLIENT-ID";
      break;
    default:
      return "ERR_INVALID_COMMAND";
  }
}

int saferead(int filedes, void *readbuffer)
{
  memset (readbuffer,FALSE,BUFLEN);
  int readresult = read (filedes, readbuffer, BUFLEN - 1);
  if (readresult < 0)
    errexit ("reading error",NULL);
  return readresult;
}

void safewrite(int filedes, void *writebuffer, int size)
{
  if (write (filedes, writebuffer, size) < 0)
    errexit ("error writing message: %s", writebuffer);
}

void *safemalloc (unsigned int sz)
{
    void *p;
    if ((p = (void *)malloc (sz)) == NULL)
    {
        printf ("memory allocation failed, exiting ...\n");
        exit (EXIT_ERRORCODE);
    }
    memset (p,FALSE,sz);
    return (p);
}
