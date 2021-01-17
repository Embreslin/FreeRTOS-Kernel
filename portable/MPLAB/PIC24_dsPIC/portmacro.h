/*
 * FreeRTOS Kernel V10.4.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 * 1 tab == 4 spaces!
 */

#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

/* a bit of a hack to get the stack limit -- do the hack on portSETUP_TCB and we could use traceTASK_CREATE for Tracelyzer library.*/
//#ifdef traceTASK_CREATE
//#error traceTASK_CREATE is used by portmacro.h - please combine them
//#endif
//#define traceTASK_CREATE(pxnewTCB) 	 do{ *(pxNewTCB->pxTopOfStack)++ = (StackType_t)(pxNewTCB->pxEndOfStack - 16);  }while(0)
#define portSETUP_TCB(pxnewTCB) 	 do{ *(pxNewTCB->pxTopOfStack)++ = (StackType_t)(pxNewTCB->pxEndOfStack - 16);  }while(0)
/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE	uint16_t
#define portBASE_TYPE	short
#define portPOINTER_SIZE_TYPE uint16_t
//#define portPOINTER_SIZE_TYPE uint32_t

typedef portSTACK_TYPE StackType_t;
typedef short BaseType_t;
typedef unsigned short UBaseType_t;

#if (configUSE_16_BIT_TICKS == 1)
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffff
#else
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#endif
#define WAIT_FOREVER portMAX_DELAY
/*-----------------------------------------------------------*/

/* Hardware specifics. */
#define portBYTE_ALIGNMENT			2
#define portSTACK_GROWTH			1
#define portTICK_PERIOD_MS			( ( TickType_t ) 1000 / configTICK_RATE_HZ )
/*-----------------------------------------------------------*/

/* Critical section management. */
#define portINTERRUPT_BITS			( ( uint16_t ) configKERNEL_INTERRUPT_PRIORITY << ( uint16_t ) 5 )

#define portDISABLE_INTERRUPTS()	SET_CPU_IPL(configKERNEL_INTERRUPT_PRIORITY)
#define portENABLE_INTERRUPTS()		SET_CPU_IPL(0)

/* Note that exiting a critical section through portEXIT_CRITICAL will set the
IPL bits to 0, no matter what their value was prior to entering the critical section.
To keep the IPL register's info, see the macros at "ISRs Critical Section" below. */
extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );
#define portENTER_CRITICAL()            vPortEnterCritical()
#define portEXIT_CRITICAL()             vPortExitCritical()

/* ISRs Critical Section */
extern UBaseType_t uxPortSetISRMask( void );
#define portSET_INTERRUPT_MASK_FROM_ISR()                 uxPortSetISRMask()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR( uxLastIPL )    RESTORE_CPU_IPL( uxLastIPL )
/*-----------------------------------------------------------*/

/* Task utilities. */
extern void vPortYield( void );
#define portYIELD()				asm volatile ( "CALL _vPortYield			\n"		\
												"NOP					  " );
/*-----------------------------------------------------------*/

/* Task ISR utilities. */
extern volatile UBaseType_t uxISRProcNesting;
#define portSTART_ISR()     uxISRProcNesting++
#define portEND_ISR()       uxISRProcNesting--

extern volatile UBaseType_t uxYieldDelayed;
/* Note that Yielding from ISR evaluates the need of a Higher prio. task to run
or if a Yield was previously delayed by the Tick or another ISR w/ Higher prio. */
#define portYIELDFromISR( uxHigherPriorityTask )                                                   \
{                                                                                                  \
    portEND_ISR();                                                                                 \
    if( uxISRProcNesting == 0 && ( uxHigherPriorityTask == pdTRUE || uxYieldDelayed == pdTRUE ) )  \
    {                                                                                              \
        uxYieldDelayed = pdFALSE;                                                                  \
        portYIELD();                                                                               \
    }                                                                                              \
    if( uxHigherPriorityTask == pdTRUE && uxISRProcNesting != 0 )                                  \
    {                                                                                              \
        uxYieldDelayed = pdTRUE;                                                                   \
    }                                                                                              \
} /* portYIELDFromISR */
/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) CALLBACK void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) CALLBACK void vFunction( void *pvParameters )
/*-----------------------------------------------------------*/

/* Required by the kernel aware debugger. */
#ifdef __DEBUG
	#define portREMOVE_STATIC_QUALIFIER
#endif

#define portNOP()				asm volatile ( "NOP" )

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

