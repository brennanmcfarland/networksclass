#define TRUE 1
#define FALSE 0
#define NUMFLAGS 4
#define FLAGSALLOCSIZE NUMFLAGS*sizeof(int)
#define IPADDRESSSIZE 4
#define IPADDRESSPREFIXSIZE 3
#define BYTESIZE 1
#define IPADDRESSDECIMALBUFFERSIZE 3

typedef struct OrgPrefix
{
  char ipprefix[IPADDRESSPREFIXSIZE];
  char *orgname;
} OrgPrefix;
void dumpAddress(char *ipaddress);
void dumpPrefix(char *ipaddress);
void dumpOrg(char *ipaddress, struct OrgPrefix orgprefixlist[], int listsize);
int getFileLines(char *filename, int *maxlinelength);
void compileOrgList(struct OrgPrefix orgprefixlist[], char *orgprefixfilename, int orglistfilenumlines, int *maxlinelength);
char *parseInput(int input, int flags[]);
int readbyte(int file, char *readbyte);
void *safemalloc (unsigned int sz);
