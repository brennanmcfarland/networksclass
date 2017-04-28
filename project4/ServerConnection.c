
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h> //for directory handling
#include <sys/stat.h> //for directory handling
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
char *textoutputbuffer; //pure text output to a client
char *textinputbuffer; //pure text input from a client

unsigned int currentclient_id; //id of the current client, to be replaced if handled in parallel


/* could turn this into a hashtable in the future, but for a small number of
 connections a simple array will do*/
//char **clientmaps; //mappings from client name to client id
//int clientmapssize = 0; //the number of client name-id mappings
//int clientmapscapacity = IDNAMEMAPTABLECAPACITY;


//this is only useful if there's multiple parallel connections, so for now it's pointless
unsigned int generateclient_id(char **client_name)
{
  currentclient_id = 1;
  return 1;
}

int usage (char *progname)
{
    fprintf (stderr,"usage: %s port\n", progname);
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

  textinputbuffer = (char *)safemalloc(sizeof(char[BUFLEN]));
  textinputbuffer = "";
  textoutputbuffer = (char *)safemalloc(sizeof(char[BUFLEN]));
  textoutputbuffer = "";

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
