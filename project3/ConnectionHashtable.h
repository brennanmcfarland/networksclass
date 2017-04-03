/*
  Brennan McFarland
  bfm21
  ConnectionHashtable.h
  3/3/17
  The header for ConnectionHashtable.c
*/

#include "McFarlandNetworks.h"

#define DEFAULTCONNECTIONHASHTABLESIZE 16
#define CONNECTIONHASHTABLEGROWTHFACTOR 2
#define ENTRYCOUNTINITIALIZER 1

typedef struct ConnectionHashtableListEntry
{
  char *orig_ip;
  char *resp_ip;
  unsigned int orig_port;
  unsigned int resp_port;
  int secsinceepoch_start;
  int msecsincesec_start;
  int secsinceepoch_end;
  int msecsincesec_end;
  int isTCP;
  unsigned int o_to_r_packets;
  int o_to_r_app_bytes;
  unsigned int r_to_o_packets;
  int r_to_o_app_bytes;
  //deprecated, remove these once it's all transferred over to the new stuff
  int count;
  int datavol;
} ConnectionHashtableListEntry;

typedef struct ConnectionHashtableListNode
{
  ConnectionHashtableListEntry *entry;
  struct ConnectionHashtableListNode *next;
} ConnectionHashtableListNode;

typedef struct ConnectionHashtableList
{
  ConnectionHashtableListNode *head;
} ConnectionHashtableList;

typedef struct ConnectionHashtable
{
  ConnectionHashtableList *tableentrylists;
} ConnectionHashtable;

void printConnectionHashtableDebug();
ConnectionHashtableListNode *findInConnectionHashtable(char **originatoripaddress,
  char **responderipaddress, unsigned int originatorport, unsigned int responderport);
int ConnectionHashtableTestStringEquality(char *string1, char *string2);
void initializeConnectionHashtableList(char **originatoripaddress,
  char **responderipaddress, unsigned int originatorport, unsigned int responderport,
  int secsinceepoch, int msecsincesec, int isTCP, int app_data_vol);
void initializeNewConnectionHashtableEntry(char **originatoripaddress,
  char **responderipaddress, unsigned int originatorport, unsigned int responderport,
  int secsinceepoch_start, int msecsincesec_start, int secsinceepoch_end,
  int msecsincesec_end, int isTCP, int app_data_vol, ConnectionHashtableListNode **newnode);
void insertInConnectionHashtable(char **originatoripaddress, char **responderipaddress,
  unsigned int originatorport, unsigned int responderport, int secsinceepoch,
  int msecsincesec, int isTCP, int app_data_vol);
int testConnectionEquality(char **originatoripaddress, char **responderipaddress,
  unsigned int originatorport, unsigned int responderport, int isTCP,
  ConnectionHashtableListNode *currentbucketnodeptr);
int compareTimeStamps(int secsinceepoch_a, int msecsincesec_a, int secsinceepoch_b,
  int msecsincesec_b);
unsigned int ConnectionHashtableHashCode(char **originatoripaddress,
  char **responderipaddress, unsigned int originatorport, unsigned int responderport);
void initializeConnectionHashtable();
void *ConnectionHashtableSafeMalloc (unsigned int sz);
