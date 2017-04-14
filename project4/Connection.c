
#include "Connection.h"


char *getcommand_name(unsigned int command_id)
{
  switch(command_id)
  {
    case 1:
      return "TEST";
      break;
    default:
      return "ERR_INVALID_COMMAND";
  }
}
