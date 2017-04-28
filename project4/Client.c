
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
  if(strncmp(userinputbuffer, "quit", sizeof("quit")-1) == 0)
  {
    sendcommandmessage(CMDID_QUIT, FALSE, FALSE);
    printf("Goodbye\n");
    close(sd);
    exit(0);
  }
  else if(strncmp(userinputbuffer, "list files", sizeof("list files")-1) == 0)
    sendreceiveprintif(CMDID_LISTFILES, FALSE, FALSE);
  else if(strncmp(userinputbuffer, "read ", sizeof("read ")-1) == 0)
    sendreceiveprintif(CMDID_READFILE, FALSE, userinputbuffer+sizeof("read"));
  else if(strncmp(userinputbuffer, "session log", sizeof("session log")-1) == 0)
    sendreceiveprintif(CMDID_READSESSIONLOG, FALSE, FALSE);
  else if(strncmp(userinputbuffer, "time", sizeof("time")-1) == 0)
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
  while(1 == 1)
  {
    promptcommandlist();
    parseuserinput();
  }

  /* close & exit -- this should never be reached in the code's current state*/
  close (sd);
  exit (0);
}
