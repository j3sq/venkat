/*
 * nodesLog.h
 *
 * Created: 2017-11-20 13:48:46
 *  Author: A254382
 */ 

#ifndef __NODES_LOG__
#define __NODES_LOG__

#ifdef AVR_3PI
#include <pololu/3pi.h>
#else
#include "stubs.h"
#endif

typedef enum _nodeLogStatus_t
{
	SUCCES              = 0,
	ERR_LIST_FULL       = 1,
	ERR_INVALID_DATA    = 2,
	ERR_INVALID_TYPE    = 3,
	ERR_NO_NEW_LOG_DATA = 4,
	ERR_ALL_NODES_READ  = 5,	
}nodeLogStatus_t;

typedef enum _logNodeType_t
{
	NORMAL = 0,
	BONUS  = 1
}logNodeType_t;


/* This function must be called before the call of any function from 
   this module! */
void initNodeLog(void);

/* This function adds a value to the log list 
   uint8_t value       - This parameter contains the code that will be added 
				         to the list. Maximum allowed value is 127.
   logNodeType_t type  - This parameter specifies the type of the logged data.
                         Allowed values are NORMAL or BONUS. */
nodeLogStatus_t addValueToNodeLog(uint8_t value, logNodeType_t type);

/* This function is used to get a previously saved value from the node log.
   uint8_t *value      - This parameter contains the address of a variable which 
						 will be updated during the call of this function 
						 with the oldest code value in the node log. Possible values are 
						 between 0 and 127.
   logNodeType_t *type - This parameter contains the address of a variable which
						 will be updated during the call of this function with the
						 type of the oldest node saved previously saved in the list.
						 Allowed values are NORMAL or BONUS. */
nodeLogStatus_t getValueFromNodeLog(uint8_t *value, logNodeType_t *type);

/* This function prints all the visited nodes. Press A to see next visited node. */
void printVisitedNodes(void);

#endif

