
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
char *userinputbuffer; //input from the user
int sd, ret;
int client_id;

//send a message/command
int sendmessage(unsigned int command_id, unsigned int target_id,
  char *message_text)
{
  //char *command_name = getcommand_name(command_id);

  return ERROR;
}

void promptuser(char *inputmessage)
{
  printf("%s\n", inputmessage);
  safescanf((char **)&userinputbuffer);
  while(strlen(userinputbuffer) > MAXCOMMANDNAMESIZE)
  {
    printf("Input size out of range.  Please try again: ");
    safescanf((char **)&userinputbuffer);
  }
}

//generate a unique id for the client and send it to the server
unsigned int generateclient_id(char **client_name)
{
  //message the server
  CommandMessage message_generate_id = *(CommandMessage *)safemalloc(sizeof(CommandMessage));
  message_generate_id.command_id = CMDID_GENERATECLIENTID;
  strcpy(message_generate_id.command_name, getcommand_name(message_generate_id.command_id));
  message_generate_id.target_id = FALSE;
  strcpy(message_generate_id.target_name, "");
  message_generate_id.client_id = FALSE;
  strcpy(message_generate_id.client_name, *client_name);
  printf("%s\n", message_generate_id.client_name);
  safewritecommand(sd, (void *)&message_generate_id);

  //and wait for the id as a response
  return 1;
}

//wait for the server to respond with a message
void waitforserverresponse(int filedes, void *readbuffer)
{
  while(1 == 1)
  {
    if(saferead(filedes, readbuffer) != 0)
      return;
  }
}

void safescanf(char **buffer)
{
  int scanbuffersize = SCANBUFFERINITSIZE;
  char *strbuffer;
  int nextchar;
  size_t lenofstr = 0;
  strbuffer = saferealloc(NULL, sizeof(char)*scanbuffersize);//size is start size
  while(EOF!=(nextchar=fgetc(stdin)) && nextchar != '\n'){
      strbuffer[lenofstr++]=nextchar;
      if(lenofstr==scanbuffersize){
          strbuffer = saferealloc(strbuffer, sizeof(char)*(scanbuffersize+=16));
      }
  }
  strbuffer[lenofstr++]='\0';
  *buffer = saferealloc(strbuffer, sizeof(char)*lenofstr);
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

int main (int argc, char *argv [])
{

    /* initialize client */
    init(argc, argv);


    /* print the welcome message and send whatever the user types as to get id*/
    promptuser("Welcome.  Please input a username:");
    client_id = generateclient_id((char **)&userinputbuffer);
    printf("Client recognizes user: %s\n", userinputbuffer);

    //TODO: add an exit condition
    /* main loop */
    while(1 == 1)
    {
      /* capture and print whatever the server provides */
      saferead(sd, (void *)&inputbuffer);
    }

    /* close & exit */
    close (sd);
    exit (0);
}
