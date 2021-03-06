/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * \version 2017.07
 * \date 2017-07-26
 * \author Xiangcai Huang(xiangcai@synopsys.com)
--------------------------------------------- */

/**
 * \file
 * \ingroup	EMBARC_APP_FREERTOS_IOT_IBABY_SMARTHOME_MULTINODE_WEARABLE_NODE
 * \brief	emsk timer1 for ibaby wearable node
 */

/**
 * \addtogroup	EMBARC_APP_FREERTOS_IOT_IBABY_SMARTHOME_MULTINODE_WEARABLE_NODE
 * @{
 */
/* standard C HAL */
#include <stdio.h>

/* embARC HAL */
#include "arc.h"
#include "arc_builtin.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"

#include "embARC.h"
#include "embARC_debug.h"
#include "dev_iic.h"
#include "board.h"

#include "common.h"
#include "timer1.h"


// #define T1_COUNT_UNIT_MS  (20000) /* counting accuracy of Timer1: 1ms */
#define T1_COUNT_UNIT_LMS (2000) /* counting accuracy of Timer1: 0.1ms */
static void timer1_isr(void *ptr);


/** arc timer 1 interrupt routine */
static void timer1_isr(void *ptr)
{
	timer_int_clear(TIMER_1);
	t1_cnt++;
}

/** software timing used timer1 interrupt start */
extern void timer1_start(void)
{
	t1_cnt = 0;

	int_handler_install(INTNO_TIMER1, timer1_isr);
	int_enable(INTNO_TIMER1);
	// timer_start(TIMER_1, TIMER_CTRL_IE, T1_COUNT_UNIT_MS);
	timer_start(TIMER_1, TIMER_CTRL_IE, T1_COUNT_UNIT_LMS);
}

/** software timing used timer1 stop */
extern void timer1_stop(void)
{
	uint32_t dec;

	timer_stop(TIMER_1);
	dec = t1_cnt - (t1_cnt / 10) * 10;

	EMBARC_PRINTF("************     timing     ************\r\n");
	EMBARC_PRINTF("* timer1 counter : %d.%d ms\r\n", t1_cnt / 10, dec);
	EMBARC_PRINTF("****************************************\r\n\r\n");
}

/** performance timer initialization */
extern void perf_init(void)
{
	if (timer_start(TIMER_1, TIMER_CTRL_NH, 0xFFFFFFFF) < 0) {
		EMBARC_PRINTF("perf timer init failed\r\n");
		while(1);
	}
}

/** performance timer start */
extern void perf_start(void)
{
	if (timer_current(TIMER_1, (void *)(&t1_start)) < 0) {
		t1_start = 0;
	}
}

/** performance timer end, and return the time passed */
extern uint32_t perf_end(void)
{
	uint32_t t1_end = 0;

	if (timer_current(TIMER_1, (void *)(&t1_end)) < 0) {
		return 0;
	}

	if (t1_start < t1_end) {
		return (t1_end - t1_start);
	} else {
		return (0xFFFFFFFF - t1_start + t1_end);
	}
}

/** @} end of group EMBARC_APP_FREERTOS_IOT_IBABY_SMARTHOME_MULTINODE_WEARABLE_NODE */