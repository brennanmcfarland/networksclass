
#include "Connection.h"

char *filelist; //string holding the list of readable files
char filecontents[MAXFILEREADSIZE]; //string holding the contents of a file


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
    case 34:
      return "LIST-FILES";
      break;
    case 36:
      return "READ-FILE";
      break;
    case 38:
      return "WRITE-FILE";
      break;
    case 40:
      return "READ-SESSION-LOG";
      break;
    case 42:
      return "GET-TIME";
      break;
    case 99:
      return "QUIT";
      break;
    default:
      return "ERR_INVALID_COMMAND";
  }
}

//wait for the other end to respond with a message
void awaitresponse(int filedes, void *readbuffer)
{
  while(1 == 1)
  {
    if(saferead(filedes, readbuffer) != 0)
      return;
  }
}

//await for the other end to respond with text
void awaittext(int filedes, void *readbuffer)
{
  while(1==1)
  {
    if(safereadtext(filedes, readbuffer) != 0)
      return;
  }
}

//send the contents of one file
void sendfile(char *filename, char *buffer, char *filecontents,
  unsigned int clientid, char *directory)
{
  FILE *sendingfilestream;
  //get file path
  char filepath[strlen(filename)+strlen(directory)];
  filepath[0] = '\0';
  strcat((char *)filepath, directory);
  strcat((char *)filepath, filename);
  filepath[strlen(filepath)-1] = '\0';

  //read from the file and send it across the network
  safefileopen(&sendingfilestream, (char *)filepath, 'r');
  safefileread(sendingfilestream, (void *)filecontents, MAXFILEREADSIZE);
  if(strcmp(filecontents, "") == 0) //send stub if empty, waits forever otherwise
    strcpy(filecontents, "[empty file]\n");
  sendtext(filecontents, buffer, clientid);
  safefileclose(&sendingfilestream);
  memset(filecontents, FALSE, strlen(filecontents));
  memset(buffer, FALSE, strlen(buffer));
}

int saferead(int filedes, void *readbuffer)
{
  memset (readbuffer,FALSE,BUFLEN);
  int readresult = read (filedes, readbuffer, BUFLEN - 1);
  if (readresult < 0)
    errexit ("reading error",NULL);
  return readresult;
}

int safereadtext(int filedes, void *readbuffer)
{
  memset(readbuffer,FALSE,MAXFILEREADSIZE);
  int readresult = read (filedes, readbuffer, MAXFILEREADSIZE);
  if (readresult < 0)
    errexit ("error reading message: %s", readbuffer);
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

char *receivetext(char *texttoreceive, char *textbuffer, int source_id)
{
  textbuffer = (char *)safemalloc(MAXFILEREADSIZE);
  awaittext(source_id, (void *)textbuffer);
  return (char *)textbuffer;
}

void safewrite(int filedes, void *writebuffer)
{
  int writeresult;
  if ((writeresult = write (filedes, writebuffer, sizeof( *writebuffer))) < 0)
    errexit ("error writing message: %s", writebuffer);
}

void safewritetext(int filedes, char *writebuffer)
{
  int lentowrite = strlen(writebuffer);
  int writeresult;
  if ((writeresult = write (filedes, writebuffer, lentowrite)) < 0)
    errexit ("error writing message: %s", writebuffer);
}

void safewritecommand(int filedes, void *writebuffer)
{
  if (write (filedes, writebuffer, sizeof(CommandMessage)) < 0)
    errexit ("error writing message: %s", writebuffer);
}

int safefileread(FILE *filestream, void *readbuffer, int readbuffersize)
{
  int readresult = fread(readbuffer,BYTESIZE,readbuffersize,filestream);
  if(readresult == FALSE && ferror(filestream) != FALSE)
  {
    printf("Error reading from file.\n");
    exit(EXIT_ERRORCODE);
  }
  return readresult;
}

int safefilewrite(FILE *filestream, void *writebuffer, int writebuffersize)
{
  int writeresult = fwrite(writebuffer, 1, writebuffersize, filestream);
  if(writeresult == FALSE && ferror(filestream) != FALSE)
  {
    printf("Error writing to file.\n");
    exit(EXIT_ERRORCODE);
  }
  return writeresult;
}

void safefileopen(FILE **filestream, char *filename, char filemode)
{
  char *filemodeptr = &filemode;
  if((*filestream = fopen(filename,filemodeptr)) == NULL)
  {
    printf("Error: Unable to open file %s.\n", filename);
    exit(EXIT_ERRORCODE);
  }
}

void safefileclose(FILE **filestream)
{
  if(fclose(*filestream) != 0)
  {
    printf("Error: Unable to close file.\n");
    exit(EXIT_ERRORCODE);
  }
}

void sendtext(char *texttosend, char *textbuffer, unsigned int target_id)
{
  safewritetext(target_id, texttosend);
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
    return (p);
}

char *safestrcpy(char **dest, char **src)
{
  int destsize = strlen(*dest);
  char *dest2 = (char *)safemalloc(destsize+strlen(*src)+1);
  if(strlen(*dest) == 0)
    strcpy(dest2, *src);
  else
    dest2 = (char *)strcat(*dest, *src);
  dest2 = (char *)strcat(dest2, "\n");
  *dest = (char *)safemalloc(strlen(dest2));
  strcpy(*dest, dest2);
  free(dest2);
  return *dest;
}
