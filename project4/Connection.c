
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

//seems like it never receives it
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
  char filepath[strlen(filename)+strlen(directory)];
  filepath[0] = '\0';
  strcat((char *)filepath, directory);
  strcat((char *)filepath, filename);
  filepath[strlen(filepath)-1] = '\0';

  //check if the file exists and the user can open it
  //if(access((char *)filepath+1, F_OK) == -1)
  //{
  //  strcpy(filecontents, "Error: The file either does not exist or cannot be accessed");
  //  sendtext(filecontents, buffer, clientid);
  //}
  //else
  //{
    safefileopen(&sendingfilestream, (char *)filepath, 'r');
    safefileread(sendingfilestream, (void *)filecontents, MAXFILEREADSIZE);
    sendtext(filecontents, buffer, clientid);
    safefileclose(&sendingfilestream);
  //}
  memset(filecontents, FALSE, strlen(filecontents));
  memset(buffer, FALSE, strlen(buffer));
}

int saferead(int filedes, void *readbuffer)
{
  memset (readbuffer,FALSE,BUFLEN);
  int readresult = read (filedes, readbuffer, BUFLEN - 1);
  //printf("read %d bytes\n", readresult);
  if (readresult < 0)
    errexit ("reading error",NULL);
  return readresult;
}

int safereadtext(int filedes, void *readbuffer)
{
  memset(readbuffer,FALSE,MAXFILEREADSIZE);
  int temp = read (filedes, readbuffer, MAXFILEREADSIZE);
  if (temp < 0)
    errexit ("error reading message: %s", readbuffer);
  printf("read %d bytes of text\n", temp);
  return temp;
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
  //TODO: this may overflow with too big input
  textbuffer = (char *)safemalloc(MAXFILEREADSIZE);
  awaittext(source_id, (void *)textbuffer);
  //ERR: HERE IT"S BLANK
  //printf("received the following:%s\n", (char *)textbuffer);
  return (char *)textbuffer;
}

/*
//receive text from the network, receives chunk by chunk into textbuffer until end
void receivetext(char **texttoreceive, char **textbuffer, int source_id)
{
  //wait for more text and read it in until we get an eof
  do
  {
    int tempblock[BUFLEN];
    awaitresponse(source_id, (void *)&tempblock);
    *textbuffer = (char *)tempblock;
    printf("buffer contains the following:%s\n", *textbuffer);

    //int textbuffersize = strlen(*textbuffer);
    //char *textbuffer2 = (char *)safemalloc(textbuffersize+strlen(*texttoreceive)+1);
    //if(strlen(*textbuffer) == 0)
    //  strcpy(textbuffer2, *texttoreceive);
    //else
    //  textbuffer2 = (char *)strcat(*textbuffer, *texttoreceive);
    //textbuffer2 = (char *)strcat(textbuffer2, "\n");
    // *textbuffer = (char *)safemalloc(strlen(textbuffer2));
    //strcpy(*textbuffer, textbuffer2);

    //strcpy(&textbuffer, &texttoreceive);
    safestrcpy(texttoreceive, textbuffer);
    printf("received the following:%s\n", *texttoreceive);
  }while((*textbuffer)[strlen(*textbuffer)-1] != EOF);
}
*/

void safewrite(int filedes, void *writebuffer)
{
  int temp;
  if ((temp = write (filedes, writebuffer, sizeof( *writebuffer))) < 0)
    errexit ("error writing message: %s", writebuffer);
  //printf("wrote %d bytes\n", temp);
}

void safewritetext(int filedes, char *writebuffer)
{
  int lentowrite = strlen(writebuffer);
  int temp;
  if ((temp = write (filedes, writebuffer, lentowrite)) < 0)
    errexit ("error writing message: %s", writebuffer);
  //ERR: HERE WRITEBUFFER HAS THE FILE CONTENTS
  printf("wrote %d bytes of text\n", temp);
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
  //textbuffer = (char **)safemalloc(strlen(*texttosend));

  char tempbuffer[(int)strlen(texttosend)];
  //tempbuffer = *(char **)safemalloc(sizeof(char[strlen(*texttosend)]));
  strncpy(tempbuffer, texttosend, (int)strlen(texttosend));
  //int i;
  //for(i=0; i<26; i++)
  //{
  //  tempbuffer[i] = *texttosend[i];
  //}
  //*textbuffer = (char[strlen(*texttosend)])(*texttosend);
  //safewritetext(target_id, &tempbuffer);
  //ERR: HERE TEXTTOSEND HAS THE FILE CONTENTS
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
    //memset (p,FALSE,sz);
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
  return *dest;
}
