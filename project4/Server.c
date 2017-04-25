#include "Server.h"
#include "ServerConnection.c"

char *filelist; //string holding the list of readable files
char *filecontents; //string holding the contents of a file


//send the contents of one file
void sendfile(char *filename)
{
  FILE *sendingfilestream;
  safefileopen(&sendingfilestream, filename, 'r');
  safefileread(sendingfilestream, (void *)filecontents, MAXFILEREADSIZE);
  sendtext(filecontents, textoutputbuffer, currentclient_id);
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
  char *currentdirectory = "files";

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
      sendfile(commandmessage.target_name);
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
