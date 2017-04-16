
#include "Connection.h"


char *getcommand_name(unsigned int command_id)
{
  //all commands must be < MAXCOMMANDNAMESIZE
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

int safereadcommand(int filedes, void *readbuffer)
{
  memset (readbuffer,FALSE,sizeof(CommandMessage));
  int readresult = read (filedes, readbuffer, sizeof(CommandMessage));
  if (readresult < 0)
    errexit ("reading error",NULL);
  return readresult;
}

void safewrite(int filedes, void *writebuffer)
{
  if (write (filedes, writebuffer, sizeof( *writebuffer)) < 0)
    errexit ("error writing message: %s", writebuffer);
}

void safewritecommand(int filedes, void *writebuffer)
{
  if (write (filedes, writebuffer, sizeof(CommandMessage)) < 0)
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

void *saferealloc (void *buffer, unsigned int sz)
{
    void *p;
    if ((p = (void *)realloc (buffer, sz)) == NULL)
    {
        printf ("memory allocation failed, exiting ...\n");
        exit (EXIT_ERRORCODE);
    }
    //memset (p,FALSE,sz);
    return (p);
}
