#define FILESDIRECTORY "clientfiles/"
#define INPUT_QUIT "quit"

#define STRING_WELCOMEMESSAGE "\n\n"\
  "Once, there was a time before the internet, a time in which the forces of "\
  "ignorance and darkness roamed freely about the human race.  It was a world "\
  "of dialtones and modems, of serial network connections and abysmal data "\
  "transfer rates, measured not in gigabytes, not in megabytes, but in "\
  "kilobytes, if that.  To combat the omnipotent forces of evil and spare "\
  "humanity from an otherwise certain spiral into chaos and discord, the wise "\
  "ones of these ancient years created the Bulletin Board System, a by-then's-"\
  "standards-supreme weapon capable of staving off the darkness and "\
  "preserving a sacred order in the world.  Armed with such weapons, the "\
  "peoples of earth were once again united and no longer shrouded in dreadful "\
  "isolation.  However, with the rise of the internet, their service was no "\
  "longer required, and they faded into oblivion in the company of unsung "\
  "heroes...\n\n"\
  "This is not one of those bulletin board systems.\n\n"\
  "This is Brennan's BS Bulletin Board System, abbreviated BBSBBS, or BBS^2, "\
  "if you prefer.  It kind of sucks.  Not much to see here, but feel free to "\
  "poke around.\n"
#define STRING_COMMANDLIST "\nList of things you can do:\n"\
  "quit - terminate the connection\n"\
  "list files - display the list of readable files\n"\
  "read [filename] - read the contents of a file\n"\
  "session log - view previous login sessions\n"\
  "time - retrieve the server system time\n"\

void promptcommandlist();
void promptuser(char *inputmessage);
void sendreceiveprintif(unsigned int commandid, unsigned int targetid,
  char *targetname);
void parseuserinput();
