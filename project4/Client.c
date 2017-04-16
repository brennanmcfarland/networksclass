
#include "Client.h"
#include "ClientConnection.c"

void promptcommandlist()
{
  promptuser(STRING_COMMANDLIST);
}

void promptuser(char *inputmessage)
{
  printf("%s\n", inputmessage);
  safescanf((char **)&userinputbuffer);
  while(strlen(userinputbuffer) > MAXCOMMANDNAMESIZE)
  {
    printf("Input size out of range.  Please try again: ");
    userinputbuffer = "";
    safescanf((char **)&userinputbuffer);
  }
}

void parseuserinput()
{
  if(strcmp(userinputbuffer, "quit") == 0)
  {
    sendcommandmessage(CMDID_QUIT, FALSE);
    printf("Goodbye\n");
    exit(0);
  }
  else if(strcmp(userinputbuffer, "list files") == 0)
  {
    sendcommandmessage(CMDID_LISTFILES, FALSE);
    waitforservertext(sd);
    printf("finished getting text from server\n");
    //waitforserverresponse(sd, textinputbuffer); //probably wont work, buffer not inited
  }
  else
  {
    printf("Invalid input.  Please try again: ");
    userinputbuffer = "";
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
  client_id = generateclient_id((char **)&userinputbuffer);
  client_name = userinputbuffer;
  printf("Client recognizes user: %s\n", userinputbuffer);
  printf(STRING_WELCOMEMESSAGE);

  //TODO: add an exit condition
  /* main loop */
  while(1 == 1)
  {
    promptcommandlist();
    parseuserinput();
    waitforserverresponse(sd, (void *)&inputbuffer);
    /* capture and print whatever the server provides */
    saferead(sd, (void *)&inputbuffer);
  }

  /* close & exit */
  close (sd);
  exit (0);
}
