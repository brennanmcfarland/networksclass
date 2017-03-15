/*
  Brennan McFarland
  bfm21
  McFarlandProject2.h
  3/3/17
  Given an packet trace file, analyzes it for information about ethernet,
  ip properties, prints aggregated analysis of all packets to output,
  -s prints trace summary
  -e prints ethernet headers
  -i prints ip headers
  -t prints packet types
  -m prints traffic matrix
*/

#include <unistd.h> //for getopt and file operations
#include <stdlib.h>
#include <stdio.h> //for file operations
#include <string.h> //for memset
#include <arpa/inet.h> //for converting between network and host order
#include </usr/include/netinet/ip.h> //for the iphdr struct
#include "IPHashtable.c"
#include "TrafficMatrix.c"
#include "McFarlandProject3.h"


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
int numpackets = INTINITIALIZER;
double firstpackettimestamp = INVALIDDOUBLE;
double lastpackettimestamp = INVALIDDOUBLE;
int eth_numfullpackets = INTINITIALIZER;
int eth_numpartialpackets = INTINITIALIZER;
int numnonippackets = INTINITIALIZER;
int ip_numfullheaders = INTINITIALIZER;
int ip_numpartialheaders = INTINITIALIZER;
int ip_numtcppackets = INTINITIALIZER;
int ip_numupdpackets = INTINITIALIZER;


//prints packet trace summary
void printTraceSummary()
{
  printf("PACKETS: %d\n", numpackets);
  printf("FIRST: %.6f\n", firstpackettimestamp);
  printf("LAST: %.6f\n", lastpackettimestamp);
  printf("DURATION: %.6f\n", lastpackettimestamp-firstpackettimestamp);
}

//prints ethernet header info
void printEthernetHeaderInfo(double timestamp, char *sourceaddress, char *destinationaddress,
  unsigned short protocoltype)
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
    printf("0x%02x%02x\n", (protocoltype & PROTOCOLTYPEMSBMASK)/PROTOCOLMASKMSBDIVISOR,
      (protocoltype & PROTOCOLTYPELSBMASK));
  }
}

//prints packet information, including ips, ports, and protocol info
void printPacketInfo(double timestamp, char sourceaddress[IPADDRESSSIZE],
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
    //printf("%u %d %d\n", ihl*WORDSIZE, protocol, ttl);
  }
}

void printPacketTypes()
{
  printf("ETH: %d %d\n", eth_numfullpackets, eth_numpartialpackets);
  printf("NON-IP: %d\n", numnonippackets);
  printf("IP: %d %d\n", ip_numfullheaders, ip_numpartialheaders);
  printf("SRC: %d\n", sourceiphashtablesize);
  printf("DST: %d\n", destiphashtablesize);
  printf("TRANSPORT: %d %d %d\n", ip_numtcppackets, ip_numupdpackets,
    (ip_numfullheaders-ip_numtcppackets-ip_numupdpackets));
}

//walks and prints a formatted representation of the traffic matrix
void printTrafficMatrix()
{
  int i;
  for(i=INTINITIALIZER; i<trafficmatrixcapacity; i++)
  {
    MatrixListNode *currentnode = (trafficmatrix.tableentrylists[i].head);
    if(currentnode != NULL)
    {
      do
      {
        printIPAddress(currentnode->entry->sourceaddressvalue);
        printf(" ");
        printIPAddress(currentnode->entry->destinationaddressvalue);
        printf(" %d ", currentnode->entry->count);
        printf("%d", currentnode->entry->datavol);
        printf("\n");
      }while((currentnode = currentnode->next) != NULL);
    }
  }
}

//formats and prints a single MAC address
void printMACAddress(char macaddress[MACADDRESSSIZE])
{
  int macaddressnumberbyte = (int)(macaddress[INTINITIALIZER]);
  printf("%02x",(unsigned char)macaddressnumberbyte);
  int i;
  for(i=OFFSETVALUE; i<MACADDRESSSIZE; i++)
  {
    macaddressnumberbyte = (int)(macaddress[i]);
    printf(":%02x",(unsigned char)macaddressnumberbyte);
  }
}

//formats and prints a single IP address
void printIPAddress(char ipaddress[IPADDRESSSIZE])
{
  int ipaddressnumberbyte = (int)(ipaddress[INTINITIALIZER]);
  printf("%d",(unsigned char)ipaddressnumberbyte);
  int i;
  for(i=OFFSETVALUE; i<IPADDRESSSIZE; i++)
  {
    ipaddressnumberbyte = (int)(ipaddress[i]);
    printf(".%d",(unsigned char)ipaddressnumberbyte);
  }
}

//analyzes the packet trace as specified by the user input
void analyzePacketTrace()
{
  //read packet metadata
  while(safeRead(tracefilestream, (void *)tracepacketmetainfobuffer, sizeof(PacketMetaInfo))
    != FALSE)
  {
    if(flags[FLAG_VERBOSEOUTPUT] == TRUE)
      printf("Reading packet...\n");
    numpackets++;
    packetMetaInfoToHostOrder(tracepacketmetainfobuffer);
    if(firstpackettimestamp == INVALIDDOUBLE)
    {
      firstpackettimestamp = formatTimeStamp(
        tracepacketmetainfo.meta_secsinceepoch, tracepacketmetainfo.meta_msecsincesec);
    }

    //read ethernet header
    int truncatedethheader = analyzePacketEthernetHeader(tracepacketmetainfobuffer);

    //read ip header
    char *formattedprotocoltype = safeMalloc(sizeof(unsigned short));
    sprintf(formattedprotocoltype, "0x%02x%02x",
      (tracepacketethernetheader.eth_protocoltype & PROTOCOLTYPEMSBMASK)/PROTOCOLMASKMSBDIVISOR,
      (tracepacketethernetheader.eth_protocoltype & PROTOCOLTYPELSBMASK));
    if((testStringEquality(formattedprotocoltype, "0x0800") == FALSE) && truncatedethheader
      == FALSE)
    {
      numnonippackets++;
      if(flags[FLAG_PRINTPACKETS] == TRUE)
        printPacketInfo(formatTimeStamp(
          tracepacketmetainfo.meta_secsinceepoch, tracepacketmetainfo.meta_msecsincesec),
          "non-IP", "", INTINITIALIZER, INTINITIALIZER, INTINITIALIZER);
    }
    else
    {
      analyzePacketIPHeader(truncatedethheader);
      if(flags[FLAG_VERBOSEOUTPUT] == TRUE)
        printf("%d",tracepacketipheader.protocol);
    }
    //read any remaining bits
    safeRead(tracefilestream, safeMalloc(tracepacketmetainfo.meta_caplen),
      tracepacketmetainfo.meta_caplen);
  }

  lastpackettimestamp = formatTimeStamp(
    tracepacketmetainfo.meta_secsinceepoch, tracepacketmetainfo.meta_msecsincesec);

  if(flags[FLAG_VERBOSEOUTPUT] == TRUE)
    printf("Reached end of file.\n");

  /*if(flags[FLAG_PRINTTRACESUMMARY] == TRUE)
    printTraceSummary();*/
  /*if(flags[FLAG_PRINTPACKETTYPES] == TRUE)
    printPacketTypes();*/
  /*if(flags[FLAG_PRINTTRAFFICMATRIX] == TRUE)
    printTrafficMatrix();*/
}

//analyze a single ethernet packet header, FALSE if not truncated/successful
int analyzePacketEthernetHeader(PacketMetaInfo *tracepacketmetainfo)
{
  if(tracepacketmetainfo->meta_caplen >= sizeof(PacketEthernetHeader))
  {
    safeRead(tracefilestream, (void *)tracepacketethernetheaderbuffer,
      sizeof(PacketEthernetHeader));
    tracepacketmetainfo->meta_caplen -= sizeof(PacketEthernetHeader);
    packetEthernetHeaderToHostOrder(tracepacketethernetheaderbuffer);
    /*if(flags[FLAG_PRINTETHERNETHEADERS] == TRUE)
      printEthernetHeaderInfo(formatTimeStamp(
        tracepacketmetainfo->meta_secsinceepoch, tracepacketmetainfo->meta_msecsincesec),
        tracepacketethernetheader.eth_srcaddress, tracepacketethernetheader.eth_destaddress,
        tracepacketethernetheader.eth_protocoltype);*/
    eth_numfullpackets++;
    return FALSE;
  }
  eth_numpartialpackets++;
  /*if(flags[FLAG_PRINTETHERNETHEADERS] == TRUE)
    printEthernetHeaderInfo(formatTimeStamp(
      tracepacketmetainfo->meta_secsinceepoch, tracepacketmetainfo->meta_msecsincesec),
      "Ethernet-truncated", "", INTINITIALIZER);*/
  if(flags[FLAG_PRINTPACKETS] == TRUE)
    printPacketInfo(formatTimeStamp(
      tracepacketmetainfo->meta_secsinceepoch, tracepacketmetainfo->meta_msecsincesec),
      "unknown", "", INTINITIALIZER, INTINITIALIZER, INTINITIALIZER);
  return TRUE;
}

//analyze a single ip packet header
void analyzePacketIPHeader(int truncatedethhdr)
{
  if(tracepacketmetainfo.meta_caplen >= sizeof(struct iphdr))
  {
    safeRead(tracefilestream, (void *)tracepacketipheaderbuffer, sizeof(struct iphdr));
    iphdrToHostOrder(tracepacketipheaderbuffer);
    tracepacketmetainfo.meta_caplen -= sizeof(struct iphdr);
    if(tracepacketmetainfo.meta_caplen+sizeof(struct iphdr) < tracepacketipheader.ihl*WORDSIZE)
    {
      ip_numpartialheaders++;
      if(flags[FLAG_PRINTPACKETS] == TRUE)
        printPacketInfo(formatTimeStamp(
          tracepacketmetainfo.meta_secsinceepoch, tracepacketmetainfo.meta_msecsincesec),
          "IP-truncated", "", INTINITIALIZER, INTINITIALIZER, INTINITIALIZER);
      return;
    }
    ip_numfullheaders++;
    if(tracepacketipheader.protocol == TCPPROTOCOLNUM)
      ip_numtcppackets++;
    if(tracepacketipheader.protocol == UDPPROTOCOLNUM)
      ip_numupdpackets++;
    insertSourceIP(tracepacketipheader.saddr);
    insertDestIP(tracepacketipheader.daddr);
    char *tracepacketipheadersourceaddress = formatIPAddress(tracepacketipheader.saddr);
    char **tracepacketipheadersourceaddressbuffer = &tracepacketipheadersourceaddress;
    char *tracepacketipheaderdestaddress = formatIPAddress(tracepacketipheader.daddr);
    //assuming traffic matrix only counts values with full ip header
    char **tracepacketipheaderdestaddressbuffer = &tracepacketipheaderdestaddress;
      insertInTrafficMatrix(tracepacketipheadersourceaddressbuffer,
        tracepacketipheaderdestaddressbuffer, tracepacketipheader.tot_len);
    if(flags[FLAG_PRINTPACKETS] == TRUE)
    {
      printPacketInfo(formatTimeStamp(
        tracepacketmetainfo.meta_secsinceepoch, tracepacketmetainfo.meta_msecsincesec),
        tracepacketipheadersourceaddress, tracepacketipheaderdestaddress,
        tracepacketipheader.ihl, tracepacketipheader.protocol,
        tracepacketipheader.ttl);
    }
  }
  else
  {
    if(truncatedethhdr == FALSE)
    {
      ip_numpartialheaders++;
      if(flags[FLAG_PRINTPACKETS] == TRUE)
        printPacketInfo(formatTimeStamp(
          tracepacketmetainfo.meta_secsinceepoch, tracepacketmetainfo.meta_msecsincesec),
          "IP-truncated", "", INTINITIALIZER, INTINITIALIZER, INTINITIALIZER);
    }
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
  for(i=INTINITIALIZER; i<sizeof(string1)-OFFSETVALUE; i++)
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
  for(i=INTINITIALIZER; i<sizeof(ipaddressarray); i++)
  {
    ipaddressarray[i] = (ipaddressint >> BITSHIFTVALUE*(sizeof(ipaddressarray)-OFFSETVALUE-i))
      & PROTOCOLMASKMSBDIVISOR;
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
  trailingdecimal = trailingdecimal/TRAILINGDECIMALCONVERTER;
  return trailingdecimal;

}

//handles parsing of all user input arguments, sets flags and vars appropriately
void parseInput(int argc, char *argv[], char **tracefilename)
{
  int i;
  for(i=INTINITIALIZER; i<NUMFLAGS; i++)
    flags[i] = FALSE;
  int input;
  //read input args into variables
  while((input = getopt(argc, argv, "pstvr:")) != FINISHED)
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
    exit(EXIT_ERRORCODE);
  }

  if(flags[FLAG_VERBOSEOUTPUT] == TRUE)
    printf("input file name: %s\n",*tracefilename);
}

//sets argument flag and returns any options given
char *parseInputArg(int inputargtoparse)
{
    switch(inputargtoparse)
    {
    case 'p':
      flags[FLAG_PRINTPACKETS] = TRUE;
      break;
    case 's':
      flags[FLAG_PRINTCONNECTIONSUMMARIES] = TRUE;
      break;
    case 't':
      flags[FLAG_PRINTROUNDTRIPTIMES] = TRUE;
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
        exit(EXIT_ERRORCODEA);
      }
      else
      {
        exit(EXIT_ERRORCODEB);
      }
    }
    return NULL;
}

int safeRead(FILE *filestream, void *readbuffer, int readbuffersize)
{
  int readresult = fread(readbuffer,BYTESIZE,readbuffersize,filestream);
  if(readresult == FALSE && ferror(filestream) != FALSE)
  {
    printf("Error reading from file.\n");
    exit(EXIT_ERRORCODE);
  }
  return readresult;
}

void safeOpen(FILE **filestream, char *filename, char filemode)
{
  char *filemodeptr = &filemode;
  if((*filestream = fopen(filename,filemodeptr)) == NULL)
  {
    printf("Error: Unable to read from file.\n");
    exit(EXIT_ERRORCODE);
  }
}

void *safeMalloc (unsigned int sz)
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

int main(int argc, char *argv[])
{
  char *tracefilename = NULL;
  tracepacketmetainfo = *(PacketMetaInfo *)safeMalloc(sizeof(PacketMetaInfo));
  tracepacketethernetheader = *(PacketEthernetHeader *)safeMalloc(sizeof(PacketEthernetHeader));
  tracepacketipheader = *(struct iphdr *)safeMalloc(sizeof(struct iphdr));
  initializeTables();
  initializeTrafficMatrix();

  parseInput(argc, argv, &tracefilename);

  if(flags[FLAG_PRINTPACKETS]+flags[FLAG_PRINTCONNECTIONSUMMARIES]+
    flags[FLAG_PRINTROUNDTRIPTIMES] != SINGLEARGCHECK)
  {
    printf("Error: must specify one and only one option!\n");
    return EXIT_ERRORCODE;
  }

  safeOpen(&tracefilestream, tracefilename, 'r');

  analyzePacketTrace();

  return EXIT_NOERROR;
}
