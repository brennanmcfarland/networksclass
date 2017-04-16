
#define PROTOCOL "tcp"
#define BYTESIZE 1
#define TRUE 1
#define FALSE 0
#define EXIT_ERRORCODE 1
#define BUFLEN sizeof(CommandMessage)
#define IDNAMEMAPTABLECAPACITY 16
#define IDNAMEMAPTABLEGROWTHFACTOR 2
#define MAXINPUTLENGTH 65536

#define CMDID_GENERATECLIENTID 32

char *getcommand_name(unsigned int command_id);
int saferead(int filedes, void *readbuffer);
void safewrite(int filedes, void *writebuffer);
void *safemalloc (unsigned int sz);


typedef struct CommandMessage
{
  //what command to execute
  unsigned int command_id;
  char *command_name;

  //the target of the command, which forum/message board/etc
  unsigned int target_id;
  char *target_name;

  //the client sending the command
  unsigned int client_id;
  char *client_name;

  //if the message includes text, eg a post, it goes here
  char *message_text;

} CommandMessage;
