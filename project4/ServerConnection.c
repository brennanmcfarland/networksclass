
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "ServerConnection.h"
#include "Connection.c"
#include "McFarlandNetworks.h"
#include "ConnectionHashtable.c"

/*
  TODO: terminate a connection

*/

struct sockaddr_in sockin;
struct sockaddr addr;
struct protoent *protoinfo;
unsigned int addrlen;
int sd, sd2;

CommandMessage inputbuffer; //input from clients
CommandMessage outputbuffer; //output to a client

unsigned int currentclient_id; //id of the current client, to be replaced if handled in parallel

/* could turn this into a hashtable in the future, but for a small number of
 connections a simple array will do*/
//char **clientmaps; //mappings from client name to client id
//int clientmapssize = 0; //the number of client name-id mappings
//int clientmapscapacity = IDNAMEMAPTABLECAPACITY;

/*
//returns the new client id
unsigned int generateclient_id(char **client_name)
{
  //initialize the list of ids if it's not already
  if(clientmaps == NULL)
    clientmaps = safemalloc(sizeof(char *[clientmapscapacity]));
  //grow the list of ids if it's full
  else if(clientmapssize >= clientmapscapacity)
  {
    clientmapscapacity = clientmapscapacity*IDNAMEMAPTABLEGROWTHFACTOR;
    //TODO: need to actually copy the mappings over to a new array
  }

  //and add the new id
  clientmaps[clientmapssize] = *client_name;
  return 1;
}
*/

//this is only useful if there's multiple parallel connections, so for now it's pointless
unsigned int generateclient_id(char **client_name)
{
  currentclient_id = 1;
  return 1;
}

//wait for the client to respond with a message
void waitforclientresponse(int filedes, void *readbuffer)
{
  while(1 == 1)
  {
    if(saferead(filedes, readbuffer) != 0)
      return;
  }
}

void handleconnection(int sd)
{
  /* ask for login information and wait for response*/
  waitforclientresponse(sd, (void *)&inputbuffer);
  CommandMessage message_generate_id = inputbuffer;
  if(message_generate_id.command_id != CMDID_GENERATECLIENTID)
    errexit("error generating user id", NULL);
  generateclient_id((char **)&(message_generate_id.client_name));
  printf("Server recognizes user: %s\n", message_generate_id.client_name);

  //saferead(sd, inputbuffer);
  //printf("hallo");
  //printf("%s", (char *)inputbuffer);

  //while(saferead(sd, buffer) != 0)
  //  generateclient_id((char **)&buffer);

  /* test stuff (remove this later) */
  //printf("%s", clientmaps[0]);
  /* write test message to the connection (remove this later)*/
  //safewrite(sd2, argv[SERVER_MSG_POS]);
}

int usage (char *progname)
{
    fprintf (stderr,"usage: %s port msg\n", progname);
    exit (EXIT_ERRORCODE);
}

int errexit (char *format, char *arg)
{
    fprintf (stderr,format,arg);
    fprintf (stderr,"\n");
    exit (EXIT_ERRORCODE);
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

void safefileopen(FILE **filestream, char *filename, char filemode)
{
  char *filemodeptr = &filemode;
  if((*filestream = fopen(filename,filemodeptr)) == NULL)
  {
    printf("Error: Unable to read from file.\n");
    exit(EXIT_ERRORCODE);
  }
}

//initialize the client-server connection
void init(int argc, char *argv [])
{
  if (argc != SERVER_REQUIRED_ARGC)
      usage (argv [0]);

  /* determine protocol */
  if ((protoinfo = getprotobyname (PROTOCOL)) == NULL)
      errexit ("cannot find protocol information for %s", PROTOCOL);

  /* setup endpoint info */
  memset ((char *)&sockin,0x0,sizeof (sockin));
  sockin.sin_family = AF_INET;
  sockin.sin_addr.s_addr = INADDR_ANY;
  sockin.sin_port = htons ((u_short) atoi (argv [SERVER_PORT_POS]));

  /* allocate a socket */
  /*   would be SOCK_DGRAM for UDP */
  sd = socket(PF_INET, SOCK_STREAM, protoinfo->p_proto);
  if (sd < 0)
      errexit("cannot create socket", NULL);

  /* bind the socket */
  if (bind (sd, (struct sockaddr *)&sockin, sizeof(sockin)) < 0)
      errexit ("cannot bind to port %s", argv [SERVER_PORT_POS]);

  initializeConnections();

}

int main (int argc, char *argv [])
{

  /* initialize the server */
  init(argc, argv);

  /* listen for incoming connections */
  if (listen (sd, QLEN) < 0)
    errexit ("cannot listen on port %s\n", argv [SERVER_PORT_POS]);

  /* handle connections serially */
  //could change later to do a thread for each connection
  while(1 == 1)
  {
    sd2 = accept (sd,&addr,&addrlen);
    if (sd2 < 0)
      errexit ("error accepting connection", NULL);
    handleconnection(sd2);
    close (sd2);
  }

  /* close connection sd and exit */
  close (sd);
  exit (0);
}