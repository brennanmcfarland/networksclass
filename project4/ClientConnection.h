
#define ERROR 1
#define CLIENT_REQUIRED_ARGC 3
#define CLIENT_HOST_POS 1
#define CLIENT_PORT_POS 2
#define CLIENT_BUFLEN 1024

int usage (char *progname);
int errexit (char *format, char *arg);
void init(int argc, char *argv []);
