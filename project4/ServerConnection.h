
#define SERVER_REQUIRED_ARGC 2
#define SERVER_PORT_POS 1
#define SERVER_MSG_POS 2
#define ERROR 1
#define QLEN 1
#define SERVER_BUFLEN 1024
#define SERVER_MAXCCONNECTIONS 1024

unsigned int generateclient_id(char **client_name);
int usage (char *progname);
int errexit (char *format, char *arg);
void init(int argc, char *argv []);
