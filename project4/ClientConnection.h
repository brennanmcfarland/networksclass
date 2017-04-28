
#define ERROR 1
#define CLIENT_REQUIRED_ARGC 3
#define CLIENT_HOST_POS 1
#define CLIENT_PORT_POS 2
#define CLIENT_BUFLEN 1024
#define SCANBUFFERINITSIZE 1024

void sendcommandmessage(unsigned int command_id, unsigned int target_id, char *target_name);
unsigned int generateclient_id(char *client_name);
int safescanf(char *buffer);
int usage (char *progname);
int errexit (char *format, char *arg);
void init(int argc, char *argv []);
