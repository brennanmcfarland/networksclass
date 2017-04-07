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

  u_int16_t o_to_r_seqno;
  u_int32_t o_to_r_ack;
  u_int16_t r_to_o_seqno;
  u_int32_t r_to_o_ack;

  int o_to_r_secsinceepoch_start;
  int o_to_r_msecsincesec_start;
  int o_to_r_secsinceepoch_end;
  int o_to_r_msecsincesec_end;
  int r_to_o_secsinceepoch_start;
  int r_to_o_msecsincesec_start;
  int r_to_o_secsinceepoch_end;
  int r_to_o_msecsincesec_end;
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
  int secsinceepoch, int msecsincesec, int isTCP, int app_data_vol, u_int16_t o_to_r_seqno,
  u_int32_t o_to_r_ack);
void initializeNewConnectionHashtableEntry(char **originatoripaddress,
  char **responderipaddress, unsigned int originatorport, unsigned int responderport,
  int secsinceepoch_start, int msecsincesec_start, int secsinceepoch_end,
  int msecsincesec_end, int isTCP, int app_data_vol, u_int16_t o_to_r_seqno, u_int32_t o_to_r_ack,
  ConnectionHashtableListNode **newnode);
void initializeRTTTimestamps(int secsinceepoch_start, int msecsincesec_start,
  int secsinceepoch_end, int msecsincesec_end, int app_data_vol, u_int16_t seqno,
  u_int32_t ackno, ConnectionHashtableListNode **newnode);
void updateOtoRStart(int secsinceepoch_start, int msecsincesec_start,
  int secsinceepoch_end, int msecsincesec_end, int app_data_vol, u_int16_t seqno,
  u_int32_t ackno, ConnectionHashtableListNode **currentnode);
void updateRtoOStart(int secsinceepoch_start, int msecsincesec_start,
  int secsinceepoch_end, int msecsincesec_end, int app_data_vol, u_int16_t seqno,
  u_int32_t ackno, ConnectionHashtableListNode **currentnode);
void updateOtoREnd(int secsinceepoch, int msecsincesec, u_int16_t seqno, u_int32_t ack_seqno,
  ConnectionHashtableListNode **currentnode);
void updateRtoOEnd(int secsinceepoch, int msecsincesec, u_int16_t seqno, u_int32_t ack_seqno,
  ConnectionHashtableListNode **currentnode);
void insertInConnectionHashtable(char **originatoripaddress, char **responderipaddress,
  unsigned int originatorport, unsigned int responderport, int secsinceepoch,
  int msecsincesec, int isTCP, u_int16_t o_to_r_seqno, u_int32_t o_to_r_ack, int app_data_vol);
int testConnectionEquality(char **originatoripaddress, char **responderipaddress,
  unsigned int originatorport, unsigned int responderport, int isTCP,
  ConnectionHashtableListNode *currentbucketnodeptr);
int compareTimeStamps(int secsinceepoch_a, int msecsincesec_a, int secsinceepoch_b,
  int msecsincesec_b);
double formatAsTrailingDecimal(int integerdigits);
unsigned int ConnectionHashtableHashCode(char **originatoripaddress,
  char **responderipaddress, unsigned int originatorport, unsigned int responderport);
void initializeConnectionHashtable();
void *ConnectionHashtableSafeMalloc (unsigned int sz);
