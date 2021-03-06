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
 * \version 2017.06
 * \date 2017-06-26
 * \author dbHu(wwmhu@outlook.com)
--------------------------------------------- */

/**
 * \file
 * \ingroup	EMBARC_APP_FREERTOS_IOT_IBABY_SMARTHOME_MULTINODE_LAMP_NODE
 * \brief	lamp driver for ibaby lamp node
 */

/**
 * \addtogroup	EMBARC_APP_FREERTOS_IOT_IBABY_SMARTHOME_MULTINODE_LAMP_NODE
 * @{
 */
/* embARC HAL */
#include "embARC.h"
#include "embARC_debug.h"

/* custom HAL */
#include "lamp.h"


#define LED0 (0x01)
#define LED1 (0x02)
#define LED2 (0x04)
#define LED3 (0x08)
#define LED4 (0x10)
#define LED5 (0x20)
#define LED6 (0x40)
#define LED7 (0x80)

#define LED_ON  (0xff)
#define LED_OFF (0x0)


/**
 * \brief	initialize GPIO for lamp
 *              using led on emsk to simulate desk lamp
 */
extern void lamp_init(void)
{
	emsk_led_init();
}

/**
 * \brief	turn on lamp
 */
extern void lamp_on(void)
{
	led_write(LED_ON, LED0);
}

/**
 * \brief	turn off lamp
 */
extern void lamp_off(void)
{
	led_write(LED_OFF, LED0);
}

/** @} end of group EMBARC_APP_FREERTOS_IOT_IBABY_SMARTHOME_MULTINODE_LAMP_NODE */

