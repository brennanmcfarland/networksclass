#define FILESDIRECTORY "serverfiles/"

void receivefile(char *filename, int sd);
void listfiles(unsigned int clientid);
void readfilelist();
void handleconnection(int sd);
void init(int argc, char *argv []);
