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
  -may want to move location of analyzePacketTrace, not sure
*/

//vars for operations
FILE *tracefilestream;
int flags[NUMFLAGS];

//packet info structs
PacketMetaInfo tracepacketmetainfo;
PacketMetaInfo *tracepacketmetainfobuffer = &tracepacketmetainfo;
PacketEthernetHeader tracepacketethernetheader;
PacketEthernetHeader *tracepacketethernetheaderbuffer = &tracepacketethernetheader;
struct iphdr tracepacketipheader;
struct iphdr *tracepacketipheaderbuffer = &tracepacketipheader;
struct iphdr tracepacketipheader;

//vars for aggregate data
int numpackets = 0;
double firstpackettimestamp = -1.0;
double lastpackettimestamp = -1.0;
int eth_numfullpackets = 0;
int eth_numpartialpackets = 0;
int numnonippackets = 0;
int ip_numpartialpackets = 0;
int ip_numpartialheaders = 0;
int ip_numsourceaddresses = 0;
int ip_numdestinationaddresses = 0;
int ip_numtcppackets = 0;
int ip_numupdpackets = 0;


//prints packet trace summary
void printTraceSummary()
{
  printf("PACKETS: %d\n", numpackets);
  printf("FIRST: %.6f\n", firstpackettimestamp);
  printf("LAST: %.6f\n", lastpackettimestamp);
  printf("DURATION: %.6f\n", lastpackettimestamp-firstpackettimestamp);
}

//prints ethernet header info
void printEthernetHeaderInfo(double timestamp, char *sourceaddress, char *destinationaddress, unsigned short protocoltype)
{
  char *truncatedethernetheadermessage = "Ethernet-truncated";
  if(testStringEquality(sourceaddress, truncatedethernetheadermessage))
  {
    printf("%.6f %s\n", timestamp, truncatedethernetheadermessage);
  }
  else
  {
    printf("%.6f ", timestamp);
    printMACAddress(sourceaddress);
    printf(" ");
    printMACAddress(destinationaddress);
    printf(" ");
    //if the least significant byte is nonzero it appends 0s
    printf("0x%02x%02x\n", (protocoltype & 0xff00)/0xff, (protocoltype & 0x00ff));
  }
}

//prints ip header info
void printIPHeaderInfo(double timestamp, char sourceaddress[IPADDRESSSIZE],
  char destinationaddress[IPADDRESSSIZE], unsigned int ihl, u_int8_t protocol, u_int8_t ttl)
{
  char *truncatedipheadermessage = "IP-truncated";
  char *nonipheadermessage = "non-IP";
  char *unknownwhetherip = "unknown";
  if(testStringEquality(sourceaddress, truncatedipheadermessage))
  {
    printf("%.6f %s\n", timestamp, truncatedipheadermessage);
  }
  else if(testStringEquality(sourceaddress, nonipheadermessage))
  {
    printf("%.6f %s\n", timestamp, nonipheadermessage);
  }
  else if(testStringEquality(sourceaddress, unknownwhetherip))
  {
    printf("%.6f %s\n", timestamp, unknownwhetherip);
  }
  else
  {
    printf("%.6f ", timestamp);
    printIPAddress(sourceaddress);
    printf(" ");
    printIPAddress(destinationaddress);
    printf(" ");
    printf("%u %d %d\n", ihl*WORDSIZE, protocol, ttl);
  }
}

void printPacketTypes()
{
  printf("ETH: %d %d\n", eth_numfullpackets, eth_numpartialpackets);
  printf("NON-IP: %d\n", numnonippackets);
  printf("IP: %d %d\n", ip_numpartialpackets, ip_numpartialheaders);
  printf("SRC: %d\n", ip_numsourceaddresses);
  printf("DST: %d\n", ip_numdestinationaddresses);
  printf("TRANSPORT: %d %d %d\n", ip_numtcppackets, ip_numupdpackets,
    (ip_numpartialpackets-ip_numtcppackets-ip_numupdpackets));
}

//formats and prints a single MAC address
void printMACAddress(char macaddress[MACADDRESSSIZE])
{
  int macaddressnumberbyte = (int)(macaddress[0]);
  printf("%02x",(unsigned char)macaddressnumberbyte);
  int i;
  for(i=1; i<MACADDRESSSIZE; i++)
  {
    macaddressnumberbyte = (int)(macaddress[i]);
    printf(":%02x",(unsigned char)macaddressnumberbyte);
  }
}

//formats and prints a single IP address
void printIPAddress(char ipaddress[IPADDRESSSIZE])
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
void analyzePacketTrace()
{
  //read packet metadata
  while(safeRead(tracefilestream, (void *)tracepacketmetainfobuffer, sizeof(PacketMetaInfo)) != 0)
  {
    if(flags[FLAG_VERBOSEOUTPUT] == TRUE)
      printf("Reading packet...\n");
    numpackets++;
    packetMetaInfoToHostOrder(tracepacketmetainfobuffer);
    if(firstpackettimestamp == -1.0)
    {
      firstpackettimestamp = formatTimeStamp(
        tracepacketmetainfo.meta_secsinceepoch, tracepacketmetainfo.meta_msecsincesec);
    }

    //read ethernet header
    analyzePacketEthernetHeader(tracepacketmetainfobuffer);

    //read ip header
    char *formattedprotocoltype = safeMalloc(sizeof(unsigned short));
    sprintf(formattedprotocoltype, "0x%02x%02x",
      (tracepacketethernetheader.eth_protocoltype & 0xff00)/0xff, (tracepacketethernetheader.eth_protocoltype & 0x00ff));
    if((testStringEquality(formattedprotocoltype, "0x0800") == FALSE))
    {
      numnonippackets++;
      if(flags[FLAG_PRINTIPHEADERS] == TRUE)
        printIPHeaderInfo(formatTimeStamp(
          tracepacketmetainfo.meta_secsinceepoch, tracepacketmetainfo.meta_msecsincesec),
          "non-IP", "", 0, 0, 0);
    }
    else
    {
      analyzePacketIPHeader();
      if(flags[FLAG_VERBOSEOUTPUT] == TRUE)
        printf("%d",tracepacketipheader.protocol);
    }
    //read any remaining bits
    safeRead(tracefilestream, safeMalloc(tracepacketmetainfo.meta_caplen), tracepacketmetainfo.meta_caplen);
  }

  lastpackettimestamp = formatTimeStamp(
    tracepacketmetainfo.meta_secsinceepoch, tracepacketmetainfo.meta_msecsincesec);

  if(flags[FLAG_VERBOSEOUTPUT] == TRUE)
    printf("Reached end of file.\n");

  if(flags[FLAG_PRINTTRACESUMMARY] == TRUE)
    printTraceSummary();
  if(flags[FLAG_PRINTPACKETTYPES] == TRUE)
    printPacketTypes();
}

//analyze a single ethernet packet header
void analyzePacketEthernetHeader(PacketMetaInfo *tracepacketmetainfo)
{
  if(tracepacketmetainfo->meta_caplen >= sizeof(PacketEthernetHeader))
  {
    safeRead(tracefilestream, (void *)tracepacketethernetheaderbuffer, sizeof(PacketEthernetHeader));
    tracepacketmetainfo->meta_caplen -= sizeof(PacketEthernetHeader);
    packetEthernetHeaderToHostOrder(tracepacketethernetheaderbuffer);
    if(flags[FLAG_PRINTETHERNETHEADERS] == TRUE)
      printEthernetHeaderInfo(formatTimeStamp(
        tracepacketmetainfo->meta_secsinceepoch, tracepacketmetainfo->meta_msecsincesec),
        tracepacketethernetheader.eth_srcaddress, tracepacketethernetheader.eth_destaddress,
        tracepacketethernetheader.eth_protocoltype);
    eth_numfullpackets++;
  }
  else
  {
    eth_numpartialpackets++;
    if(flags[FLAG_PRINTETHERNETHEADERS] == TRUE)
      printEthernetHeaderInfo(formatTimeStamp(
        tracepacketmetainfo->meta_secsinceepoch, tracepacketmetainfo->meta_msecsincesec),
        "Ethernet-truncated", "", 0);
    if(flags[FLAG_PRINTIPHEADERS] == TRUE)
      printIPHeaderInfo(formatTimeStamp(
        tracepacketmetainfo->meta_secsinceepoch, tracepacketmetainfo->meta_msecsincesec),
        "unknown", "", 0, 0, 0);
  }
}

//analyze a single ip packet header
void analyzePacketIPHeader()
{
  if(tracepacketmetainfo.meta_caplen >= sizeof(struct iphdr))
  {
    safeRead(tracefilestream, (void *)tracepacketipheaderbuffer, sizeof(struct iphdr));
    iphdrToHostOrder(tracepacketipheaderbuffer);
    tracepacketmetainfo.meta_caplen -= sizeof(struct iphdr);
    ip_numpartialpackets++;
    if(tracepacketipheader.protocol == 6)
      ip_numtcppackets++;
    if(tracepacketipheader.protocol == 17)
      ip_numupdpackets++;
    if(flags[FLAG_PRINTIPHEADERS] == TRUE)
    {
      printIPHeaderInfo(formatTimeStamp(
        tracepacketmetainfo.meta_secsinceepoch, tracepacketmetainfo.meta_msecsincesec),
        formatIPAddress(tracepacketipheader.saddr), formatIPAddress(tracepacketipheader.daddr),
        tracepacketipheader.ihl, tracepacketipheader.protocol,
        tracepacketipheader.ttl);
    }
  }
  else
  {
    ip_numpartialheaders++;
    if(flags[FLAG_PRINTIPHEADERS] == TRUE)
      printIPHeaderInfo(formatTimeStamp(
        tracepacketmetainfo.meta_secsinceepoch, tracepacketmetainfo.meta_msecsincesec),
        "IP-truncated", "", 0, 0, 0);
  }
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
void packetEthernetHeaderToHostOrder(PacketEthernetHeader * packetethernetheader)
{
  packetethernetheader->eth_protocoltype = ntohs(packetethernetheader->eth_protocoltype);
}

//converts data in iphdr to host order
void iphdrToHostOrder(struct iphdr * packetipheader)
{
  //packetipheader->ihl = ntohl(packetipheader->ihl);
  packetipheader->version = ntohl(packetipheader->version);
  packetipheader->tot_len = ntohs(packetipheader->tot_len);
  packetipheader->id = ntohs(packetipheader->id);
  packetipheader->frag_off = ntohs(packetipheader->frag_off);
  packetipheader->check = ntohs(packetipheader->check);
  packetipheader->saddr = ntohl(packetipheader->saddr);
  packetipheader->daddr = ntohl(packetipheader->daddr);
}

//returns whether 2 strings are equal
int testStringEquality(char *string1, char *string2)
{
  if(sizeof(string1) != sizeof(string2))
  {
    return FALSE;
  }
  int i;
  for(i=0; i<sizeof(string1)-1; i++)
  {
    if(string1[i] != string2[i])
    {
      return FALSE;
    }
  }
  return TRUE;
}

//formats ip address as a char array
char *formatIPAddress(u_int32_t ipaddressint)
{
  char *ipaddressarray = safeMalloc(IPADDRESSSIZE);
  int i;
  for(i=0; i<sizeof(ipaddressarray); i++)
  {
    ipaddressarray[i] = (ipaddressint >> 8*(sizeof(ipaddressarray)-1-i)) & 0xff;
  }
  return ipaddressarray;
}

//formats timestamp data for output
double formatTimeStamp(int secsinceepoch, int msecsincesec)
{
  return (double)secsinceepoch+formatAsTrailingDecimal(msecsincesec);
}

//given an integer, formats as a decimal value trailing the decimal point
double formatAsTrailingDecimal(int integerdigits)
{
  double trailingdecimal = (double)integerdigits;
  trailingdecimal = trailingdecimal/1000000;
  return trailingdecimal;

}

//handles parsing of all user input arguments, sets flags and vars appropriately
void parseInput(int argc, char *argv[], char **tracefilename)
{
  int i;
  for(i=0; i<NUMFLAGS; i++)
    flags[i] = FALSE;
  int input;
  //read input args into variables
  while((input = getopt(argc, argv, "seitmvr:")) != -1)
  {
    int prevtracefilenameset = flags[FLAG_TRACEFILENAME];
    char *option = parseInputArg(input);

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
char *parseInputArg(int inputargtoparse)
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
  char *tracefilename = NULL;
  tracepacketmetainfo = *(PacketMetaInfo *)safeMalloc(sizeof(PacketMetaInfo));
  tracepacketethernetheader = *(PacketEthernetHeader *)safeMalloc(sizeof(PacketEthernetHeader));
  tracepacketipheader = *(struct iphdr *)safeMalloc(sizeof(struct iphdr));


  parseInput(argc, argv, &tracefilename);

  safeOpen(&tracefilestream, tracefilename, 'r');

  analyzePacketTrace();

  return 0;
}
