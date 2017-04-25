
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "ClientConnection.h"
#include "Connection.c"
#include "McFarlandNetworks.h"



struct sockaddr_in sockin;
struct hostent *hinfo;
struct protoent *protoinfo;

CommandMessage inputbuffer; //input from the server
CommandMessage outputbuffer; //output to the server

//char *textoutputbufferstr = "";
char *textoutputbuffer = ""; //pure text output to the server
//char textinputbuffer[TEXTBUFLEN]; //pure text input from the server
//char *textinputbufferstr = "";
char *textinputbuffer = "";
//char *userinputbufferstr = "";
char userinputbuffer[SCANBUFFERINITSIZE]; //input from the user
char *useroutputbuffer = ""; //output to display from the user

int sd, ret;
int client_id;
char *client_name;

//send a command
void sendcommandmessage(unsigned int command_id, unsigned int target_id)
{
  CommandMessage commandmessage = *(CommandMessage *)safemalloc(sizeof(CommandMessage));
  commandmessage.command_id = command_id;
  strcpy(commandmessage.command_name, getcommand_name(commandmessage.command_id));
  commandmessage.target_id = target_id;
  strcpy(commandmessage.target_name, ""); //TODO: make this something valid
  commandmessage.client_id = client_id;
  strcpy(commandmessage.client_name, client_name);
  safewritecommand(sd, (void *)&commandmessage);
}


//generate a unique id for the client and send it to the server
unsigned int generateclient_id(char *client_name)
{
  //message the server
  CommandMessage message_generate_id = *(CommandMessage *)safemalloc(sizeof(CommandMessage));
  message_generate_id.command_id = CMDID_GENERATECLIENTID;
  strcpy(message_generate_id.command_name, getcommand_name(message_generate_id.command_id));
  message_generate_id.target_id = FALSE;
  strcpy(message_generate_id.target_name, "");
  message_generate_id.client_id = FALSE;
  strcpy(message_generate_id.client_name, client_name);
  safewritecommand(sd, (void *)&message_generate_id);

  //and wait for the id as a response
  return 1;
}

/*
void waitforservertext(int filedes)
{
  //safescanf(&textinputbuffer);
  int scanbuffersize = SCANBUFFERINITSIZE;
  char *strbuffer;
  int nextchar;
  size_t lenofstr = 0;
  strbuffer = saferealloc(NULL, sizeof(char)*scanbuffersize);//size is start size
  const char filemode_read = 'r';
  printf("%c\n", fgetc(fdopen(filedes, &filemode_read)));
  while(EOF!=(nextchar=fgetc(fdopen(filedes, &filemode_read))) && nextchar != '\n'){
      strbuffer[lenofstr++]=nextchar;
      if(lenofstr==scanbuffersize){
          strbuffer = saferealloc(strbuffer, sizeof(char)*(scanbuffersize+=16));
      }
  }
  strbuffer[lenofstr++]='\0';
  textinputbuffer = saferealloc(strbuffer, sizeof(char)*lenofstr);
}
*/
void safescanf(char *buffer)
{
  int scanbuffersize = SCANBUFFERINITSIZE;
  //char strbuffer[SCANBUFFERINITSIZE];
  //size_t lenofstr = 0;
  //strbuffer = safemalloc(sizeof(char)*scanbuffersize);//size is start size
  fgets(buffer, scanbuffersize, stdin);

  //TODO: check for input too large, rn it causes undefined behavior
  //it needs to clear the buffer or otherwise things get messed up
  //could use getline() and throw the rest away?

  //strcpy(*buffer, strbuffer);
  //while(EOF!=(nextchar=fgetc(stdin)) && nextchar != '\n'){
  //    strbuffer[lenofstr++]=nextchar;
  //    if(lenofstr==scanbuffersize){
  //        strbuffer = saferealloc(strbuffer, sizeof(char)*(scanbuffersize+=16));
  //    }
  //}
  //strbuffer[lenofstr++]='\0';
  //if(strlen(*buffer) == 0)
    //*buffer = safemalloc(sizeof(char)*lenofstr);
  //*buffer = saferealloc(strbuffer, sizeof(char)*lenofstr);
}

int usage (char *progname)
{
    fprintf (stderr,"usage: %s host port\n", progname);
    exit (EXIT_ERRORCODE);
}

int errexit (char *format, char *arg)
{
    fprintf (stderr,format,arg);
    fprintf (stderr,"\n");
    exit (EXIT_ERRORCODE);
}

//initialize the client-server connection
void init(int argc, char *argv [])
{
  if (argc != CLIENT_REQUIRED_ARGC)
      usage (argv [0]);

  //textinputbuffer = (char **)safemalloc(sizeof(char[BUFLEN]));
  textoutputbuffer = (char *)safemalloc(sizeof(char[BUFLEN]));
  textoutputbuffer = "";

  /* lookup the hostname */
  hinfo = gethostbyname (argv [CLIENT_HOST_POS]);
  if (hinfo == NULL)
      errexit ("cannot find name: %s", argv [CLIENT_HOST_POS]);

  /* set endpoint information */
  memset ((char *)&sockin, 0x0, sizeof (sockin));
  sockin.sin_family = AF_INET;
  sockin.sin_port = htons (atoi (argv [CLIENT_PORT_POS]));
  memcpy ((char *)&sockin.sin_addr,hinfo->h_addr,hinfo->h_length);

  if ((protoinfo = getprotobyname (PROTOCOL)) == NULL)
      errexit ("cannot find protocol information for %s", PROTOCOL);

  /* allocate a socket */
  /*   would be SOCK_DGRAM for UDP */
  sd = socket(PF_INET, SOCK_STREAM, protoinfo->p_proto);
  if (sd < 0)
      errexit("cannot create socket",NULL);

  /* connect the socket */
  if (connect (sd, (struct sockaddr *)&sockin, sizeof(sockin)) < 0)
      errexit ("cannot connect", NULL);
}
