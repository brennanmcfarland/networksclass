#define MAXFILEREADSIZE 4096

void listfiles(unsigned int clientid);
void readfilelist();
void handleconnection(int sd);
void init(int argc, char *argv []);
