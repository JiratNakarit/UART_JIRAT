/*
*************************** C HEADER FILE ************************************

project   :
filename  : PIC24_SCAN_DEBOUNCE_SW.H
version   : 2
date      :

******************************************************************************

Copyright (c) 20xx ,
All rights reserved.

******************************************************************************

VERSION HISTORY:
----------------------------
Version      : 1
Date         :
Revised by   :
Description  :

Version      : 2
Date         :
Revised by   :
Description  : *
               *
               *

******************************************************************************
*/

#ifndef  PIC24_SCAN_DEBOUNCE_SW_INCLUDED
#define  PIC24_SCAN_DEBOUNCE_SW_INCLUDED

/****************************************************************************/
/**                                                                        **/
/**                              MODULES USED                              **/
/**                                                                        **/
/****************************************************************************/

//#include <stdlib.h>  // depend on application requirements.
//#include "cdat68k.h"  // define data types
#include "CONFIG_PIC24.h"
#include <stdlib.h>
#include <stdio.h>
#include "CDATPIC24.H"
#include "EVENT_DRIVEN_CCS.H"
#include "EVENT_TYPE_CCS.H"

/****************************************************************************/
/**                                                                        **/
/**                        DEFINITIONS AND MACROS                          **/
/**                                                                        **/
/****************************************************************************/
#define KEY_MAX_NUM 4

/****************************************************************************/
/**                                                                        **/
/**                        TYPEDEFS AND STRUCTURES                         **/
/**                                                                        **/
/****************************************************************************/


/****************************************************************************/
/**                                                                        **/
/**                         EXPORTED VARIABLES                             **/
/**                                                                        **/
/****************************************************************************/

#ifndef PIC24_SCAN_DEBOUNCE_SW_C_SRC
extern EV_STRUCT Sw0EvStruct, Sw1EvStruct, Sw2EvStruct, Sw3EvStruct;
extern EV_STRUCT Sw4EvStruct, Sw5EvStruct, Sw6EvStruct, Sw7EvStruct;


#endif

/****************************************************************************/
/**                                                                        **/
/**                       EXPORTED FUNCTIONS                               **/
/**                                                                        **/
/****************************************************************************/

int8u ReadKeyPort (void);

void ScanAndDebounceSW (int8u inPortValue, EVQ_STRUCT *evQueuePtr); 

/****************************************************************************/
#endif

/****************************************************************************/
/**                                                                        **/
/**                              EOF                                       **/
/**                                                                        **/
/****************************************************************************/