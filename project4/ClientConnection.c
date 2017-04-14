
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "ClientConnection.h"
#include "Connection.h"
#include "McFarlandNetworks.h"


struct sockaddr_in sockin;
struct hostent *hinfo;
struct protoent *protoinfo;
char buffer [CLIENT_BUFLEN];
int sd, ret;

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

    //TODO: add an exit condition
    while(1 == 1)
    {
      /* snarf whatever server provides and print it */
      memset (buffer,0x0,CLIENT_BUFLEN);
      ret = read (sd,buffer,CLIENT_BUFLEN - 1);
      if (ret < 0)
        errexit ("reading error",NULL);
        fprintf (stdout,"%s\n",buffer);
    }

    /* close & exit */
    close (sd);
    exit (0);
}
