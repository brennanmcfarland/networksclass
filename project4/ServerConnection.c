
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


struct sockaddr_in sockin;
struct sockaddr addr;
struct protoent *protoinfo;
unsigned int addrlen;
int sd, sd2;
char buffer [SERVER_BUFLEN];

/* could turn this into a hashtable in the future, but for a small number of
 connections a simple array will do*/
char **clientmaps; //mappings from client name to client id
int clientmapssize = 0; //the number of client name-id mappings
int clientmapscapacity = IDNAMEMAPTABLECAPACITY;

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
  return clientmapssize-1;
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

//this isn't working right, keeps throwing a reading error, am i reading the wrong thing?
int saferead(int filedes, void *readbuffer)
{
  memset (buffer,FALSE,SERVER_BUFLEN);
  int readresult = read (filedes,buffer,SERVER_BUFLEN - 1);
  if (readresult < 0)
    errexit ("reading error",NULL);
  return readresult;
}

void safewrite(int filedes, void *writebuffer)
{
  if (write (filedes, writebuffer, strlen (writebuffer)) < 0)
    errexit ("error writing message: %s", writebuffer);
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

void *safemalloc (unsigned int sz)
{
    void *p;
    if ((p = (void *)malloc (sz)) == NULL)
    {
        printf ("memory allocation failed, exiting ...\n");
        exit (EXIT_ERRORCODE);
    }
    memset (p,FALSE,sz);
    return (p);
}

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
}

int main (int argc, char *argv [])
{

    /* initialize the server */
    init(argc, argv);

    /* listen for incoming connections */
    if (listen (sd, QLEN) < 0)
        errexit ("cannot listen on port %s\n", argv [SERVER_PORT_POS]);

    while(1 == 1)
    {
      /* accept a connection */
      sd2 = accept (sd,&addr,&addrlen);
      if (sd2 < 0)
        errexit ("error accepting connection", NULL);

      /* ask for login information and wait for response*/
      safewrite(sd2, "Welcome.  Please input a username:");

      saferead(sd2, buffer);
      printf("%s", (char *)buffer);
      //while(saferead(sd, buffer) != 0)
      //  generateclient_id((char **)&buffer);

      /* test stuff (remove this later) */
      //printf("%s", clientmaps[0]);
      /* write test message to the connection (remove this later)*/
      //safewrite(sd2, argv[SERVER_MSG_POS]);

    }

    /* close connections and exit */
    close (sd);
    close (sd2);
    exit (0);
}
