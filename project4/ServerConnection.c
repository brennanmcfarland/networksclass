
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "ServerConnection.h"
#include "Connection.h"
#include "McFarlandNetworks.h"


struct sockaddr_in sockin;
struct sockaddr addr;
struct protoent *protoinfo;
unsigned int addrlen;
int sd, sd2;

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

      /* write message to the connection */
      if (write (sd2,argv [SERVER_MSG_POS],strlen (argv [SERVER_MSG_POS])) < 0)
        errexit ("error writing message: %s", argv [SERVER_MSG_POS]);
    }

    /* close connections and exit */
    close (sd);
    close (sd2);
    exit (0);
}
