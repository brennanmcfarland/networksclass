/*
  Brennan McFarland
  bfm21
  McFarlandProject2.h
  3/3/17
  The header for McFarlandProject2.c
*/
#define TRUE 1
#define FALSE 0

#define NUMFLAGS 7
#define FLAG_TRACEFILENAME 0
#define FLAG_PRINTPACKETS 1
#define FLAG_PRINTCONNECTIONSUMMARIES 2
#define FLAG_PRINTROUNDTRIPTIMES 3
#define FLAG_VERBOSEOUTPUT 4

#define MACADDRESSSIZE 6
#define IPADDRESSSIZE 4
#define FLAGSALLOCSIZE NUMFLAGS*sizeof(int)
#define BYTESIZE 1
#define WORDSIZE 4

#define PROTOCOLTYPEMSBMASK 0xff00
#define PROTOCOLTYPELSBMASK 0x00ff
#define PROTOCOLMASKMSBDIVISOR 0xff

#define TCPPROTOCOLNUM 6
#define UDPPROTOCOLNUM 17

#define TRAILINGDECIMALCONVERTER 1000000

#define FINISHED -1
#define EXIT_ERRORCODEA 2
#define EXIT_ERRORCODEB 3
#define SINGLEARGCHECK 1

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

/*
struct tcphdr
  {
    __extension__ union
    {
      struct
      {
        u_int16_t th_sport;             // source port
        u_int16_t th_dport;             // destination port
        tcp_seq th_seq;         // sequence number
        tcp_seq th_ack;         // acknowledgement number
# if __BYTE_ORDER == __LITTLE_ENDIAN
        u_int8_t th_x2:4;               // (unused)
        u_int8_t th_off:4;              // data offset
# endif
# if __BYTE_ORDER == __BIG_ENDIAN
        u_int8_t th_off:4;              // data offset
        u_int8_t th_x2:4;               // (unused)
# endif
        u_int8_t th_flags;
# define TH_FIN 0x01
# define TH_SYN 0x02
# define TH_RST 0x04
# define TH_PUSH        0x08
# define TH_ACK 0x10
# define TH_URG 0x20
        u_int16_t th_win;               // window
        u_int16_t th_sum;               // checksum
        u_int16_t th_urp;               // urgent pointer
      };
      struct
      {
        u_int16_t source;
        u_int16_t dest;
        u_int32_t seq;
        u_int32_t ack_seq;
# if __BYTE_ORDER == __LITTLE_ENDIAN
        u_int16_t res1:4;
        u_int16_t doff:4;
        u_int16_t fin:1;
        u_int16_t syn:1;
        u_int16_t rst:1;
        u_int16_t psh:1;
        u_int16_t ack:1;
        u_int16_t urg:1;
        u_int16_t res2:2;
# elif __BYTE_ORDER == __BIG_ENDIAN
        u_int16_t doff:4;
       u_int16_t res1:4;
        u_int16_t res2:2;
        u_int16_t urg:1;
        u_int16_t ack:1;
        u_int16_t psh:1;
        u_int16_t rst:1;
        u_int16_t syn:1;
        u_int16_t fin:1;
# else
#  error "Adjust your <bits/endian.h> defines"
# endif
        u_int16_t window;
        u_int16_t check;
        u_int16_t urg_ptr;
      };
    };
};
*/

/*
struct udphdr
{
  __extension__ union
  {
    struct
    {
      u_int16_t uh_sport;               // source port
      u_int16_t uh_dport;               // destination port
      u_int16_t uh_ulen;                // udp length
      u_int16_t uh_sum;         // udp checksum
    };
    struct
    {
      u_int16_t source;
      u_int16_t dest;
      u_int16_t len;
      u_int16_t check;
    };
  };
};

*/

void printTraceSummary();
void printEthernetHeaderInfo(double timestamp, char *sourceaddress, char *destinationaddress,
  unsigned short protocoltype);
void printPacketInfo(double timestamp, char sourceaddress[IPADDRESSSIZE],
  char destinationaddress[IPADDRESSSIZE], unsigned int ihl, u_int8_t protocol, u_int8_t ttl);
void printPacketTypes();
void printTrafficMatrix();
void printMACAddress(char *macaddress);
void printIPAddress(char *ipaddress);
void analyzePacketTrace();
int analyzePacketEthernetHeader(PacketMetaInfo *tracepacketmetainfo);
int analyzePacketIPHeader(int truncatedethhdr);
void analyzeTransportHeader();
void analyzePacketTCPHeader();
void analyzePacketUDPHeader();
void packetMetaInfoToHostOrder(PacketMetaInfo * packetmetainfo);
void packetEthernetHeaderToHostOrder(PacketEthernetHeader * packetethernetheader);
void iphdrToHostOrder(struct iphdr * packetipheader);
void tcphdrToHostOrder(struct tcphdr *packettcpheader);
int testStringEquality(char *string1, char *string2);
char *formatIPAddress(u_int32_t ipaddressint);
double formatTimeStamp(int secsinceepoch, int msecsinsec);
double formatAsTrailingDecimal(int integerdigits);
void parseInput(int argc, char *argv[], char **tracefilename);
char *parseInputArg(int inputargtoparse);
int safeRead(FILE *filestream, void *readbuffer, int readbuffersize);
void safeOpen(FILE **filestream, char *filename, char filemode);
void *safeMalloc (unsigned int sz);
