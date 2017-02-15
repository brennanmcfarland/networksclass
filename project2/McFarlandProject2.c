/*
  Brennan McFarland
  bfm21
  McFarlandProject2.c
  [DATE]
  [DESCRIPTION]
*/

#include <unistd.h> //for getopt and file operations
#include <stdlib.h>
#include <stdio.h> //for file operations
#include <string.h> //for memset
#include <arpa/inet.h> //for converting between network and host order
#include "McFarlandProject2.h"

/*
  TODO
  -may need to break up main function into components
  -wrap long lines
*/

//prints packet trace summary
void printTraceSummary(int numpackets, double firstpackettimestamp, double lastpackettimestamp)
{
  printf("PACKETS: %d\n", numpackets);
  printf("FIRST: %.6f\n", firstpackettimestamp);
  printf("LAST: %.6f\n", lastpackettimestamp);
  printf("DURATION: %.6f\n", lastpackettimestamp-firstpackettimestamp);
}

//formats and prints a single IP address
void dumpIPAddress(char *ipaddress)
{
  int ipaddressnumberbyte = (int)(ipaddress[0]);
  printf("%d",(unsigned char)ipaddressnumberbyte);
  int i;
  for(i=1; i<IPADDRESSSIZE; i++)
  {
    ipaddressnumberbyte = (int)(ipaddress[i]);
    printf(".%d",(unsigned char)ipaddressnumberbyte);
  }
}

//converts data in packetmetainfo to host order
void packetMetaInfoNtohl(PacketMetaInfo * packetmetainfo)
{
  packetmetainfo->meta_secsinceepoch = ntohl(packetmetainfo->meta_secsinceepoch);
  packetmetainfo->meta_msecsincesec = ntohl(packetmetainfo->meta_msecsincesec);
  packetmetainfo->meta_ignored = ntohs(packetmetainfo->meta_msecsincesec);
  packetmetainfo->meta_caplen = ntohs(packetmetainfo->meta_caplen);
}

//given an integer, formats as a decimal value trailing the decimal point
double formatAsTrailingDecimal(int integerdigits)
{
  double trailingdecimal = (double)integerdigits;
  while(trailingdecimal >= 1.0)
  {
    trailingdecimal = trailingdecimal/10;
  }
  return trailingdecimal;
}

//sets argument flag and returns any options given
char *parseInput(int input, int flags[])
{
    switch(input)
    {
    case 's':
      flags[FLAG_PRINTTRACESUMMARY] = TRUE;
      break;
    case 'e':
      flags[FLAG_PRINTETHERNETHEADERS] = TRUE;
      break;
    case 'i':
      flags[FLAG_PRINTIPHEADERS] = TRUE;
      break;
    case 't':
      flags[FLAG_PRINTPACKETTYPES] = TRUE;
      break;
    case 'm':
      flags[FLAG_PRINTTRAFFICMATRIX] = TRUE;
      break;
    case 'v':
      flags[FLAG_VERBOSEOUTPUT] = TRUE;
      break;
    case 'r':
      flags[FLAG_TRACEFILENAME] = TRUE;
      return optarg;
      break;
    case '?':
      if(optopt == 'c')
      {
        exit(2);
      }
      else
      {
        exit(3);
      }
    }
    return NULL;
}

int safeRead(FILE *filestream, void *readbuffer, int readbuffersize)
{
  int readresult = fread(readbuffer,BYTESIZE,readbuffersize,filestream);
  if(readresult == 0 && ferror(filestream) != 0)
  {
    printf("Error reading from file.\n");
    exit(1);
  }
  return readresult;
}

void *safemalloc (unsigned int sz)
{
    void *p;
    if ((p = (void *)malloc (sz)) == NULL)
    {
        printf ("memory allocation failed, exiting ...\n");
        exit (1);
    }
    memset (p,FALSE,sz);
    return (p);
}

int main(int argc, char *argv[])
{
  int flags[NUMFLAGS];
  int i;
  for(i=0; i<NUMFLAGS; i++)
    flags[i] = FALSE;
  int input;
  char *tracefilename;
  FILE *tracefilestream = NULL;

  //read input args into variables
  while((input = getopt(argc, argv, "seitmvr:")) != -1)
  {
    int prevtracefilenameset = flags[FLAG_TRACEFILENAME];
    char *option = parseInput(input, flags);

    //if the given argument is r, copy the input file name
    if(flags[FLAG_TRACEFILENAME] == TRUE && prevtracefilenameset == FALSE)
    {
      //no need to check if null; already handled by getopt
      tracefilename = option;
    }
  }
  if(flags[FLAG_TRACEFILENAME] == FALSE)
  {
    printf("Error: must specify an input file.\n");
    return 1;
  }

  if(flags[FLAG_VERBOSEOUTPUT] == TRUE)
    printf("input file name: %s\n",tracefilename);

  //read the packet trace from the file until finished
  char filemode_read = 'r';
  char *filemode = &filemode_read;
  if((tracefilestream = fopen(tracefilename,filemode)) == NULL)
  {
    printf("Error: Unable to read from file.\n");
    exit(1);
  }

  PacketMetaInfo tracepacketmetainfo = *(PacketMetaInfo *)safemalloc(sizeof(PacketMetaInfo));
  PacketMetaInfo *tracepacketmetainfobuffer = &tracepacketmetainfo;
  memset(tracepacketmetainfobuffer,FALSE, sizeof(PacketMetaInfo));
  int numpackets = 0;
  double firstpackettimestamp = -1.0; //default value to signal not set

  //for every packet
  //read packet metadata
  while(safeRead(tracefilestream, (void *)tracepacketmetainfobuffer,sizeof(PacketMetaInfo)) != 0)
  {
    if(flags[FLAG_VERBOSEOUTPUT] == TRUE)
      printf("Reading packet...\n");

    numpackets++;
    packetMetaInfoNtohl(tracepacketmetainfobuffer);
    if(firstpackettimestamp == -1.0)
    {
      firstpackettimestamp = (double)tracepacketmetainfo.meta_secsinceepoch
        +formatAsTrailingDecimal(tracepacketmetainfo.meta_msecsincesec);
    }

    //read the rest of the bytes in the packet

  }

  double lastpackettimestamp = (double)tracepacketmetainfo.meta_secsinceepoch
    +formatAsTrailingDecimal(tracepacketmetainfo.meta_msecsincesec);

  //printf("%d", (unsigned int)((*(PacketMetaInfo *)(tracepacketheaderbuffer)).meta_secsinceepoch));

  if(flags[FLAG_VERBOSEOUTPUT] == TRUE)
    printf("Reached end of file.\n");

  if(flags[FLAG_PRINTTRACESUMMARY] == TRUE)
    printTraceSummary(numpackets, firstpackettimestamp, lastpackettimestamp);

  return 0;
}
