#define TRUE 1
#define FALSE 0
#define NUMFLAGS 4
#define FLAGSALLOCSIZE NUMFLAGS*sizeof(int)
#define IPADDRESSSIZE 4
#define IPADDRESSPREFIXSIZE 3
#define BYTESIZE 1

struct OrgPrefix;
void dumpAddress(char *ipaddress);
void dumpPrefix(char *ipaddress);
void compileOrgList(struct OrgPrefix *orgprefixlist, char *orgprefixfilename);
char *parseInput(int input, int flags[]);
int readbyte(int file, char *readbyte);
void *safemalloc (unsigned int sz);
