/*
  Brennan McFarland
  bfm21
  McFarlandProject1.h
  02/04/2017
  The header for McFarlandProject1.c
*/
#define TRUE 1
#define FALSE 0
#define INDEXINITIALIZER 0
#define SKIPINDEXINITIALIZER 1
#define TALLYINITIALIZER 0
#define RETURNSUCCESS 0
#define RETURNFAILURE 1
#define RETURNERROR 255
#define ERRORCODE2 -2
#define ERRORCODE3 -3
#define ERRORCODE4 -4
#define ERRORCODE5 -5
#define NUMFLAGS 4
#define FLAGSALLOCSIZE NUMFLAGS*sizeof(int)
#define IPADDRESSSIZE 4
#define IPADDRESSPREFIXSIZE 3
#define BYTESIZE 1
#define IPADDRESSDECIMALBUFFERSIZE 3

#define FLAGA 0
#define FLAGB 1
#define FLAGC 2
#define FLAGD 3

typedef struct OrgPrefix
{
  char ipprefix[IPADDRESSPREFIXSIZE];
  char *orgname;
} OrgPrefix;
void dumpAddress(char *ipaddress);
void dumpPrefix(char *ipaddress);
void dumpOrg(char *ipaddress, struct OrgPrefix orgprefixlist[], int listsize);
int getFileLines(char *filename, int *maxlinelength);
void compileOrgList(struct OrgPrefix orgprefixlist[], char *orgprefixfilename, int orglistfilenumlines, int maxlinelengthvalue);
char *parseInput(int input, int flags[]);
int readbyte(int file, char *readbyte);
void *safemalloc (unsigned int sz);
