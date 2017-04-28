/*
  Brennan McFarland
  bfm21
  Server.h
  4/28/17
  the header for proj5d.c
  handles the server backend, managing the server files and responding to commands
    -handles connections from the client serially
    -interprets + sends the appropriate response to commands
    -retrieves the list of server files + records sessions and the time
*/

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
void printcommunication(unsigned int commandid, int isoutbound, char *senttext);
void handleconnection(int sd);
void init(int argc, char *argv []);
