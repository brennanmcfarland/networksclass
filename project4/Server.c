#include "Server.h"
#include "ServerConnection.c"

char *filelist; //string holding the list of readable files
char filecontents[MAXFILEREADSIZE]; //string holding the contents of a file


//send the contents of one file
void sendfile(char *filename, unsigned int clientid)
{
  FILE *sendingfilestream;
  char filepath[strlen(filename)+strlen(FILESDIRECTORY)];
  filepath[0] = '\0';
  strcat((char *)filepath, FILESDIRECTORY);
  strcat((char *)filepath, filename);
  filepath[strlen(filepath)-1] = '\0';

  /*
    TODO: get the below commented out code to actually work so it checks if the
    file is there
  */
  //check if the file exists and the user can open it
  //if(access((char *)filepath+1, F_OK) == -1)
  //{
  //  strcpy(filecontents, "Error: The file either does not exist or cannot be accessed");
  //  sendtext(filecontents, textoutputbuffer, clientid);
  //}
  //else
  //{
    safefileopen(&sendingfilestream, (char *)filepath, 'r');
    safefileread(sendingfilestream, (void *)filecontents, MAXFILEREADSIZE);
    sendtext(filecontents, textoutputbuffer, clientid);
    safefileclose(&sendingfilestream);
  //}
  memset(filecontents, FALSE, strlen(filecontents));
  memset(textoutputbuffer, FALSE, strlen(textoutputbuffer));
}

void receivefile(char *filename, int sd)
{
  //wait to receive the file text from the client
  filecontents = receivetext(filecontents, filecontents, sd);

  //write that text to a file in FILESDIRECTORY with the name filename
  //overwrite if necessary
  FILE *writingfilestream;

  safefileopen(&writingfilestream, (char *)filepath, 'w');
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

void handleconnection(int sd)
{
  /* ask for login information and wait for response*/
  awaitresponse(sd, (void *)&inputbuffer);
  CommandMessage message_generate_id = inputbuffer;
  if(message_generate_id.command_id != CMDID_GENERATECLIENTID)
    errexit("error generating user id", NULL);
  generateclient_id((char **)&(message_generate_id.client_name));
  printf("Server recognizes user: %s\n", message_generate_id.client_name);

  /* continue accepting responses until the quit command is received */
  while(1 == 1)
  {
    memset((void *)&inputbuffer, FALSE, sizeof(CommandMessage));
    awaitresponse(sd, (void *)&inputbuffer);
    CommandMessage commandmessage = inputbuffer;

    if(commandmessage.command_id == CMDID_LISTFILES)
      listfiles(sd2);
    if(commandmessage.command_id == CMDID_QUIT)
      return;
    if(commandmessage.command_id == CMDID_READFILE)
      sendfile(commandmessage.target_name, sd2);
    if(commandmessage.command_id == CMDID_WRITEFILE)
      receivefile(commandmessage.target_name, sd2);
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
