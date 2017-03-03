/*
  Brennan McFarland
  bfm21
  McFarlandProject2.h
  [DATE]
  The header for McFarlandProject2.c
*/
#define TRUE 1
#define FALSE 0

#define NUMFLAGS 7
#define FLAG_TRACEFILENAME 0
#define FLAG_PRINTTRACESUMMARY 1
#define FLAG_PRINTETHERNETHEADERS 2
#define FLAG_PRINTIPHEADERS 3
#define FLAG_PRINTPACKETTYPES 4
#define FLAG_PRINTTRAFFICMATRIX 5
#define FLAG_VERBOSEOUTPUT 6

#define MACADDRESSSIZE 6
#define IPADDRESSSIZE 4
#define FLAGSALLOCSIZE NUMFLAGS*sizeof(int)
#define BYTESIZE 1
#define WORDSIZE 4

typedef struct PacketMetaInfo
{
  unsigned int meta_secsinceepoch;
  unsigned int meta_msecsincesec;
  unsigned short meta_caplen;
  unsigned short meta_ignored;
} PacketMetaInfo;

typedef struct PacketEthernetHeader
{
  char eth_destaddress[MACADDRESSSIZE];
  char eth_srcaddress[MACADDRESSSIZE];
  unsigned short eth_protocoltype;
} PacketEthernetHeader;

void printTraceSummary();
void printEthernetHeaderInfo(double timestamp, char *sourceaddress, char *destinationaddress, unsigned short protocoltype);
void printIPHeaderInfo(double timestamp, char sourceaddress[IPADDRESSSIZE],
  char destinationaddress[IPADDRESSSIZE], unsigned int ihl, u_int8_t protocol, u_int8_t ttl);
void printPacketTypes();
void printTrafficMatrix();
void printMACAddress(char *macaddress);
void printIPAddress(char *ipaddress);
void analyzePacketTrace();
int analyzePacketEthernetHeader(PacketMetaInfo *tracepacketmetainfo);
void analyzePacketIPHeader(int truncatedethhdr);
void packetMetaInfoToHostOrder(PacketMetaInfo * packetmetainfo);
void packetEthernetHeaderToHostOrder(PacketEthernetHeader * packetethernetheader);
void iphdrToHostOrder(struct iphdr * packetipheader);
int testStringEquality(char *string1, char *string2);
char *formatIPAddress(u_int32_t ipaddressint);
double formatTimeStamp(int secsinceepoch, int msecsinsec);
double formatAsTrailingDecimal(int integerdigits);
void parseInput(int argc, char *argv[], char **tracefilename);
char *parseInputArg(int inputargtoparse);
int safeRead(FILE *filestream, void *readbuffer, int readbuffersize);
void safeOpen(FILE **filestream, char *filename, char filemode);
void *safeMalloc (unsigned int sz);
