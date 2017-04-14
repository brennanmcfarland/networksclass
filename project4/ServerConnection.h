
#define SERVER_REQUIRED_ARGC 3
#define SERVER_PORT_POS 1
#define SERVER_MSG_POS 2
#define ERROR 1
#define QLEN 1
#define SERVER_BUFLEN 1024

unsigned int generateclient_id(char **client_name);
int usage (char *progname);
int errexit (char *format, char *arg);
int saferead(int filedes, void *readbuffer);
void safewrite(int filedes, void *writebuffer);
int safefileread(FILE *filestream, void *readbuffer, int readbuffersize);
void safefileopen(FILE **filestream, char *filename, char filemode);
void *safemalloc (unsigned int sz);
void init(int argc, char *argv []);
