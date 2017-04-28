/*
  Brennan McFarland
  bfm21
  proj5.c
  4/28/17
  handles the upper level of how the user interacts with the client side
    -accepts input from and displays output to the user, calls commands handled
      in ClientConnection.c
*/

#include "Client.h"
#include "ClientConnection.c"


void promptcommandlist()
{
  promptuser(STRING_COMMANDLIST);
}

void promptuser(char *inputmessage)
{
  printf("%s\n", inputmessage);
  safescanf(userinputbuffer);
}

void sendreceiveprintif(unsigned int commandid, unsigned int targetid,
  char *targetname)
{
  sendcommandmessage(commandid, targetid, targetname);
  useroutputbuffer = receivetext(textinputbuffer, useroutputbuffer, sd);
  printf("\n%s", useroutputbuffer);
}

void parseuserinput()
{
  if(strncmp(userinputbuffer, "quit", sizeof("quit")-OFFSETVALUE) == EMPTYSTRING)
  {
    sendcommandmessage(CMDID_QUIT, FALSE, FALSE);
    printf("Goodbye\n");
    close(sd);
    exit(EXIT_NOERROR);
  }
  else if(strncmp(userinputbuffer, "list files",
    sizeof("list files")-OFFSETVALUE) == EMPTYSTRING)
    sendreceiveprintif(CMDID_LISTFILES, FALSE, FALSE);
  else if(strncmp(userinputbuffer, "read ", sizeof("read ")-OFFSETVALUE)
    == EMPTYSTRING)
    sendreceiveprintif(CMDID_READFILE, FALSE, userinputbuffer+sizeof("read"));
  else if(strncmp(userinputbuffer, "session log",
    sizeof("session log")-OFFSETVALUE) == EMPTYSTRING)
    sendreceiveprintif(CMDID_READSESSIONLOG, FALSE, FALSE);
  else if(strncmp(userinputbuffer, "time", sizeof("time")-OFFSETVALUE)
    == EMPTYSTRING)
    sendreceiveprintif(CMDID_GETTIME, FALSE, FALSE);
  else
  {
    printf("Invalid input.  Please try again: ");
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
  while(TRUE == TRUE)
  {
    promptcommandlist();
    parseuserinput();
  }

  /* close & exit -- this should never be reached in the code's current state*/
  close (sd);
  exit (EXIT_NOERROR);
}
