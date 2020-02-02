/*
 * nodesLog.c
 *
 * Created: 2017-11-20 13:52:05
 *  Author: A254382
 */ 

#include "nodesLog.h"

#define MAXIMUM_NUMBER_OF_NODES  (32u)

#if ((MAXIMUM_NUMBER_OF_NODES) > 127u)
	#error "ERRORR - To many nodes defined! - Please decrease MAXIMUM_NUMBER_OF_NODES"
#endif

#define BONUS_MARKER_MASK         (0x80u)
#define MAXIMUM_ALLOWED_VALUE     (0x7Fu)

static uint8_t logArray[MAXIMUM_NUMBER_OF_NODES];
static uint8_t indexRead_logArray = 0;
static uint8_t indexWrite_logArray = 0;

void initNodeLog(void)
{
	uint8_t i = 0;
	
	indexRead_logArray  = 0;
	indexWrite_logArray = 0;
	for (i = 0; i < MAXIMUM_NUMBER_OF_NODES; i++)
	{
		logArray[i] = 0;
	}
}

nodeLogStatus_t addValueToNodeLog(uint8_t value, logNodeType_t type)
{
	nodeLogStatus_t status = SUCCES;
	
	if (indexWrite_logArray < MAXIMUM_NUMBER_OF_NODES)
	{
		if (value <= MAXIMUM_ALLOWED_VALUE)
		{
			
			switch (type)
			{
				case BONUS:
					logArray[indexWrite_logArray] = value;
					logArray[indexWrite_logArray] |= BONUS_MARKER_MASK;	
					indexWrite_logArray++;
				break;
				case NORMAL:
					logArray[indexWrite_logArray] = value;
					indexWrite_logArray++;
				break;
				default:
					status = ERR_INVALID_TYPE;
				break;
			}			
		}
		else
		{
			status = ERR_INVALID_DATA;
		}
	}
	else
	{
		status = ERR_LIST_FULL;
	}
	
	return status;
}

nodeLogStatus_t getValueFromNodeLog(uint8_t *value, logNodeType_t *type)
{
	nodeLogStatus_t status = SUCCES;
	
	if(indexRead_logArray < MAXIMUM_NUMBER_OF_NODES)
	{
		if (indexRead_logArray < indexWrite_logArray)
		{
			*value = logArray[indexRead_logArray];
			if (logArray[indexRead_logArray] & BONUS_MARKER_MASK)
			{
				*type = BONUS;
				*value &= ~BONUS_MARKER_MASK;
			}
			else
			{
				*type = NORMAL;	
			}
			indexRead_logArray++;
		}
		else
		{
			status = ERR_NO_NEW_LOG_DATA;
		}
	}
	else
	{
		status = ERR_ALL_NODES_READ;
	}
	return status;
}

void printVisitedNodes(void)
{
	while(1)
	{
		nodeLogStatus_t status = SUCCES;
		uint8_t value = 0;
		logNodeType_t nodeType = NORMAL;
		clear();
		lcd_goto_xy(0,0);
		print("START");
		wait_for_button(BUTTON_A);
		status = getValueFromNodeLog(&value, &nodeType);
		while (status != ERR_ALL_NODES_READ && status != ERR_NO_NEW_LOG_DATA)
		{
			if (SUCCES == status)
			{
				clear();
				lcd_goto_xy(0,0);
				print_long(value);
				lcd_goto_xy(0,1);
				switch (nodeType)
				{
					case NORMAL:
						print ("NORMAL");
					break;
					case BONUS:
						print ("BONUS");
					break;
					default:
						print ("UNKNOWN");
					break;
				}
				wait_for_button(BUTTON_A);
			}
			else
			{
				clear();
				lcd_goto_xy(0,0);
				print("ERROR");
				lcd_goto_xy(0,1);
				print_long((uint8_t)status);
			}
			status = getValueFromNodeLog(&value, &nodeType);
		}
		clear();
		lcd_goto_xy(0,0);
		print("END");
		wait_for_button(BUTTON_A);
		indexRead_logArray = 0;
	}
}
