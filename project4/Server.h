#define FILESDIRECTORY "files/"

void sendfile(char *filename, unsigned int clientid);
void listfiles(unsigned int clientid);
void readfilelist();
void handleconnection(int sd);
void init(int argc, char *argv []);
