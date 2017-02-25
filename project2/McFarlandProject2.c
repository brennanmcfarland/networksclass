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
#include </usr/include/netinet/ip.h> //for the iphdr struct
#include "Hashtable.c"
#include "McFarlandProject2.h"

/*
  TODO
  -may need to break up main function into components
  -wrap long lines
  -there's a bug in -s, run with sample B gives incorrect ms for end and incorrect duration
  -may want to move location of analyzePacketTrace, not sure
*/

//prints packet trace summary
void printTraceSummary(int numpackets, double firstpackettimestamp, double lastpackettimestamp)
{
  printf("PACKETS: %d\n", numpackets);
  printf("FIRST: %.6f\n", firstpackettimestamp);
  printf("LAST: %.6f\n", lastpackettimestamp);
  printf("DURATION: %.6f\n", lastpackettimestamp-firstpackettimestamp);
}

//prints ethernet header info
void printEthernetHeaderInfo(double timestamp, char *sourceaddress, char *destinationaddress)
{
  char *truncatedethernetheadermessage = "Ethernet-truncated"; //double check to make sure it's the same
  if(testStringEquality(sourceaddress,truncatedethernetheadermessage))
    printf("%.6f %s\n", timestamp, truncatedethernetheadermessage);
  else
    printf("%.6f %s %s\n", timestamp, sourceaddress, destinationaddress);
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

//analyzes the packet trace as specified by the user input
void analyzePacketTrace(FILE *tracefilestream, int flags[])
{
  PacketMetaInfo tracepacketmetainfo = *(PacketMetaInfo *)safeMalloc(sizeof(PacketMetaInfo));
  PacketMetaInfo *tracepacketmetainfobuffer = &tracepacketmetainfo;
  memset(tracepacketmetainfobuffer, FALSE, sizeof(PacketMetaInfo));

  PacketEthernetHeader tracepacketethernetheader = *(PacketEthernetHeader *)safeMalloc(sizeof(PacketEthernetHeader));
  PacketEthernetHeader *tracepacketethernetheaderbuffer = &tracepacketethernetheader;
  memset(tracepacketethernetheaderbuffer, FALSE, sizeof(PacketEthernetHeader));

  struct iphdr tracepacketipheader = *(struct iphdr *)safeMalloc(sizeof(struct iphdr));
  struct iphdr *tracepacketipheaderbuffer = &tracepacketipheader;
  memset(tracepacketipheaderbuffer, FALSE, sizeof(struct iphdr));

  int numpackets = 0;
  double firstpackettimestamp = -1.0; //default value to signal not set

  //for every packet
  //read packet metadata
  while(safeRead(tracefilestream, (void *)tracepacketmetainfobuffer, sizeof(PacketMetaInfo)) != 0)
  {
    if(flags[FLAG_VERBOSEOUTPUT] == TRUE)
      printf("Reading packet...\n");

    numpackets++;
    packetMetaInfoToHostOrder(tracepacketmetainfobuffer);
    if(firstpackettimestamp == -1.0)
    {
      firstpackettimestamp = (double)tracepacketmetainfo.meta_secsinceepoch
        +formatAsTrailingDecimal(tracepacketmetainfo.meta_msecsincesec);
    }

    //read ethernet header
    if(tracepacketmetainfo.meta_caplen >= sizeof(PacketEthernetHeader))
    {
      safeRead(tracefilestream, (void *)tracepacketethernetheaderbuffer, sizeof(PacketEthernetHeader));
      tracepacketmetainfo.meta_caplen -= sizeof(PacketEthernetHeader);
    }
    else if(flags[FLAG_PRINTETHERNETHEADERS] == TRUE )
      printEthernetHeaderInfo((double)tracepacketmetainfo.meta_secsinceepoch
        +formatAsTrailingDecimal(tracepacketmetainfo.meta_msecsincesec),
        "Ethernet-truncated", "Ethernet-truncated");
    //read ip header
    if(tracepacketmetainfo.meta_caplen >= sizeof(struct iphdr))
    {
      safeRead(tracefilestream, (void *)tracepacketipheaderbuffer, sizeof(struct iphdr));
      tracepacketmetainfo.meta_caplen -= sizeof(struct iphdr);
    }
    else if(flags[FLAG_PRINTIPHEADERS] == TRUE)
    {
      printEthernetHeaderInfo((double)tracepacketmetainfo.meta_secsinceepoch
        +formatAsTrailingDecimal(tracepacketmetainfo.meta_msecsincesec),
        "IP-truncated", "IP-truncated");
    }
    //read any remaining bits
    safeRead(tracefilestream, safeMalloc(tracepacketmetainfo.meta_caplen), tracepacketmetainfo.meta_caplen);
  }

  double lastpackettimestamp = (double)tracepacketmetainfo.meta_secsinceepoch
    +formatAsTrailingDecimal(tracepacketmetainfo.meta_msecsincesec);

  if(flags[FLAG_VERBOSEOUTPUT] == TRUE)
    printf("Reached end of file.\n");

  if(flags[FLAG_PRINTTRACESUMMARY] == TRUE)
    printTraceSummary(numpackets, firstpackettimestamp, lastpackettimestamp);
}

//converts data in packetmetainfo to host order
void packetMetaInfoToHostOrder(PacketMetaInfo * packetmetainfo)
{
  packetmetainfo->meta_secsinceepoch = ntohl(packetmetainfo->meta_secsinceepoch);
  packetmetainfo->meta_msecsincesec = ntohl(packetmetainfo->meta_msecsincesec);
  packetmetainfo->meta_ignored = ntohs(packetmetainfo->meta_msecsincesec);
  packetmetainfo->meta_caplen = ntohs(packetmetainfo->meta_caplen);
}

//converts data in packetethernetheader to host order
void PacketEthernetHeaderToHostOrder(PacketEthernetHeader * packetethernetheader)
{
  packetethernetheader->eth_protocoltype = ntohs(packetethernetheader->eth_protocoltype);
}

//returns whether 2 strings are equal
int testStringEquality(char *string1, char *string2)
{
  if(sizeof(string1) != sizeof(string2))
    return FALSE;
  int i;
  for(i=0; i<sizeof(string1); i++)
  {
    if(string1[i] != string2[i])
      return FALSE;
  }
  return TRUE;
}

//given an integer, formats as a decimal value trailing the decimal point
double formatAsTrailingDecimal(int integerdigits)
{
  double trailingdecimal = (double)integerdigits;
  trailingdecimal = trailingdecimal/1000000;
  return trailingdecimal;
  //while(trailingdecimal >= 1.0)
  //{
  //  trailingdecimal = trailingdecimal/10;
  //}
  //return trailingdecimal;
}

//handles parsing of all user input arguments, sets flags and vars appropriately
void parseInput(int argc, char *argv[], int *flags, char **tracefilename)
{
  int i;
  for(i=0; i<NUMFLAGS; i++)
    flags[i] = FALSE;
  int input;
  //read input args into variables
  while((input = getopt(argc, argv, "seitmvr:")) != -1)
  {
    int prevtracefilenameset = flags[FLAG_TRACEFILENAME];
    char *option = parseInputArg(input, flags);

    //if the given argument is r, copy the input file name
    if(flags[FLAG_TRACEFILENAME] == TRUE && prevtracefilenameset == FALSE)
    {
      //no need to check if null; already handled by getopt
      *tracefilename = option;
    }
  }
  if(flags[FLAG_TRACEFILENAME] == FALSE)
  {
    printf("Error: must specify an input file.\n");
    exit(1);
  }

  if(flags[FLAG_VERBOSEOUTPUT] == TRUE)
    printf("input file name: %s\n",*tracefilename);
}

//sets argument flag and returns any options given
char *parseInputArg(int inputargtoparse, int flags[])
{
    switch(inputargtoparse)
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

void safeOpen(FILE **filestream, char *filename, char filemode)
{
  char *filemodeptr = &filemode;
  if((*filestream = fopen(filename,filemodeptr)) == NULL)
  {
    printf("Error: Unable to read from file.\n");
    exit(1);
  }
}

void *safeMalloc (unsigned int sz)
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
  char *tracefilename = NULL;
  FILE *tracefilestream = NULL;
  parseInput(argc, argv, flags, &tracefilename);

  safeOpen(&tracefilestream, tracefilename, 'r');

  analyzePacketTrace(tracefilestream, flags);

  return 0;
}
