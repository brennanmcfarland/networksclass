/*
  Brennan McFarland
  bfm21
  ConnectionHashtable.c
  4/26/17
  a hashtable for an transport layer connections
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h> //for memset
#include </usr/include/netinet/ip.h>
#include </usr/include/netinet/udp.h> //for the udphdr struct
#include </usr/include/netinet/tcp.h> //for the tcphdr struct
#include "ConnectionHashtable.h"
#include "McFarlandNetworks.h"


ConnectionHashtable connectionhashtable;
int connectionhashtablesize = INTINITIALIZER;
int connectionhashtablecapacity = DEFAULTCONNECTIONHASHTABLESIZE;


//for debugging
void printConnectionHashtableDebug()
{
  int i;
  for(i=INTINITIALIZER; i<sizeof(connectionhashtable.tableentrylists); i++)
  {
    ConnectionHashtableListNode *currentbucketnodeptr = (connectionhashtable.tableentrylists[i].head);
    if(currentbucketnodeptr != NULL)
    {
      do
      {
      }while((currentbucketnodeptr = currentbucketnodeptr->next) != NULL);
    }
    free(currentbucketnodeptr);
    printf("\n");
  }
}

//if found, return pointer to node, if not and list exists, return pointer to
//last element in list, if list dne, return null
ConnectionHashtableListNode *findInConnectionHashtable(char **originatoripaddress,
  char **responderipaddress, unsigned int originatorport, unsigned int responderport)
{
  unsigned int connectionhashtablesearchkey = ConnectionHashtableHashCode(
    originatoripaddress, responderipaddress, originatorport, responderport)
    %(unsigned int)connectionhashtablecapacity;
  //if the list DNE, it's not there
  if(connectionhashtable.tableentrylists[connectionhashtablesearchkey].head == NULL)
    return NULL;
  //see if it's in the list
  ConnectionHashtableListNode *currentbucketnodeptr = (
    connectionhashtable.tableentrylists[connectionhashtablesearchkey].head);
  while(currentbucketnodeptr->next != NULL &&
    ((ConnectionHashtableTestStringEquality(currentbucketnodeptr->entry->orig_ip,
      *originatoripaddress) == FALSE)
    || (ConnectionHashtableTestStringEquality(currentbucketnodeptr->entry->resp_ip,
      *responderipaddress) == FALSE)
    || (originatorport != currentbucketnodeptr->entry->orig_port)
    || (responderport != currentbucketnodeptr->entry->resp_port)
    ))
    currentbucketnodeptr = currentbucketnodeptr->next;
  return currentbucketnodeptr;
}

//returns whether 2 strings are equal
int ConnectionHashtableTestStringEquality(char *string1, char *string2)
{
  if(sizeof(string1) != sizeof(string2))
  {
    return FALSE;
  }
  int i;
  for(i=INTINITIALIZER; i<sizeof(string1)-OFFSETVALUE; i++)
  {
    if(string1[i] != string2[i])
    {
      return FALSE;
    }
  }
  return TRUE;
}

void initializeConnectionHashtableList(char **originatoripaddress,
  char **responderipaddress, unsigned int originatorport, unsigned int responderport,
  int secsinceepoch, int msecsincesec, int isTCP, int app_data_vol, u_int16_t o_to_r_seqno,
  u_int32_t o_to_r_ack)
{
  unsigned int connectionlistfirstkey = ConnectionHashtableHashCode(
    originatoripaddress, responderipaddress, originatorport, responderport)
    %(unsigned int)connectionhashtablecapacity;
  //printf("creating new list with key %u\n", connectionlistfirstkey);
  ConnectionHashtableListNode **headbuffer =
    (ConnectionHashtableListNode **)ConnectionHashtableSafeMalloc(POINTERSIZE);
  headbuffer = &(connectionhashtable.tableentrylists[connectionlistfirstkey].head);
  initializeNewConnectionHashtableEntry(originatoripaddress, responderipaddress,
    originatorport, responderport, secsinceepoch, msecsincesec,
    secsinceepoch, msecsincesec, isTCP, app_data_vol, o_to_r_seqno, o_to_r_ack, headbuffer);
}
/*
void initializeTrafficMatrixList(int listfirstentryipaddresskey,
  char **listfirstentrysourceipaddress, char **listfirstentrydestipaddress,
  int listfirstentrydatavol)
{
  MatrixListNode **headbuffer = (MatrixListNode **)trafficMatrixSafeMalloc(POINTERSIZE);
  headbuffer = &(trafficmatrix.tableentrylists[listfirstentryipaddresskey].head);
  initializeNewTrafficMatrixEntry(listfirstentryipaddresskey,
    listfirstentrysourceipaddress, listfirstentrydestipaddress,
    listfirstentrydatavol, headbuffer);
}*/

void initializeNewConnectionHashtableEntry(char **originatoripaddress,
  char **responderipaddress, unsigned int originatorport, unsigned int responderport,
  int secsinceepoch_start, int msecsincesec_start, int secsinceepoch_end,
  int msecsincesec_end, int isTCP, int app_data_vol, u_int16_t o_to_r_seqno, u_int32_t o_to_r_ack,
  ConnectionHashtableListNode **newnode)
{
  *newnode = (ConnectionHashtableListNode *)ConnectionHashtableSafeMalloc(sizeof(ConnectionHashtableListNode));
  (*newnode)->entry = (ConnectionHashtableListEntry *)ConnectionHashtableSafeMalloc(sizeof(ConnectionHashtableListEntry *));
  *((*newnode)->entry) = *(ConnectionHashtableListEntry *)ConnectionHashtableSafeMalloc(sizeof(ConnectionHashtableListEntry));
  (*newnode)->entry->orig_ip = *originatoripaddress;
  (*newnode)->entry->resp_ip = *responderipaddress;
  (*newnode)->entry->orig_port = originatorport;
  (*newnode)->entry->resp_port = responderport;
  (*newnode)->entry->secsinceepoch_start = secsinceepoch_start;
  (*newnode)->entry->msecsincesec_start = msecsincesec_start;
  (*newnode)->entry->secsinceepoch_end = secsinceepoch_end;
  (*newnode)->entry->msecsincesec_end = msecsincesec_end;
  (*newnode)->entry->isTCP = isTCP;
  initializePrevAcks(newnode);
  (*newnode)->entry->o_to_r_packets = OFFSETVALUE;
  (*newnode)->entry->o_to_r_app_bytes = app_data_vol;
  (*newnode)->entry->r_to_o_packets = INTINITIALIZER;
  (*newnode)->entry->r_to_o_app_bytes = INTINITIALIZER;
  (*newnode)->entry->o_to_r_seqno = o_to_r_seqno;
  (*newnode)->entry->o_to_r_ack = o_to_r_ack;
  updateOtoRPrevAcks(newnode, o_to_r_ack);
  (*newnode)->entry->r_to_o_seqno = INVALIDINT;
  (*newnode)->entry->r_to_o_ack = INVALIDINT;
  (*newnode)->entry->o_to_r_fastretransmits = INTINITIALIZER;
  (*newnode)->entry->r_to_o_fastretransmits = INTINITIALIZER;
  (*newnode)->entry->count = OFFSETVALUE;
  (*newnode)->next = NULL;
  initializeRTTTimestamps(secsinceepoch_start, msecsincesec_start, secsinceepoch_end,
    msecsincesec_end, app_data_vol, o_to_r_seqno, o_to_r_ack, newnode);
}

void initializeRTTTimestamps(int secsinceepoch_start, int msecsincesec_start,
  int secsinceepoch_end, int msecsincesec_end, int app_data_vol, u_int16_t seqno,
  u_int32_t ackno, ConnectionHashtableListNode **newnode)
{
  (*newnode)->entry->o_to_r_secsinceepoch_start = INVALIDINT;
  (*newnode)->entry->o_to_r_msecsincesec_start = INVALIDINT;
  (*newnode)->entry->o_to_r_secsinceepoch_end = INVALIDINT;
  (*newnode)->entry->o_to_r_msecsincesec_end = INVALIDINT;
  (*newnode)->entry->r_to_o_secsinceepoch_start = INVALIDINT;
  (*newnode)->entry->r_to_o_msecsincesec_start = INVALIDINT;
  (*newnode)->entry->r_to_o_secsinceepoch_end = INVALIDINT;
  (*newnode)->entry->r_to_o_msecsincesec_end = INVALIDINT;
  updateOtoRStart(secsinceepoch_start, msecsincesec_start, secsinceepoch_end,
    msecsincesec_end, app_data_vol, seqno, ackno, newnode);
}

void initializePrevAcks(ConnectionHashtableListNode **newnode)
{
  int i;
  for(i=0; i<PREVACKSSIZE; i++)
  {
    (*newnode)->entry->o_to_r_prevacks[i] = INVALIDINT;
    (*newnode)->entry->r_to_o_prevacks[i] = INVALIDINT;
  }
}

//check if timestamps for calculating rtt need to be updated with new information
void updateOtoRStart(int secsinceepoch_start, int msecsincesec_start,
  int secsinceepoch_end, int msecsincesec_end, int app_data_vol, u_int16_t seqno,
  u_int32_t ackno, ConnectionHashtableListNode **currentnode)
{
  if(app_data_vol > INTINITIALIZER && (*currentnode)->entry->o_to_r_secsinceepoch_start == INVALIDINT)
  {
    if(compareTimeStamps(secsinceepoch_start, msecsincesec_start,
      (*currentnode)->entry->o_to_r_secsinceepoch_start,
      (*currentnode)->entry->o_to_r_msecsincesec_start) == FALSE)
    {
      (*currentnode)->entry->o_to_r_secsinceepoch_start = secsinceepoch_start;
      (*currentnode)->entry->o_to_r_msecsincesec_start = msecsincesec_start;
      (*currentnode)->entry->o_to_r_seqno = seqno;
      (*currentnode)->entry->o_to_r_ack = ackno;
    }
  }
}

//check if timestamps for calculating rtt need to be updated with new information
void updateRtoOStart(int secsinceepoch_start, int msecsincesec_start,
  int secsinceepoch_end, int msecsincesec_end, int app_data_vol, u_int16_t seqno,
  u_int32_t ackno, ConnectionHashtableListNode **currentnode)
{
  if(app_data_vol > INTINITIALIZER && (*currentnode)->entry->r_to_o_secsinceepoch_start == INVALIDINT)
  {
    if(compareTimeStamps(secsinceepoch_start, msecsincesec_start,
      (*currentnode)->entry->r_to_o_secsinceepoch_start,
      (*currentnode)->entry->r_to_o_msecsincesec_start) == FALSE)
    {
      (*currentnode)->entry->r_to_o_secsinceepoch_start = secsinceepoch_start;
      (*currentnode)->entry->r_to_o_msecsincesec_start = msecsincesec_start;
      (*currentnode)->entry->r_to_o_seqno = seqno;
      (*currentnode)->entry->r_to_o_ack = ackno;
    }
  }
}

//check if this is the first sequence no >= the desired one for rtt
void updateOtoREnd(int secsinceepoch, int msecsincesec, u_int16_t seqno, u_int32_t ack_seqno,
  ConnectionHashtableListNode **currentnode)
{
  //check if found already, ie if o_to_r end timestamp fields are INVALIDINT
  if((*currentnode)->entry->o_to_r_secsinceepoch_start != INVALIDINT
    && (*currentnode)->entry->o_to_r_secsinceepoch_end == INVALIDINT)
  {
    //if not, check if the new sequence # is >= to the old one, if so update timestamp
    if((*currentnode)->entry->o_to_r_seqno <= ack_seqno)
    {
      (*currentnode)->entry->o_to_r_secsinceepoch_end = secsinceepoch;
      (*currentnode)->entry->o_to_r_msecsincesec_end = msecsincesec;
    }
  }
}

//check if this is the first sequence no >= the desired one for rtt
void updateRtoOEnd(int secsinceepoch, int msecsincesec, u_int16_t seqno, u_int32_t ack_seqno,
  ConnectionHashtableListNode **currentnode)
{
  //check if found already, ie if o_to_r end timestamp fields are INVALIDINT
  if((*currentnode)->entry->r_to_o_secsinceepoch_start != INVALIDINT
    && (*currentnode)->entry->r_to_o_secsinceepoch_end == INVALIDINT)
  {
    //if not, check if the new sequence # is >= to the old one, if so update timestamp
    if((*currentnode)->entry->r_to_o_seqno <= ack_seqno)
    {
      (*currentnode)->entry->r_to_o_secsinceepoch_end = secsinceepoch;
      (*currentnode)->entry->r_to_o_msecsincesec_end = msecsincesec;
    }
  }
}

//update the array of previous acks and check if there's 3 dup acks in a row
void updateOtoRPrevAcks(ConnectionHashtableListNode **currentnode, u_int32_t newack)
{
  //printf("|%d| ", (int)newack);
  int i;
  for(i=PREVACKSSIZE-OFFSETVALUE; i>INTINITIALIZER; i--)
  {
    (*currentnode)->entry->o_to_r_prevacks[i] =
      (*currentnode)->entry->o_to_r_prevacks[i-OFFSETVALUE];
  }
  (*currentnode)->entry->o_to_r_prevacks[INTINITIALIZER] =
    newack;
  printf("%d %d %d\n", (int)(*currentnode)->entry->o_to_r_prevacks[0],
    (int)(*currentnode)->entry->o_to_r_prevacks[1],
    (int)(*currentnode)->entry->o_to_r_prevacks[2]);
  //check fast retransmit condition, return if false
  for(i=INTINITIALIZER; i<PREVACKSSIZE-OFFSETVALUE; i++)
  {
    if((*currentnode)->entry->o_to_r_prevacks[i] !=
      (*currentnode)->entry->o_to_r_prevacks[i+OFFSETVALUE])
      return;
  }
  (*currentnode)->entry->o_to_r_fastretransmits++;
  //do i need to call re-initialize otor prevacks here?
}

//update the array of previous acks and check if there's 3 dup acks in a row
void updateRtoOPrevAcks(ConnectionHashtableListNode **currentnode, u_int32_t newack)
{
  int i;
  for(i=PREVACKSSIZE-OFFSETVALUE; i>INTINITIALIZER; i--)
  {
    (*currentnode)->entry->r_to_o_prevacks[i] =
      (*currentnode)->entry->r_to_o_prevacks[i-OFFSETVALUE];
  }
  (*currentnode)->entry->r_to_o_prevacks[OFFSETVALUE] =
    newack;
  //check fast retransmit condition, return if false
  for(i=0; i<PREVACKSSIZE-OFFSETVALUE; i++)
  {
    if((*currentnode)->entry->r_to_o_prevacks[i] !=
      (*currentnode)->entry->r_to_o_prevacks[i+OFFSETVALUE])
      return;
  }
  (*currentnode)->entry->r_to_o_fastretransmits++;
  //do i need to call re-initialize rtoo prevacks here?
}

void insertInConnectionHashtable(char **originatoripaddress, char **responderipaddress,
  unsigned int originatorport, unsigned int responderport, int secsinceepoch,
  int msecsincesec, int isTCP, u_int16_t seqno, u_int32_t ack, int app_data_vol)
{
  unsigned int newconnectiontableentrykey = ConnectionHashtableHashCode(
    originatoripaddress, responderipaddress, originatorport, responderport)
    %(unsigned int)connectionhashtablecapacity;
  //if it already exists in the list, increment the # occurences, if not create
  //first check if list DNE
  if(connectionhashtable.tableentrylists[newconnectiontableentrykey].head == NULL)
  {
    initializeConnectionHashtableList(originatoripaddress, responderipaddress,
      originatorport, responderport, secsinceepoch, msecsincesec, isTCP, app_data_vol,
      seqno, ack);
    connectionhashtablesize++;
    //printf("inserting in new bucket %d\n", newconnectiontableentrykey);
  }
  else
  {
    //see if it's in the list, if so increment value, if not add it
    ConnectionHashtableListNode *currentbucketnodeptr = (
      connectionhashtable.tableentrylists[newconnectiontableentrykey].head);
    currentbucketnodeptr = findInConnectionHashtable(originatoripaddress,
      responderipaddress, originatorport, responderport);
    if(testConnectionEquality(originatoripaddress, responderipaddress,
        originatorport, responderport, isTCP, currentbucketnodeptr) == TRUE
      ||
      testConnectionEquality(responderipaddress, originatoripaddress,
        responderport, originatorport, isTCP, currentbucketnodeptr) == TRUE)
    {
      if(testConnectionEquality(originatoripaddress, responderipaddress,
        originatorport, responderport, isTCP, currentbucketnodeptr) == TRUE)
      {
        updateOtoRStart(secsinceepoch, msecsincesec, secsinceepoch, msecsincesec,
          app_data_vol, seqno, ack, (ConnectionHashtableListNode **)&currentbucketnodeptr);
        updateRtoOEnd(secsinceepoch, msecsincesec, seqno, ack,
          (ConnectionHashtableListNode **)&currentbucketnodeptr);
        updateOtoRPrevAcks((ConnectionHashtableListNode **)&currentbucketnodeptr, ack);
        currentbucketnodeptr->entry->o_to_r_packets++;
        currentbucketnodeptr->entry->o_to_r_app_bytes += app_data_vol;
      }
      else
      {
        currentbucketnodeptr->entry->r_to_o_packets++;
        currentbucketnodeptr->entry->r_to_o_app_bytes += app_data_vol;
        updateOtoREnd(secsinceepoch, msecsincesec, seqno, ack,
          (ConnectionHashtableListNode **)&currentbucketnodeptr);
        updateRtoOStart(secsinceepoch, msecsincesec, secsinceepoch, msecsincesec,
          app_data_vol, seqno, ack, (ConnectionHashtableListNode **)&currentbucketnodeptr);
        updateRtoOPrevAcks((ConnectionHashtableListNode **)&currentbucketnodeptr, ack);
      }
      if(compareTimeStamps(secsinceepoch, msecsincesec,
        currentbucketnodeptr->entry->secsinceepoch_start,
        currentbucketnodeptr->entry->msecsincesec_start) == TRUE)
      {
        currentbucketnodeptr->entry->secsinceepoch_start = secsinceepoch;
        currentbucketnodeptr->entry->msecsincesec_start = msecsincesec;
      }
      if(compareTimeStamps(secsinceepoch, msecsincesec,
        currentbucketnodeptr->entry->secsinceepoch_end,
        currentbucketnodeptr->entry->msecsincesec_end) == FALSE)
      {
        currentbucketnodeptr->entry->secsinceepoch_end = secsinceepoch;
        currentbucketnodeptr->entry->msecsincesec_end = msecsincesec;
      }
      //printf("incrementing count of existing node in bucket %d\n", newconnectiontableentrykey);
    }
    else if(currentbucketnodeptr != NULL)
    {
      ConnectionHashtableListNode **currentbucketnodeptrbuffer = (ConnectionHashtableListNode **)ConnectionHashtableSafeMalloc(
        POINTERSIZE);
      currentbucketnodeptrbuffer = &(currentbucketnodeptr->next);
      initializeNewConnectionHashtableEntry(originatoripaddress, responderipaddress,
        originatorport, responderport, secsinceepoch, msecsincesec, secsinceepoch,
        msecsincesec, isTCP, app_data_vol, seqno, ack, currentbucketnodeptrbuffer);
      connectionhashtablesize++;
      //updateOtoREnd(secsinceepoch, msecsincesec, seqno, ack_seqno, currentbucketnodeptrbuffer);
      //printf("inserting in new node in bucket %d\n", newconnectiontableentrykey);
    }
  }
}

//TRUE if packets are from the same connection
int testConnectionEquality(char **originatoripaddress, char **responderipaddress,
  unsigned int originatorport, unsigned int responderport, int isTCP,
  ConnectionHashtableListNode *currentbucketnodeptr)
{
  if(currentbucketnodeptr != NULL && (ConnectionHashtableTestStringEquality(currentbucketnodeptr
    ->entry->orig_ip,*originatoripaddress) == TRUE)
    && (ConnectionHashtableTestStringEquality(currentbucketnodeptr->entry->resp_ip,
      *responderipaddress) == TRUE)
    && (currentbucketnodeptr->entry->orig_port == originatorport)
    && (currentbucketnodeptr->entry->resp_port == responderport)
    && (isTCP == currentbucketnodeptr->entry->isTCP))
    return TRUE;
  return FALSE;
}

//TRUE if timestamp b is later, FALSE otherwise
int compareTimeStamps(int secsinceepoch_a, int msecsincesec_a, int secsinceepoch_b,
  int msecsincesec_b)
{
  //printf("%f\n", (double)msecsincesec_a);
  if((double)secsinceepoch_a+formatAsTrailingDecimal(msecsincesec_a) <
    (double)secsinceepoch_b+formatAsTrailingDecimal(msecsincesec_b))
    return TRUE;
  return FALSE;
}

//given an integer, formats as a decimal value trailing the decimal point
double formatAsTrailingDecimal(int integerdigits)
{
  double trailingdecimal = (double)integerdigits;
  trailingdecimal = trailingdecimal/TRAILINGDECIMALCONVERTER;
  return trailingdecimal;
}

unsigned int ConnectionHashtableHashCode(char **originatoripaddress,
  char **responderipaddress, unsigned int originatorport, unsigned int responderport)
{
  unsigned int hashcode = (unsigned int)(*originatoripaddress)[0]+originatorport+
    (unsigned int)(*responderipaddress)[INTINITIALIZER]+responderport;
  return hashcode;
}

void initializeConnectionHashtable()
{
  connectionhashtable.tableentrylists = ConnectionHashtableSafeMalloc(DEFAULTCONNECTIONHASHTABLESIZE*sizeof(
    ConnectionHashtableList)*POINTERSIZE);
  //clear the array
  int i;
  for(i=INTINITIALIZER; i<DEFAULTCONNECTIONHASHTABLESIZE; i++)
  {
    connectionhashtable.tableentrylists[i] = *(ConnectionHashtableList *)ConnectionHashtableSafeMalloc(sizeof(
      ConnectionHashtableList));
    connectionhashtable.tableentrylists[i].head = NULL;
  }
  connectionhashtablesize = INTINITIALIZER;
  connectionhashtablecapacity = DEFAULTCONNECTIONHASHTABLESIZE;
}

void *ConnectionHashtableSafeMalloc (unsigned int sz)
{
    void *p;
    if ((p = (void *)malloc (sz)) == NULL)
    {
        printf ("memory allocation failed, exiting ...\n");
        exit (EXIT_ERRORCODE);
    }
    memset (p,FALSE,sz);
    return (p);
}
