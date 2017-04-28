/*
  Brennan McFarland
  bfm21
  proj5d.c
  4/28/17
  handles the server backend, managing the server files and responding to commands
    -handles connections from the client serially
    -interprets + sends the appropriate response to commands
    -retrieves the list of server files + records sessions and the time
*/

#include <time.h>
#include "Server.h"
#include "ServerConnection.c"

char currenttime[TIMESTRINGLENGTH];


void receivefile(char *filename, int sd)
{
  //wait to receive the file text from the client
  receivetext(filecontents, filecontents, sd);

  //write that text to a file in FILESDIRECTORY with the name filename
  //overwrite if necessary
  FILE *writingfilestream;
  char filepath[strlen(filename)+strlen(FILESDIRECTORY)];
  filepath[INTINITIALIZER] = '\0';
  strcat((char *)filepath, FILESDIRECTORY);
  strcat((char *)filepath, filename);
  filepath[strlen(filepath)-OFFSETVALUE] = '\0';

  safefileopen(&writingfilestream, (char *)filepath, 'w');
  safefilewrite(writingfilestream, (char *)filecontents, strlen(filecontents));
  safefileclose(&writingfilestream);
}

//send the list of readable files
void listfiles(unsigned int clientid)
{
  printcommunication(CMDID_LISTFILES, TRUE, filelist);
  sendtext(filelist, textoutputbuffer, clientid);
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
      if(strcmp(".",direntry->d_name) == EMPTYSTRING ||
        strcmp("..",direntry->d_name) == EMPTYSTRING)
        continue;
    }
    else
    {
      int filelistsize = strlen(filelist); //concat file entries back + forth
      char *filelistb =
        (char *)safemalloc(filelistsize+strlen(direntry->d_name)+OFFSETVALUE);
      if(strlen(filelist) == EMPTYSTRING)
        strcpy(filelistb, direntry->d_name);
      else
        filelistb = (char *)strcat(filelist, direntry->d_name);
      filelistb = (char *)strcat(filelistb, "\n");
      filelist = (char *)safemalloc(strlen(filelistb));
      strcpy(filelist, filelistb);
    }
  }
  chdir("..");
  closedir(dirptr);
}

//record the current login in the log file
void logsessionstart(char *name, unsigned int id)
{
  printf("logging session start...%s\n", name);
  char sessionlogfilepath[strlen(LOGSDIRECTORY)
    +strlen(SESSIONSLOGFILE)+OFFSETVALUE+OFFSETVALUE];
  sessionlogfilepath[INTINITIALIZER] = '\0';
  strcat((char *)sessionlogfilepath, LOGSDIRECTORY);
  strncat((char *)sessionlogfilepath, SESSIONSLOGFILE,
    strlen(SESSIONSLOGFILE)-OFFSETVALUE);

  FILE *loginfilestream;
  safefileopen(&loginfilestream, sessionlogfilepath, 'a');
  char logmessage[MAXCOMMANDNAMESIZE+SESSIONLOGMAXLEN];
  getlogsessionstartmessage(name, id, logmessage);
  safefilewrite(loginfilestream, (char *)logmessage, strlen(logmessage));
  safefileclose(&loginfilestream);
}

void sendtime(unsigned int clientid)
{
  gettime();
  printcommunication(CMDID_GETTIME, TRUE, currenttime);
  sendtext(currenttime, textoutputbuffer, clientid);
}

//the total log message must always be <=MAXCOMMANDNAMESIZE+SESSIONLOGMAXLEN
void getlogsessionstartmessage(char *name, unsigned int id, char *logmessage)
{
  strcpy(logmessage, "User: ");
  strncat(logmessage, name, strlen(name)-OFFSETVALUE);
  strcat(logmessage, " logon ");
  gettime();
  strcat(logmessage, currenttime);
}

//read the current time into currenttime
void gettime()
{
  time_t unformattedtime;
  time(&unformattedtime);
  strcpy(currenttime, ctime(&unformattedtime));
}

void printcommunication(unsigned int commandid, int isoutbound, char *senttext)
{
  if(isoutbound == TRUE)
    printf("S -> C: ");
  else
    printf("C -> S: ");
  printf("%s %s\n", getcommand_name(commandid), senttext);
}

void handleconnection(int sd)
{
  /* ask for login information and wait for response*/
  awaitresponse(sd, (void *)&inputbuffer);
  CommandMessage message_generate_id = inputbuffer;
  printcommunication(message_generate_id.command_id, FALSE,
    message_generate_id.client_name);
  if(message_generate_id.command_id != CMDID_GENERATECLIENTID)
    errexit("error generating user id", NULL);
  generateclient_id((char **)&(message_generate_id.client_name));
  logsessionstart(message_generate_id.client_name, currentclient_id);

  /* continue accepting responses until the quit command is received */
  while(TRUE == TRUE)
  {
    memset((void *)&inputbuffer, INTINITIALIZER, sizeof(CommandMessage));
    awaitresponse(sd, (void *)&inputbuffer);
    CommandMessage commandmessage = inputbuffer;
    printcommunication(commandmessage.command_id, FALSE, commandmessage.target_name);

    if(commandmessage.command_id == CMDID_LISTFILES)
      listfiles(sd2);
    else if(commandmessage.command_id == CMDID_QUIT)
      return;
    else if(commandmessage.command_id == CMDID_READFILE)
      sendfile(commandmessage.target_name, textoutputbuffer, filecontents,
        sd2, FILESDIRECTORY, commandmessage.command_id);
    else if(commandmessage.command_id == CMDID_WRITEFILE)
      receivefile(commandmessage.target_name, sd2);
    else if(commandmessage.command_id == CMDID_READSESSIONLOG)
      sendfile(SESSIONSLOGFILE, textoutputbuffer, filecontents,
        sd2, LOGSDIRECTORY, commandmessage.command_id);
    else if(commandmessage.command_id == CMDID_GETTIME)
      sendtime(sd2);
  }
}

int main (int argc, char *argv [])
{

  /* initialize the server */
  init(argc, argv);
  readfilelist();

  /* listen for incoming connections */
  if (listen (sd, QLEN) < EXIT_NOERROR)
    errexit ("cannot listen on port %s\n", argv [SERVER_PORT_POS]);

  /* handle connections serially */
  //could change later to do a thread for each connection
  while(TRUE == TRUE)
  {
    sd2 = accept (sd,&addr,&addrlen);
    if (sd2 < EXIT_NOERROR)
      errexit ("error accepting connection", NULL);
    handleconnection(sd2);
    close (sd2);
  }

  /* close connection sd and exit */
  close (sd);
  exit (EXIT_NOERROR);
}
