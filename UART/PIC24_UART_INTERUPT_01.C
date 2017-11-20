/*
*************************** C SOURCE FILE ************************************

project   :
filename  : PIC24_UART_INTERUPT_01.C
version   : 2
date      :

******************************************************************************

Copyright (c) 20xx
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

#define PIC24_UART_INTERUPT_01_C_SRC

/****************************************************************************/
/**                                                                        **/
/**                             MODULES USED                               **/
/**                                                                        **/
/****************************************************************************/

#include "PIC24_UART_INTERUPT_01.H"
#include<string.h>
#include<stdio.h>

/****************************************************************************/
/**                                                                        **/
/**                        DEFINITIONS AND MACROS                          **/
/**                                                                        **/
/****************************************************************************/


//UART Rx frame
#define RX1Q_LN 8
#define RX_CMND_FRM_LN 25
#define START_CHR   '['
#define END_CHR     ']'
#define CONDITION_S '('
#define CONDITION_E ')'
#define CONDITION_M ','

#define FINISH2COM  '>'

//UART Queue
#define TX1Q_LN 128

/****************************************************************************/
/**                                                                        **/
/**                        TYPEDEFS AND STRUCTURES                         **/
/**                                                                        **/
/****************************************************************************/


/****************************************************************************/
/**                                                                        **/
/**                      PROTOTYPES OF LOCAL FUNCTIONS                     **/
/**                                                                        **/
/****************************************************************************/
static void HardwareInit (void);
static void GlobalVarInit (void);
static void DynamicMemInit (void);
static void UARTQueueInit (void);
static int8u SendTx1 (int8u *strPtr);
static int8u ConvertStr2Int (int8u num);

/****************************************************************************/
/**                                                                        **/
/**                           EXPORTED VARIABLES                           **/
/**                                                                        **/
/****************************************************************************/


/****************************************************************************/
/**                                                                        **/
/**                            GLOBAL VARIABLES                            **/
/**                                                                        **/
/****************************************************************************/

static volatile Q8UX_STRUCT Tx1QCB;
static volatile int8u Tx1QArray[TX1Q_LN];
static volatile int8u *Tx1BuffPtr;
static volatile int16u TxBuffIdx;
static volatile TX1_STATUS Tx1Flag;
static volatile int16u Tx1FrameIn, Tx1FrameOut, Rx1FrameCount, RxCount,
                        Tx1QFullCount, Rx1QFullCount;

static volatile int8u *RxBuffPtr;
static volatile QPTRX_STRUCT Rx1QCB;
static volatile PTR_STRUCT Rx1BuffPtrArray[RX1Q_LN];
static volatile PTR_STRUCT DestPtrStruct;

static volatile int16u MemFail, MemCount;

static int position_x[50];
static int position_y[50];

/****************************************************************************/
/**                                                                        **/
/**                           EXPORTED FUNCTIONS                           **/
/**                                                                        **/
/****************************************************************************/


/****************************************************************************/
/**                                                                        **/
/**                             LOCAL FUNCTIONS                            **/
/**                                                                        **/
/****************************************************************************/

void main (void)
{
    int8u errCode, sendTx1Count;
    int8u c_state = 0;
    int8u s_state = 0;
    int8u sum_a = 0; 
    int8u sum_b = 0;
    int8u sum_c = 0;
    int8u pos_r = 0;
    int *posY;
    int *posX;
    int8u Arr_state = 0;

    posY = position_y;
    posX = position_x;

    DisableIntr ();
    HardwareInit ();
    GlobalVarInit ();
    DynamicMemInit ();
    UARTQueueInit ();
    EnableIntr ();
    for(;;){
        DisableIntr();
        QPtrXGet(&Rx1QCB, &DestPtrStruct, &errCode);
        if (errCode == Q_OK)
        {
            for(int8u i = 0; i < strlen(DestPtrStruct.blockPtr); i++)
            {
                if(DestPtrStruct.blockPtr[i] == CONDITION_S)
                {
                    c_state = 1;
                }
                else if(DestPtrStruct.blockPtr[i] == CONDITION_M)
                {
                    if(c_state == 1)
                    {
                        //Found x
                        if(s_state==1){pos_r = sum_a;}
                        if(s_state==2){pos_r = sum_a*10+sum_b;}
                        if(s_state==3){pos_r = sum_a*100+sum_b*10+sum_c;}
                        //printf("\r\n x == %d", pos_r);
                        *(posX+Arr_state) = pos_r;
                        s_state = 0;
                        c_state = 0;
                    }
                }
                else if(DestPtrStruct.blockPtr[i] == CONDITION_E)
                {
                    //Found y
                    if(s_state==1){pos_r = sum_a;}
                    if(s_state==2){pos_r = sum_a*10+sum_b;}
                    if(s_state==3){pos_r = sum_a*100+sum_b*10+sum_c;}
                    //printf("\r\n y == %d", pos_r);
                    *(posY+Arr_state) = pos_r;
                    s_state = 0;
                    Arr_state++;
                }
                else
                {
                    //Found numeric
                    //printf("\r\nDestPtrStruct.blockPtr[i] = %c", DestPtrStruct.blockPtr[i]);
                    if(s_state == 0){sum_a = ConvertStr2Int(DestPtrStruct.blockPtr[i]);}
                    if(s_state == 1){sum_b = ConvertStr2Int(DestPtrStruct.blockPtr[i]);}
                    if(s_state == 2){sum_c = ConvertStr2Int(DestPtrStruct.blockPtr[i]);}
                    s_state++;
                }
                
            }
            
            sendTx1Count = SendTx1 (((int8u *)DestPtrStruct.blockPtr));
            free ((void *)DestPtrStruct.blockPtr);
            MemCount--;
            EnableIntr();
            //printf("\r\n --> %d", position_x[1]);
        }
        else
        {
            EnableIntr();
        }
    }
}

static void HardwareInit (void)
{
    setup_adc_ports(NO_ANALOGS);
    //GPIO
    set_tris_a(get_tris_a() & 0xffeb);
    set_tris_b(get_tris_b() & 0xfff3);
    output_high (LED0);
    output_high (LED1);
    output_high (LED2);
    output_high (LED3);

}
static void GlobalVarInit (void)
{
    Tx1Flag = TX1_READY;
    TxBuffIdx = 0;
    Tx1FrameIn = 0;
    Tx1QFullCount = 0;
    Rx1FrameCount = 0;
    RxCount = 0;
    Rx1QFullCount = 0;
    MemFail = 0;
    MemCount = 0;
    return;
}
static void DynamicMemInit (void)
{
    //get mem block for first Rx1 buffer
    RxBuffPtr = (int8u *)malloc ((sizeof (int8u)) * RX_CMND_FRM_LN);
    if (RxBuffPtr != (int8u *)NULL)
    {
        MemCount++;
        clear_interrupt(INT_RDA);
        enable_interrupts (INT_RDA);
    }
    else
    {
        MemFail++;
    }
    return;
}
static void UARTQueueInit (void)
{
    QPtrXInit(&Rx1QCB, Rx1BuffPtrArray, RX1Q_LN);
    Q8UXInit(&Tx1QCB, Tx1QArray, TX1Q_LN);
    return;
}
static int8u SendTx1 (int8u *strPtr)//critical section
{
    int8u strLn;
    int8u strIdx;
    int8u qSpace;
    int8u errCode;
    int8u count;
    count = 0;
    strLn = strlen(strPtr);
    if (strLn != 0)
    {
        qSpace = TX1Q_LN - Q8UXCount (&Tx1QCB);
        if (qSpace >= (int16u)strLn)
        {
            for (strIdx = 0; strIdx < strLn; strIdx++)
            {
                Q8UXPut (&Tx1QCB, strPtr[strIdx], &errCode);
                count++;
            }
            if (Tx1Flag == TX1_READY)
            {
                Tx1Flag = TX1_BUSY; // Set Rx1 to Busy.
                TX1IF = 1;// Start Tx1 Interupt. 
                enable_interrupts(INT_TBE);
            }
        }
    }
    return count;
}

static int8u ConvertStr2Int (int8u num)
{
	return num - '0';
}
/****************************************************************************/

/****************************************************************************/
/**                           Interrupt Functions                          **/
/****************************************************************************/

#INT_RDA //Rx1 interupt
void RDA1 (void)
{
    static FRAME_STATE FrameState = FRAME_WAIT;
    static int16u FrmIdx = 0;
    int8u Chr;
    int8u *errCode;

    Chr = getc(); // Read data from Rx1 register
    RxCount++;
    switch (FrameState) // State machine for build frame
    {
        case FRAME_WAIT: // waits for start char of new frame
            if (Chr == START_CHR)
            {
                //Get a start char
                //RxBuffPtr[FrmIdx] = Chr; // Save start char to frame
                //FrmIdx++;
                FrameState = FRAME_PROGRESS; // Change state to build frame
            }
            break;
        case FRAME_PROGRESS: // Build frame
            if ((FrmIdx == (RX_CMND_FRM_LN - 2)) && (Chr != END_CHR))
            {
                //Characters exceed frame lenght. Frame error.
                // Rejects data and black to wait new frame.
                FrmIdx = 0;
                FrameState = FRAME_WAIT;
            }
            else if (Chr == END_CHR)
            {
                //Get and char. Frame completes.
                //RxBuffPtr[FrmIdx] = Chr;
                //FrmIdx++;
                RxBuffPtr[FrmIdx] = 0;
                FrmIdx = 0;
                Rx1FrameCount++;
                //Sends Rx1 event to event queue.
                QPtrXPut (&Rx1QCB, (void *)RxBuffPtr, &errCode);
                if (errCode == Q_FULL)
                {
                    //EvQ full error. Free mem of data block.
                    free ((void *)RxBuffPtr);
                    MemCount--;
                    Rx1QFullCount++;
                }
                FrameState = FRAME_WAIT; // Back to wait for start char of new frame.
                // GEt mem block for better of new frame
                RxBuffPtr = (int8u *)malloc ((sizeof (int8u)) * RX_CMND_FRM_LN);
                if (RxBuffPtr == (int8u *)NULL)
                {
                    //Can not get mem block. Disable Rx1 interupt.
                    disable_interrupts(INT_RDA);
                    MemFail++;
                }
                else
                {
                    MemCount++;
                }
            }
            else
            {
                RxBuffPtr[FrmIdx] = Chr;
                FrmIdx++;
            }
            break;
        default:
            break;
    }
    return;
}

#INT_TBE // Tx1 interupt
void TBE1ISR (void)
{
    int8u destChr;
    Q_ERR errCode;
    Q8UXGet (&Tx1QCB, &destChr, &errCode);
    if (errCode == Q_OK)
    {
        putc(destChr);
    }
    else
    {
        disable_interrupts (INT_TBE);
        Tx1Flag = TX1_READY;
    }
    return;
}

/****************************************************************************/

/****************************************************************************/
/**                                                                        **/
/**                                 EOF                                    **/
/**                                                                        **/
/****************************************************************************/