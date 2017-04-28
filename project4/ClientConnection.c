/*
  Brennan McFarland
  bfm21
  ClientConnection.c
  4/28/17
  handles the connection/core functionality of the client side
    -creates and sends messages specified by Client.c
    -generates a client id
    -reads input
    -initializes the connection
*/

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

char *textoutputbuffer = ""; //pure text output to the server
char *textinputbuffer = ""; //pure text input from the server
char userinputbuffer[SCANBUFFERINITSIZE]; //input from the user
char *useroutputbuffer = ""; //output to display from the user

int sd, ret;
int client_id;
char *client_name;

//send a command
void sendcommandmessage(unsigned int command_id, unsigned int target_id, char *target_name)
{
  CommandMessage *commandmessage = (CommandMessage *)safemalloc(sizeof(CommandMessage));
  commandmessage->command_id = command_id;
  strcpy(commandmessage->command_name, getcommand_name(commandmessage->command_id));
  commandmessage->target_id = target_id;
  if(target_name != NULL && strlen(target_name) != EMPTYSTRING)
    strcpy(commandmessage->target_name, target_name);
  commandmessage->client_id = client_id;
  strcpy(commandmessage->client_name, client_name);
  safewritecommand(sd, (void *)commandmessage);
  free(commandmessage);
}


//generate a unique id for the client and send it to the server
unsigned int generateclient_id(char *client_name)
{
  //message the server
  CommandMessage *message_generate_id = (CommandMessage *)safemalloc(sizeof(CommandMessage));
  message_generate_id->command_id = CMDID_GENERATECLIENTID;
  strcpy(message_generate_id->command_name, getcommand_name(message_generate_id->command_id));
  message_generate_id->target_id = FALSE;
  strcpy(message_generate_id->target_name, "");
  message_generate_id->client_id = FALSE;
  strcpy(message_generate_id->client_name, client_name);
  safewritecommand(sd, (void *)message_generate_id);
  free(message_generate_id);
  //rn this just returns true, in the future it could return a unique client id
  return TRUE;
}

int safescanf(char *buffer)
{
  int scanbuffersize = SCANBUFFERINITSIZE;
  fgets(buffer, scanbuffersize, stdin);

  //check for overly large input
  if(strlen(buffer) >= SCANBUFFERINITSIZE-OFFSETVALUE)
  {
    printf("Input size out of range.  Please try again:\n");
    char c;
    while ((c = getchar()) != '\n');
    safescanf(buffer);
    return EXIT_ERRORCODE;
  }
  return EXIT_NOERROR;
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
      usage (argv [INTINITIALIZER]);

  textoutputbuffer = (char *)safemalloc(sizeof(char[BUFLEN]));
  textoutputbuffer = "";

  /* lookup the hostname */
  hinfo = gethostbyname (argv [CLIENT_HOST_POS]);
  if (hinfo == NULL)
      errexit ("cannot find name: %s", argv [CLIENT_HOST_POS]);

  /* set endpoint information */
  memset ((char *)&sockin, INTINITIALIZER, sizeof (sockin));
  sockin.sin_family = AF_INET;
  sockin.sin_port = htons (atoi (argv [CLIENT_PORT_POS]));
  memcpy ((char *)&sockin.sin_addr,hinfo->h_addr,hinfo->h_length);

  if ((protoinfo = getprotobyname (PROTOCOL)) == NULL)
      errexit ("cannot find protocol information for %s", PROTOCOL);

  /* allocate a socket */
  sd = socket(PF_INET, SOCK_STREAM, protoinfo->p_proto);
  if (sd < EXIT_NOERROR)
      errexit("cannot create socket",NULL);

  /* connect the socket */
  if (connect (sd, (struct sockaddr *)&sockin, sizeof(sockin)) < EXIT_NOERROR)
      errexit ("cannot connect", NULL);
}
