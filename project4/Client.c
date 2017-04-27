
#include "Client.h"
#include "ClientConnection.c"

/*
  TODO: ensure clients are handled serially
  ensure the client exits gracefully if the connection is lost/times out
*/

void promptcommandlist()
{
  promptuser(STRING_COMMANDLIST);
}

void promptuser(char *inputmessage)
{
  printf("%s\n", inputmessage);
  safescanf(userinputbuffer);
  /*
  if(strlen(userinputbuffer) >= SCANBUFFERINITSIZE-1)
  {
    printf("Input size out of range.  You crashed it, genius");
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
  }
  printf("USER INPUT BUFFER CONTAINS: %s\n\n", userinputbuffer);*/
}

void parseuserinput()
{
  if(strncmp(userinputbuffer, "quit", sizeof("quit")-1) == 0)
  {
    sendcommandmessage(CMDID_QUIT, FALSE, FALSE);
    printf("Goodbye\n");
    close(sd);
    exit(0);
  }
  else if(strncmp(userinputbuffer, "list files", sizeof("list files")-1) == 0)
  {
    sendcommandmessage(CMDID_LISTFILES, FALSE, FALSE);
    useroutputbuffer = receivetext(textinputbuffer, useroutputbuffer, sd);
    printf("\n%s", useroutputbuffer);
  }
  else if(strncmp(userinputbuffer, "read ", sizeof("read ")-1) == 0)
  {
    sendcommandmessage(CMDID_READFILE, FALSE, userinputbuffer+sizeof("read"));
    useroutputbuffer = receivetext(textinputbuffer, useroutputbuffer, sd);
    printf("\n%s", useroutputbuffer);
  }
  //TODO: put this in a separate function
  else if(strncmp(userinputbuffer, "write ", sizeof("write ")-1) == 0)
  {
    char *filename = userinputbuffer+sizeof("write");
    //notify the server that it is about to receive a file
    sendcommandmessage(CMDID_WRITEFILE, FALSE, filename);
    //read the contents of the file into filecontents
    FILE *sendingfilestream;
    char filepath[strlen(filename)+strlen(FILESDIRECTORY)];
    filepath[0] = '\0';
    strcat((char *)filepath, FILESDIRECTORY);
    strcat((char *)filepath, filename);
    filepath[strlen(filepath)-1] = '\0';

    safefileopen(&sendingfilestream, filepath, 'r');
    safefileread(sendingfilestream, filecontents, MAXFILEREADSIZE);
    //and send it over the network
    sendfile(filename, textoutputbuffer, filecontents,
      sd, FILESDIRECTORY);
  }
  else
  {
    printf("Invalid input.  Please try again: ");
    //*userinputbuffer = "";
    promptcommandlist();
    parseuserinput();
  }
}

int main (int argc, char *argv [])
{
  /* initialize client connection */
  init(argc, argv);


  /* print the welcome message and send whatever the user types as to get id*/
  promptuser("Welcome.  Please input a username:");
  client_id = generateclient_id(userinputbuffer);
  client_name = userinputbuffer;
  printf("Client recognizes user: %s\n", userinputbuffer);
  printf(STRING_WELCOMEMESSAGE);

  /* main loop */
  while(1 == 1)
  {
    promptcommandlist();
    parseuserinput();
  }

  /* close & exit */
  close (sd);
  exit (0);
}
