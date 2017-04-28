#include <time.h>
#include "Server.h"
#include "ServerConnection.c"


void receivefile(char *filename, int sd)
{
  //wait to receive the file text from the client
  receivetext(filecontents, filecontents, sd);

  //write that text to a file in FILESDIRECTORY with the name filename
  //overwrite if necessary
  FILE *writingfilestream;
  char filepath[strlen(filename)+strlen(FILESDIRECTORY)];
  filepath[0] = '\0';
  strcat((char *)filepath, FILESDIRECTORY);
  strcat((char *)filepath, filename);
  filepath[strlen(filepath)-1] = '\0';

  safefileopen(&writingfilestream, (char *)filepath, 'w');
  safefilewrite(writingfilestream, (char *)filecontents, strlen(filecontents));
  safefileclose(&writingfilestream);
}

//send the list of readable files
void listfiles(unsigned int clientid)
{
  printf("%s", filelist);
  sendtext(filelist, textoutputbuffer, clientid);
  //safewrite(clientid, (void *)&filelist);
}

//read the list of files from file
void readfilelist()
{
  filelist = "";
  DIR *dirptr; //pointer to the directory
  struct dirent *direntry; //pointer to the current entry in the directory
  struct stat filestat; //the file status
  char *currentdirectory = FILESDIRECTORY;

  if((dirptr = opendir(currentdirectory)) == NULL) {
    errexit("cannot open directory: %s\n", currentdirectory);
  }
  chdir(currentdirectory);
  while((direntry = readdir(dirptr)) != NULL)
  {
    lstat(direntry->d_name,&filestat);
    if(S_ISDIR(filestat.st_mode))
    {
      // ignore . and ..
      if(strcmp(".",direntry->d_name) == 0 ||
        strcmp("..",direntry->d_name) == 0)
        continue;
    }
    else
    {
      int filelistsize = strlen(filelist);
      char *filelist2 = (char *)safemalloc(filelistsize+strlen(direntry->d_name)+1);
      if(strlen(filelist) == 0)
        strcpy(filelist2, direntry->d_name);
      else
        filelist2 = (char *)strcat(filelist, direntry->d_name);
      filelist2 = (char *)strcat(filelist2, "\n");
      filelist = (char *)safemalloc(strlen(filelist2));
      strcpy(filelist, filelist2);
    }
  }
  chdir("..");
  closedir(dirptr);
}

//record the current login in the log file
void logsessionstart(char *name, unsigned int id)
{
  char sessionlogfilepath[strlen(LOGSDIRECTORY)+strlen(SESSIONSLOGFILE)+2];
  sessionlogfilepath[0] = '\0';
  strcat((char *)sessionlogfilepath, LOGSDIRECTORY);
  strcat((char *)sessionlogfilepath, SESSIONSLOGFILE);
  //sessionlogfilepath[strlen(sessionlogfilepath)-1] = '\0';

  FILE *loginfilestream;
  safefileopen(&loginfilestream, sessionlogfilepath, 'a');
  char logmessage[MAXCOMMANDNAMESIZE+SESSIONLOGMAXLEN];
  getlogsessionstartmessage(name, id, logmessage);
  safefilewrite(loginfilestream, (char *)logmessage, strlen(logmessage));
  safefileclose(&loginfilestream);
}

//the total log message must always be <=MAXCOMMANDNAMESIZE+SESSIONLOGMAXLEN
void getlogsessionstartmessage(char *name, unsigned int id, char *logmessage)
{
  time_t currenttime;
  time(&currenttime);
  //printf("%s", ctime(&currenttime));
  strcpy(logmessage, "User: ");
  strncat(logmessage, name, strlen(name)-1);
  strcat(logmessage, " logon ");
  strcat(logmessage, ctime(&currenttime));
  //strcat(logmessage, "\n");
}

void handleconnection(int sd)
{
  /* ask for login information and wait for response*/
  awaitresponse(sd, (void *)&inputbuffer);
  CommandMessage message_generate_id = inputbuffer;
  if(message_generate_id.command_id != CMDID_GENERATECLIENTID)
    errexit("error generating user id", NULL);
  generateclient_id((char **)&(message_generate_id.client_name));
  logsessionstart(message_generate_id.client_name, currentclient_id);
  printf("Server recognizes user: %s\n", message_generate_id.client_name);

  /* continue accepting responses until the quit command is received */
  while(1 == 1)
  {
    memset((void *)&inputbuffer, FALSE, sizeof(CommandMessage));
    awaitresponse(sd, (void *)&inputbuffer);
    CommandMessage commandmessage = inputbuffer;

    if(commandmessage.command_id == CMDID_LISTFILES)
      listfiles(sd2);
    else if(commandmessage.command_id == CMDID_QUIT)
      return;
    else if(commandmessage.command_id == CMDID_READFILE)
      sendfile(commandmessage.target_name, textoutputbuffer, filecontents,
        sd2, FILESDIRECTORY);
    else if(commandmessage.command_id == CMDID_WRITEFILE)
      receivefile(commandmessage.target_name, sd2);
    else if(commandmessage.command_id == CMDID_READSESSIONLOG)
      sendfile(SESSIONSLOGFILE, textoutputbuffer, filecontents,
        sd2, LOGSDIRECTORY);
  }
}

int main (int argc, char *argv [])
{

  /* initialize the server */
  init(argc, argv);
  readfilelist();

  /* listen for incoming connections */
  if (listen (sd, QLEN) < 0)
    errexit ("cannot listen on port %s\n", argv [SERVER_PORT_POS]);

  /* handle connections serially */
  //could change later to do a thread for each connection
  while(1 == 1)
  {
    sd2 = accept (sd,&addr,&addrlen);
    if (sd2 < 0)
      errexit ("error accepting connection", NULL);
    handleconnection(sd2);
    close (sd2);
  }

  /* close connection sd and exit */
  close (sd);
  exit (0);
}
