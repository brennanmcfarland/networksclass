/*
  Brennan McFarland
  bfm21
  Connection.h
  4/28/17
  the header for Connection.c
  handles how data is read and written from files and sent across the network
  for client and server
    -sends/receives text/commands
    -performs file operations
    -memory allocation
*/

#define PROTOCOL "tcp"
#define BYTESIZE 1
#define TRUE 1
#define FALSE 0
#define EXIT_ERRORCODE 1
#define BUFLEN sizeof(CommandMessage)
#define TEXTBUFLEN 1024
#define IDNAMEMAPTABLECAPACITY 16
#define IDNAMEMAPTABLEGROWTHFACTOR 2
#define MAXCOMMANDNAMESIZE 32
#define MAXFILEREADSIZE 32768
#define EMPTYSTRING 0

#define CMDID_TEST 1
#define CMDID_GENERATECLIENTID 32
#define CMDID_LISTFILES 34
#define CMDID_READFILE 36
#define CMDID_WRITEFILE 38
#define CMDID_READSESSIONLOG 40
#define CMDID_GETTIME 42
#define CMDID_QUIT 99


char *getcommand_name(unsigned int command_id);
void awaitresponse(int filedes, void *readbuffer);
void awaittext(int filedes, void *readbuffer);
void sendfile(char *filename, char *buffer, char *filecontents,
  unsigned int clientid, char *directory, unsigned int callingcommand);
int saferead(int filedes, void *readbuffer);
int safereadtext(int filedes, void *readbuffer);
int safereadcommand(int filedes, void *readbuffer);
char *receivetext(char *texttoreceive, char *textbuffer, int source_id);
void safewrite(int filedes, void *writebuffer);
void safewritetext(int filedes, char *writebuffer);
void safewritecommand(int filedes, void *writebuffer);
int safefileread(FILE *filestream, void *readbuffer, int readbuffersize);
int safefilewrite(FILE *filestream, void *writebuffer, int writebuffersize);
void safefileopen(FILE **filestream, char *filename, char filemode);
void safefileclose(FILE **filestream);
void sendtext(char *texttosend, char *textbuffer, unsigned int target_id);
void *safemalloc (unsigned int sz);
void *saferealloc (void *buffer, unsigned int sz);
char *safestrcpy(char **dest, char **src);


//most of these fields are useless now, but could be expanded upon later
typedef struct CommandMessage
{
  //what command to execute
  unsigned int command_id;
  char command_name[MAXCOMMANDNAMESIZE];

  //the target of the command, which forum/message board/etc
  unsigned int target_id;
  char target_name[MAXCOMMANDNAMESIZE];

  //the client sending the command
  unsigned int client_id;
  char client_name[MAXCOMMANDNAMESIZE];

} CommandMessage;
