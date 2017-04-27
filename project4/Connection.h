
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
#define MAXFILEREADSIZE 4096


#define CMDID_GENERATECLIENTID 32
#define CMDID_LISTFILES 34
#define CMDID_READFILE 36
#define CMDID_WRITEFILE 38
#define CMDID_QUIT 99

char *getcommand_name(unsigned int command_id);
void awaitresponse(int filedes, void *readbuffer);
int saferead(int filedes, void *readbuffer);
int safereadtext(int filedes, void *readbuffer);
int safereadcommand(int filedes, void *readbuffer);
char *receivetext(char *texttoreceive, char *textbuffer, int source_id);
void safewrite(int filedes, void *writebuffer);
void safewritetext(int filedes, char *writebuffer);
void safewritecommand(int filedes, void *writebuffer);
void sendtext(char *texttosend, char *textbuffer, unsigned int target_id);
void *safemalloc (unsigned int sz);
void *saferealloc (void *buffer, unsigned int sz);
char *safestrcpy(char **dest, char **src);


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

  //if the message includes text, eg a post, it goes here
  //may just replace this with another struct that will follow certain commands
  //char *message_text;

} CommandMessage;
