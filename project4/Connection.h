
#define PROTOCOL "tcp"
#define BYTESIZE 1
#define TRUE 1
#define FALSE 0
#define EXIT_ERRORCODE 1
#define BUFLEN sizeof(CommandMessage)
#define IDNAMEMAPTABLECAPACITY 16
#define IDNAMEMAPTABLEGROWTHFACTOR 2
#define MAXCOMMANDNAMESIZE 32

#define CMDID_GENERATECLIENTID 32

char *getcommand_name(unsigned int command_id);
int saferead(int filedes, void *readbuffer);
int safereadcommand(int filedes, void *readbuffer);
void safewrite(int filedes, void *writebuffer);
void safewritecommand(int filedes, void *writebuffer);
void *safemalloc (unsigned int sz);


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
