
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


/*
  TODO:
    fix username input so it doesn't just capture the first word
    is it possible to overflow the buffers?  may have to look into that
*/

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

//generate a unique id for the client and send it to the server
unsigned int generateclient_id(char **client_name)
{
  //message the server
  CommandMessage message_generate_id = *(CommandMessage *)safemalloc(sizeof(CommandMessage));
  message_generate_id.command_id = CMDID_GENERATECLIENTID;
  message_generate_id.command_name = getcommand_name(message_generate_id.command_id);
  message_generate_id.target_id = FALSE;
  message_generate_id.target_name = NULL;
  message_generate_id.client_id = FALSE;
  message_generate_id.client_name = *client_name;
  message_generate_id.message_text = NULL;
  safewrite(sd, (void *)&message_generate_id);

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
  //just returns null if nothing read or error
  //fgets(*buffer, sizeof((char[])(*buffer)), stdin);

  //if(scanf("%s", *buffer) < 0)
  //  exit(EXIT_ERRORCODE);

/*
  GRABBED THIS FROM THE NET, SEE IF IT WILL WORK FOR VARIABLE LENGTH SCANNING

*/
  int size = 10;
//The size is extended by the input with the value of the provisional
  char *str;
  int ch;
  size_t len = 0;
  str = realloc(NULL, sizeof(char)*size);//size is start size TODO: will need to make safe
  while(EOF!=(ch=fgetc(stdin)) && ch != '\n'){
      str[len++]=ch;
      if(len==size){
          str = realloc(str, sizeof(char)*(size+=16)); //TODO: will need to make safe
      }
  }
  str[len++]='\0';

  *buffer = realloc(str, sizeof(char)*len); //TODO: will need to make safe

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
    //waitforserverresponse(sd, (void *)&inputbuffer);
    printf("Welcome.  Please input a username: \n");
    //char *testarr;
    //char **test;
    //test = &testarr;
    //testarr = "dlskjf hd";
    //safescanf(test);
    //printf("%s", testarr);
    //fflush(stdout);

    safescanf((char **)&userinputbuffer);
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
