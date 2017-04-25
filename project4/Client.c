
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
  while(strlen(userinputbuffer) > MAXCOMMANDNAMESIZE)
  {
    printf("Input size out of range.  Please try again: ");
    //userinputbuffer = "";
    safescanf(userinputbuffer);
  }
}

void parseuserinput()
{
  if(strncmp(userinputbuffer, "quit", sizeof("quit")-1) == 0)
  {
    sendcommandmessage(CMDID_QUIT, FALSE);
    printf("Goodbye\n");
    close(sd);
    exit(0);
  }
  else if(strncmp(userinputbuffer, "list files", sizeof("list files")-1) == 0)
  {
    sendcommandmessage(CMDID_LISTFILES, FALSE);
    receivetext(textinputbuffer, useroutputbuffer, sd);
    printf("\n%s\n", useroutputbuffer);
    //waitforservertext(sd);
    //awaitresponse(sd, textbuffer);
    //void receivetext(sd, textinputbuffer, unsigned int target_id);
    printf("%s", (char *)useroutputbuffer);
    printf("finished getting text from server\n");
    //awaitresponse(sd, textinputbuffer); //probably wont work, buffer not inited
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

  //TODO: add an exit condition
  /* main loop */
  while(1 == 1)
  {
    promptcommandlist();
    parseuserinput();
    //awaitresponse(sd, (void *)&inputbuffer);
    /* capture and print whatever the server provides */
    //saferead(sd, (void *)&inputbuffer);
    /*
      NEED TO ACTUALLY PRINT THE OUTPUT SO THE USER CAN SEE IT
      then fix problem with segfault when trying to read whole thing
    */

  }

  /* close & exit */
  close (sd);
  exit (0);
}
