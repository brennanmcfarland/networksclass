#define FILESDIRECTORY "serverfiles/"
#define LOGSDIRECTORY "serverlogs/"
#define SESSIONSLOGFILE "sessions.log\n"
#define SESSIONLOGMAXLEN 64
#define TIMESTRINGLENGTH 24 //the string length returned by ctime

void receivefile(char *filename, int sd);
void listfiles(unsigned int clientid);
void readfilelist();
void logsessionstart(char *name, unsigned int id);
void sendtime(unsigned int clientid);
void getlogsessionstartmessage(char *name, unsigned int id, char *logmessage);
void gettime();
void handleconnection(int sd);
void init(int argc, char *argv []);
