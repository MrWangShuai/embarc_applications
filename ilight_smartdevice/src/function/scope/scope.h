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
 * \version 2017.09
 * \date 2017-09-27
 * \author Zhiwei Zhang(1812816853@qq.com)
--------------------------------------------- */

/**
 * \file
 * \ingroup EMBARC_APP_FREERTOS_IOT_ILIGHT_SMARTDEVICE
 * \brief virtual scope for iLight
 */

/**
 * \addtogroup  EMBARC_APP_FREERTOS_IOT_ILIGHT_SMARTDEVICE
 * @{
 */
#ifndef _SCOPE_H_
#define _SCOPE_H_

/*!< Initial the scope using uart0 */
/*!< Attention: only use without blueteeth */
extern uint32_t scope_init(uint32_t baudrate);

/*!< Output the date to scope */
extern uint32_t output_data(int16_t *out_data);

#endif /*_SCOPE_H_ */

/** @} end of group EMBARC_APP_FREERTOS_IOT_ILIGHT_SMARTDEVICE */