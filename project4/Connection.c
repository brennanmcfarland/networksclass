/*
  Brennan McFarland
  bfm21
  Connection.c
  4/28/17
  handles how data is read and written from files and sent across the network
  for client and server
    -sends/receives text/commands
    -performs file operations
    -memory allocation
*/

#include "Connection.h"
#include "McFarlandNetworks.h"

char *filelist; //string holding the list of readable files
char filecontents[MAXFILEREADSIZE]; //string holding the contents of a file


char *getcommand_name(unsigned int command_id)
{
  //all commands must be < MAXCOMMANDNAMESIZE
  switch(command_id)
  {
    case CMDID_TEST:
      return "TEST";
      break;
    case CMDID_GENERATECLIENTID:
      return "GENERATE-CLIENT-ID";
      break;
    case CMDID_LISTFILES:
      return "LIST-FILES";
      break;
    case CMDID_READFILE:
      return "READ-FILE";
      break;
    case CMDID_WRITEFILE:
      return "WRITE-FILE";
      break;
    case CMDID_READSESSIONLOG:
      return "READ-SESSION-LOG";
      break;
    case CMDID_GETTIME:
      return "GET-TIME";
      break;
    case CMDID_QUIT:
      return "QUIT";
      break;
    default:
      return "ERR_INVALID_COMMAND";
  }
}

//wait for the other end to respond with a message
void awaitresponse(int filedes, void *readbuffer)
{
  while(TRUE == TRUE)
  {
    if(saferead(filedes, readbuffer) != EMPTYSTRING)
      return;
  }
}

//await for the other end to respond with text
void awaittext(int filedes, void *readbuffer)
{
  while(TRUE==TRUE)
  {
    if(safereadtext(filedes, readbuffer) != EMPTYSTRING)
      return;
  }
}

//send the contents of one file
void sendfile(char *filename, char *buffer, char *filecontents,
  unsigned int clientid, char *directory, unsigned int callingcommand)
{
  FILE *sendingfilestream;
  //get file path
  char filepath[strlen(filename)+strlen(directory)];
  filepath[INTINITIALIZER] = '\0';
  strcat((char *)filepath, directory);
  strcat((char *)filepath, filename);
  filepath[strlen(filepath)-OFFSETVALUE] = '\0';

  //read from the file and send it across the network
  safefileopen(&sendingfilestream, (char *)filepath, 'r');
  safefileread(sendingfilestream, (void *)filecontents, MAXFILEREADSIZE);
  if(strcmp(filecontents, "") == EMPTYSTRING) //waits forever otherwise
    strcpy(filecontents, "[empty file]\n");
  printf("S -> C: "); //only the server sends raw unstructured text in a stream
  printf("%s %s\n", getcommand_name(callingcommand), filecontents);
  sendtext(filecontents, buffer, clientid);
  safefileclose(&sendingfilestream);
  memset(filecontents, INTINITIALIZER, strlen(filecontents));
  memset(buffer, INTINITIALIZER, strlen(buffer));
}

int saferead(int filedes, void *readbuffer)
{
  memset (readbuffer,INTINITIALIZER,BUFLEN);
  int readresult = read (filedes, readbuffer, BUFLEN - OFFSETVALUE);
  if (readresult < EXIT_NOERROR)
    errexit ("reading error",NULL);
  return readresult;
}

int safereadtext(int filedes, void *readbuffer)
{
  memset(readbuffer,INTINITIALIZER,MAXFILEREADSIZE);
  int readresult = read (filedes, readbuffer, MAXFILEREADSIZE);
  if (readresult < EXIT_NOERROR)
    errexit ("error reading message: %s", readbuffer);
  return readresult;
}

int safereadcommand(int filedes, void *readbuffer)
{
  memset (readbuffer,INTINITIALIZER,sizeof(CommandMessage));
  int readresult = read (filedes, readbuffer, sizeof(CommandMessage));
  if (readresult < EXIT_NOERROR)
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
  if ((writeresult = write (filedes, writebuffer, sizeof( *writebuffer)))
    < EXIT_NOERROR)
    errexit ("error writing message: %s", writebuffer);
}

void safewritetext(int filedes, char *writebuffer)
{
  int lentowrite = strlen(writebuffer);
  int writeresult;
  if ((writeresult = write (filedes, writebuffer, lentowrite)) < EXIT_NOERROR)
    errexit ("Error writing message: %s", writebuffer);
}

void safewritecommand(int filedes, void *writebuffer)
{
  if (write (filedes, writebuffer, sizeof(CommandMessage)) < EXIT_NOERROR)
    errexit ("Error writing message: %s", writebuffer);
}

int safefileread(FILE *filestream, void *readbuffer, int readbuffersize)
{
  memset(readbuffer, INTINITIALIZER, readbuffersize);
  int readresult = fread(readbuffer,BYTESIZE,readbuffersize,filestream);
  if(readresult == EMPTYSTRING && ferror(filestream) != EXIT_NOERROR)
  {
    printf("Error reading from file.\n");
    exit(EXIT_ERRORCODE);
  }
  return readresult;
}

int safefilewrite(FILE *filestream, void *writebuffer, int writebuffersize)
{
  int writeresult = fwrite(writebuffer, BYTESIZE, writebuffersize, filestream);
  if(writeresult == EMPTYSTRING && ferror(filestream) != EXIT_NOERROR)
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
  if(fclose(*filestream) != EXIT_NOERROR)
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
    memset (p,INTINITIALIZER,sz);
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
  char *destb = (char *)safemalloc(destsize+strlen(*src)+OFFSETVALUE);
  if(strlen(*dest) == EMPTYSTRING)
    strcpy(destb, *src);
  else
    destb = (char *)strcat(*dest, *src);
  destb = (char *)strcat(destb, "\n");
  *dest = (char *)safemalloc(strlen(destb));
  strcpy(*dest, destb);
  free(destb);
  return *dest;
}
