
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
char inputbuffer [CLIENT_BUFLEN]; //input from the server
char outputbuffer [CLIENT_BUFLEN]; //output to the server
int sd, ret;

int sendMessage(unsigned int command_id, unsigned int target_id,
  unsigned int client_id, char *message_text)
{
  //char *command_name = getcommand_name(command_id);

  return ERROR;
}

/*

        TODO: SAFEREAD NEVER FINISHES, WHY????


*/


void waitforserverresponse(int filedes, void *readbuffer)
{
  while(1 == 1)
  {
    if(saferead(filedes, readbuffer) != 0)
      return;
  }
}

int saferead(int filedes, void *readbuffer)
{
  memset (readbuffer,FALSE,CLIENT_BUFLEN);
  int readresult = read (filedes, readbuffer, CLIENT_BUFLEN - 1);
  if (readresult < 0)
    errexit ("reading error",NULL);
  return readresult;
}

void safescanf(char **buffer)
{
  if(scanf("%s", outputbuffer) < 0)
    exit(EXIT_ERRORCODE);
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

    /* capture and print the welcome message from the server */
    //memset (inputbuffer,FALSE,CLIENT_BUFLEN);
    //ret = read (sd,inputbuffer,CLIENT_BUFLEN - 1);
    //if (ret < 0)
    //  errexit ("reading error",NULL);
    //fprintf (stdout,"%s\n",inputbuffer);


    waitforserverresponse(sd, (void *)&inputbuffer);
    printf("%s", inputbuffer);
    //saferead(sd, (void *)&inputbuffer);

    /*capture whatever the user types and send the appropriate message */
    safescanf((char **)&outputbuffer);

    printf("Connected as user: %s\n", outputbuffer);

    //TODO: add an exit condition
    /* main loop */
    while(1 == 1)
    {
      /* capture and print whatever the server provides */
      saferead(sd, inputbuffer);
      printf ("%s\n",inputbuffer);
    }

    /* close & exit */
    close (sd);
    exit (0);
}
