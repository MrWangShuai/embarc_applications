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
 * \date 2017-07-11
 * \author Xiangcai Huang(xiangcai@synopsys.com)
--------------------------------------------- */

/**
 * \defgroup	EMBARC_APP_FREERTOS_IBABY_SMARTHOME_NODES_WEARABLE_NODE		embARC iBaby Smarthome Node Wearable Node
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_OS_FREERTOS
 * \ingroup	EMBARC_APPS_MID_LWIP
 * \ingroup	EMBARC_APPS_MID_WAKAAMA
 * \ingroup	EMBARC_APPS_MID_FATFS
 * \brief	embARC iBaby Smarthome Node Wearable Node
 * \details
 *		Implement all function of iBaby wearable node, including data acquisition, processing and 
 *    interaction with iBaby Smarthome Gateway(LwM2M Server) as LwM2M Client.
 *
 *      Manual function including body temperature detecting, heartrate detecting and posture, 
 *    motion intensity, awake event detecting in real time during sleep.
 */

/**
 * \file
 * \ingroup	EMBARC_APP_FREERTOS_IBABY_SMARTHOME_NODES_WEARABLE_NODE
 * \brief	Functional Task for iBaby Wearable Node
 */

/**
 * \addtogroup	EMBARC_APP_FREERTOS_IBABY_SMARTHOME_NODES_WEARABLE_NODE
 * @{
 */
/* custom HAL */
#include "task_function.h"

/*
**************************************************************
*  This part will be deleted in release version
*/
#if USED_TIMER1
/** arc timer 1 interrupt routine */
static void timer1_isr(void *ptr)
{
	timer_int_clear(TIMER_1);
	t1_count++;
}

/** print message for debug major function */
static void timer1_start(void)
{
	t1_count = 0;

	int_handler_install(INTNO_TIMER1, timer1_isr);
	int_enable(INTNO_TIMER1);
	timer_start(TIMER_1, TIMER_CTRL_IE, T1_COUNT_UNIT_LMS);
}

/** print message for debug major function */
static void timer1_stop(void)
{
	uint32_t dec;

	timer_stop(TIMER_1);
	dec = t1_count - (t1_count/10) * 10;

	EMBARC_PRINTF("************     timing     ************\r\n");
	EMBARC_PRINTF("* timer1 counter : %d.%d ms\n", t1_count/10, dec);
	EMBARC_PRINTF("****************************************\r\n\r\n");
}
#endif/* USED_TIMER1 */
/*
*  end of this part
**************************************************************
*/


#if PRINT_DEBUG_FUNC
/** print message for primary function */
static void print_msg_func(void)
{
	char str[150];

	EMBARC_PRINTF("\n************ Primary function ************\r\n");
	sprintf(str,
		"* Body heartrate   : %dbpm\r\n* Body temperature : %d.%d'C\r\n* Motion intensity : %d\r\n", 
		 data_report_wn.hrate,
		 data_report_wn.btemp/10, data_report_wn.btemp%10,
		 data_report_wn.motion_intensity);
	EMBARC_PRINTF(str);
	
	if (data_report_wn.state == SLEEP)
		EMBARC_PRINTF("* State : sleep\r\n");
	else
		EMBARC_PRINTF("* State : wake\r\n");

	if (data_report_wn.warn_btemp)
		EMBARC_PRINTF("* Warn of abnormal body temperature\r\n");

	if (data_report_wn.warn_hrate)
		EMBARC_PRINTF("* Warn of abnormal body heartrate\r\n");

	if (data_report_wn.warn_downward)
		EMBARC_PRINTF("* Warn of sleep on his stomach\r\n");

	if (data_report_wn.event_awake == AWAKE)
		EMBARC_PRINTF("* Baby awake!\r\n");
}
#endif /* PRINT_DEBUG_FUNC */

#if PRINT_DEBUG_AWAKE
/** print message for debug awake event detecting function */
static void print_msg_awake(void)
{
	char str[50];

	EMBARC_PRINTF("\n************ Awake detecting ************\r\n");
	sprintf(str, "* Motion intensity in 5s : %d\n", inten_aw);
	EMBARC_PRINTF(str);

	for (uint i = 0; i < LEN_STA_QUEUE; ++i)
	{
		if (i!=0 && !(i%3))
			EMBARC_PRINTF("\n");
		
		if (state_aw[i])
		{
			sprintf(str, "* state %d : sleep\t", i);
			EMBARC_PRINTF(str);
		}else{
			sprintf(str, "* state %d : wake\t", i);
			EMBARC_PRINTF(str);
		}
	}
	EMBARC_PRINTF("\n");
	if (data_report_wn.event_awake == AWAKE)
		EMBARC_PRINTF("* Baby awake!\n");
}
#endif /* PRINT_DEBUG_AWAKE */

#if PRINT_DEBUG_SLEEP
/** print message for debug Sleep-Wake state monitoring function */
static void print_msg_sleep(uint state)
{
	char str[50];

	EMBARC_PRINTF("\n************ Sleep monitoring ************\r\n");
	for (uint i = 0; i < 5; ++i)
	{
		if (i==2 || i==3)
			EMBARC_PRINTF("\n");
		sprintf(str, "* Intensity %d : %d\t\t", i, inten_sl[i]/100);
		EMBARC_PRINTF(str);
	}
	sprintf(str, "\n* Intensity score in 5min : %f\n", score_sl);
	EMBARC_PRINTF(str);
	if (!state)
		EMBARC_PRINTF("* State of 2min ago : wake\r\n\r\n");
	else
		EMBARC_PRINTF("* State of 2min ago : sleep\r\n\r\n");
}
#endif /* PRINT_DEBUG_SLEEP */

/* function for deal with heartrate by filter */
static void process_hrate(uint32_t* hrate)
{
	/* ignore the wrong data in the beginning */
	if(dat_num < 9)
			hrate_sensor_read(NULL);
	else if(dat_num < FFT_LEN + 9)
	{
		hrate_sensor_read(&hrate_group[dat_num-9]);
		if(dat_rdy && dat_num > 9)
		{
			//EMBARC_PRINTF("hrate : %d\n", hrate_group[dat_num-10]);
			if(hrate_group[dat_num-10] < 10000 || hrate_group[dat_num-10] > 120000)
			{
				dat_num = 0;
				sum_h = 0;
				cnt_h = 0;
			}
			else 
			{
				sum_h += hrate_group[dat_num-10];
			}
			dat_rdy = 0;
		}
	}
	else if(dat_num == FFT_LEN + 9)
	{
		sum_h = sum_h / FFT_LEN;
		for(int i = 0; i < FFT_LEN - 1; i++)
		{
			if(fabs(hrate_group[i] - hrate_group[i+1]) > 1000)
			{
				flag_h = 1;
				dat_num = 0;
				cnt_h = 0;
				hrate_temp = 0;
			}
		}
		if(!flag_h)
		{
			for(int i = 0; i < FFT_LEN; i++)
			{	
				hrate_group[i] = (int)band_pass(hrate_group[i] - sum_h);
				//EMBARC_PRINTF("hrate_fir : %d\n", hrate_group[i]);
				if(fabs(hrate_group[i]) < 1000)
				{
					x[i].R = hrate_group[i] * 30;
				}
				else 
				{
					i = FFT_LEN - 1;
					dat_num = 0;
					cnt_h = 0;
					hrate_temp = 0;
				}

			}
		}	

		if(dat_num)
		{
			calc_w(w);
			fft(x,w);
			if(cnt_h > 0)
				hrate_temp = hrate_temp + ((float)(find_max(x) * 60 * FFT_DELTA) * 10 - hrate_temp) / 6;
			else if(cnt_h == 0)
			{
					cnt_h++;
					hrate_temp = 700;

			}
			dat_num = 0;

		}
		sum_h = 0;
		flag_h = 0;

		// EMBARC_PRINTF("hrate_temp : %d cnt ；%d\n", hrate_temp,cnt_h);
	}

	*hrate = (uint32_t)(hrate_temp/10); 

	// EMBARC_PRINTF("hrate_temp : %d\n", hrate_temp);
}

/* function for deal with acclerate by filter */
static int filter_acc(int val_new, int val_old, 
	bool *flag_old, char *cnt, unsigned char *par)
{
	int val_diff;
	bool flag_new;

	/* calculate low pass filter coefficient for x axis acceleration */ 
	val_diff = val_new - val_old;
	if (val_diff > 0)
		flag_new = true;
	else
		flag_new = false;
	if (flag_new == *flag_old)
	{
		(*cnt)++;// notify: must type () on both sides of "*cnt", or it will give you wrong result
		if (fabs(val_diff) > THOLD_ACC_DIFF)
			*cnt += CNT_ACC_STEP;
		if (*cnt > THOLD_ACC_CNT)
		{
			*par += PAR_ACC_STEP;
			*cnt = 0;
		}
	}else{
		*cnt  = 0;
		*par = PAR_ACC_BASE;
	}
	*flag_old = flag_new;

	/* calculate output of low pass filter */
	if (!val_diff)
		val_new = val_old;
	else
		val_new = val_old + val_diff*(*par)/256;

	return val_new;
}

/* function for deal with SVM by filter */
static int filter_svm(int val_new, int val_old, 
	bool *flag_old, char *cnt, unsigned char *par)
{
	int val_diff;
	bool flag_new;

	/* deal with SVM by limiting filter */
	if (val_new > MAX_LIMIT_SVM)
		val_new = MAX_LIMIT_SVM;

	/* deal with SVM by low pass filter */
	val_diff = val_new - val_old;
	if (val_diff)
	{
		/* calculate low pass filter coefficient for SVM */
		if (val_diff > 0)
			flag_new = true;
		else
			flag_new = false;
		if (flag_new == *flag_old)
		{
			(*cnt)++;// notify: must type () on both sides of "*cnt", or it will give you wrong result
			if (fabs(val_diff) > THOLD_SVM_DIFF)
				*cnt += CNT_SVM_STEP;

			if (*cnt > THOLD_SVM_CNT)
			{
				*par += PAR_SVM_STEP;
				*cnt = 0;
			}
		}else{
			*cnt = 0;
			*par = PAR_SVM_BASE;
		}
		*flag_old = flag_new;
	}else{
		*cnt = 0;
		*par = PAR_SVM_BASE;
	}

	/* calculate output of low pass filter */
	if (!val_diff)
		val_new = val_old;
	else
		val_new = val_old + val_diff*(*par)/256;

	/* deal with SVM by limiting filter */
	if (val_new < MIN_LIMIT_SVM)
		val_new = 0;

	return val_new;
}

/** function for processing accelerate raw data */
static int process_acc(acc_values acc_temp)
{
	int  x_new, y_new, z_new;   /* latest value */
	int  svm_new;               /* SVM : signal vector magnitude for difference */

	x_new = (int)(100*acc_temp.accl_x);
	y_new = (int)(100*acc_temp.accl_y);
	z_new = (int)(100*acc_temp.accl_z);

	/* deal with raw accelerate data by filter */
	x_new = filter_acc(x_new, x_old, &flag_old_x, &cnt_x, &par_x);
	y_new = filter_acc(y_new, y_old, &flag_old_y, &cnt_y, &par_y);
	z_new = filter_acc(z_new, z_old, &flag_old_z, &cnt_z, &par_z);

	/* calculate SVM using data fusion */
	svm_new = sqrt((x_new - x_old)*(x_new - x_old) + 
		(y_new - y_old)*(y_new - y_old) + (z_new - z_old)*(z_new - z_old));

	x_old = x_new;
	y_old = y_new;
	z_old = z_new;

/*
**************************************************************
*  This part will be deleted in release version
*/
 	/* print out message for debug */
	#if SEND_DEBUG_SVM1_5S
	char str[50];
	sprintf(str, "%d.", svm_new);//send data to matlab
	EMBARC_PRINTF(str);
	#endif
/*
*  end of this part
**************************************************************
*/

	/* deal with SVM by filter */
	svm_new = filter_svm(svm_new, svm_old, &flag_old_v, &cnt_v, &par_v);

	svm_old = svm_new;

/*
**************************************************************
*  This part will be deleted in release version
*/
	/* print out message for debug */
	#if SEND_DEBUG_SVM2_5S
	char str[50];
	sprintf(str, "%d.", svm_new);//send data to matlab
	EMBARC_PRINTF(str);
	#endif
/*
*  end of this part
**************************************************************
*/

	return svm_new;
}

/** function for awake event detecting */
static uint func_detect_awake(int inten_temp)
{
	bool flag_break_aw = false;
	uint cnt_sl_aw = 0;
	uint cnt_wk_aw = 0;
	uint event = NOEVENT;/* flag of event : NOEVENT or AWAKE */

	/* judge current status */
	if (inten_temp > THOLD_INTEN_AW)
		state_aw[0] = 0;//now in wake state
	else
		state_aw[0] = 1;//now in sleep state

	/* the number of sleep state in front of the state queue */
	for (uint i = LEN_STA_QUEUE-1; i > LEN_STA_WAKE-1; --i)
	{
		if (flag_break_aw)
		{
			flag_break_aw = false;
			break;
		}
		if (state_aw[i])
			cnt_sl_aw++;
		if (cnt_sl_aw > LEN_STA_SLEEP * PAR_STA_AW)
		{
			cnt_sl_aw = 0;
			/* statistics the number of wake state in back of the state queue */
			for (uint i = 0; i < LEN_STA_WAKE; ++i)
			{
				if (!state_aw[i])
					cnt_wk_aw++;

				if(cnt_wk_aw > LEN_STA_WAKE * PAR_STA_AW){
					/* wake up event detected */
					event = AWAKE;
					/* go exit from for loop */
					flag_break_aw = true;
				}else{
					event = NOEVENT;
				}
			}
		}else{
			event = NOEVENT;
		}
	}
	cnt_sl_aw = 0;
	cnt_wk_aw = 0;

	/* print out message for debug */
	#if PRINT_DEBUG_AWAKE
	print_msg_awake();
	#endif

	/* update motion intensity for LEN_STA_QUEUE * 5s */
	for (uint i = LEN_STA_QUEUE-1; i > 0; --i)
	{
		state_aw[i] = state_aw[i-1];
	}

	return event;
}

/** function for sleep-wake state detecting */
static uint func_detect_state(int inten_temp)
{
	uint state; /* state : SLEEP or WAKE */

	/* print out message for debug */
	#if SEND_DEBUG_INTEN_1M
	char str[50];
	sprintf(str, "%d.", inten_temp);//send data to matlab
	EMBARC_PRINTF(str);
	#endif

	inten_sl[0] = inten_temp;/* motion intensity in 1min */

	/* calculate the score of 2min ago(inten_sl[2]) by Webster sleep-wake determine method */
	score_sl = P * (K4*inten_sl[4] + K3*inten_sl[3] + K2*inten_sl[2] + K1*inten_sl[1] + K0*inten_sl[0]) / 100;

	/* determine the state of 2min ago by score */
	if (score_sl > 1)
		state = WAKE;
	else
		state = SLEEP;

	/* print out message for debug */
	#if PRINT_DEBUG_SLEEP
	print_msg_sleep(state);
	#endif

	/* update motion intensity for 5 * 1min */
	for (uint i = 4; i > 0; --i)
		inten_sl[i] = inten_sl[i-1];

	return state;
}

/** function for sleep downward state detecting */
static bool func_detect_downward(float acc_temp)
{
	bool warn;

	if (acc_temp < WARN_ACCL_Z)
		warn = true;
	else
		warn = false;

	return warn;
}



#if LWM2M_CLIENT
/** task for lwm2m client */
static void task_lwm2m_client(void *par)
{
	unsigned int cpu_status;
	
	while (1) {
		lwm2m_client_conn_stat = 1;

		/* get into sub function(lwm2mclient) to do all work about data interaction with gateway(lwm2m server) */
		if (lwm2mclient(&c_info) == 0) 
			EMBARC_PRINTF("LwM2M client end successfully\r\n");
		else
			EMBARC_PRINTF("Error: LwM2M client end failed\r\n");

		cpu_status = cpu_lock_save();
		if (c_info.server != NULL)     free((void *)(c_info.server));
		if (c_info.ep_name != NULL)    free((void *)(c_info.ep_name));
		if (c_info.serverPort != NULL) free((void *)(c_info.serverPort));
		lwm2m_client_conn_stat = 0;
		cpu_unlock_restore(cpu_status);

		vTaskSuspend(task_lwm2m_client_handle);
	}
}
/** function for initialize and start lwm2m client */
static int lwm2m_client_start(void)
{
	int c_quit = 0;
	task_lwm2m_client_handle = 0;
	
	/* check to see if wifi works */
    if (!lwip_pmwifi_isup()) {
		EMBARC_PRINTF("Error: Wifi is not ready for lwM2M client.\r\n");
		goto error_exit;
    }
   
    /* exit lwm2mClient */
    if (c_quit == 1) {
		EMBARC_PRINTF("Error: Try to exit existing client.\r\n");
		handle_sigint(2);
		goto error_exit;
	}
	
	if (lwm2m_client_conn_stat == 1) {
		EMBARC_PRINTF("Error: LwM2M client is already running.\r\n");
		goto error_exit;
	}
    
    if (p_server == NULL) {
		EMBARC_PRINTF("Error: Server Url is not specified, please check it.\r\n");
		goto error_exit;
	}
    c_info.server = p_server;
	c_info.ep_name = p_client_name;
	c_info.serverPort = p_port;

    lwm2m_client_start_flag = 1;
	EMBARC_PRINTF("Start lwm2m client.\n"); 

	/* create or resume task for lwm2mClient to realize communication with iBaby Smarthome Gateway */
    if (xTaskCreate(task_lwm2m_client, "lwm2m client", STACK_DEPTH_LWM2M, NULL, TSKPRI_HIGH, 
		&task_lwm2m_client_handle) != pdPASS){
		EMBARC_PRINTF("Error: Create task_lwm2m_client failed\r\n");
		return E_SYS;
	}

    else {
		vTaskResume(task_lwm2m_client_handle);
		return E_OK;
	}

	return E_OK;

	error_exit:
	return E_SYS;
}
#endif/* LWM2M_CLIENT */

/** task for major function */
extern void task_function(void * par)
{
	#if LWM2M_CLIENT
	/* try to start lwm2m client */
	lwm2m_client_start();
	#endif

/*
**************************************************************
*  This part will be deleted in release version
*/
	vTaskDelay(DELAY_TIME_SLICE * 10); 

	/*  initialize accelerometer before read */
	acc_sensor_init(IMU_I2C_SLAVE_ADDRESS);
	vTaskDelay(DELAY_TIME_SLICE * 2); 

	/* initialize heartrate sensor before read */
	hrate_sensor_init(HEART_RATE_I2C_SLAVE_ADDRESS);
	vTaskDelay(DELAY_TIME_SLICE * 2);
/*
*  end of this part
**************************************************************
*/
	
	for(;;) {
/*
**************************************************************
*  This part will be deleted in release version
*/
		/* 
		 * start timer1 for calculating the time of task running 
		 */
		#if USED_TIMER1
		timer1_start();
		#endif
/*
*  end of this part
**************************************************************
*/

		/* read acceleration data every 33ms */
		acc_sensor_read(&acc_vals);

		/* process raw data and calculate SVM(representation of motion intensity) in 33ms */
		svm_val = process_acc(acc_vals);

		/*
		 * awake event detecting algorithm
		 */
		if (cnt_aw < THOLD_CNT_AW)
		{
			/* summation of SVM in 5s */
			inten_aw += svm_val;
			cnt_aw++;
		}else{
			/* remove the error value in the beginning */
			if (!flag_start_aw)
			{
				inten_aw = 0;
				flag_start_aw = true;
			}

			/* detect awake event */
			data_report_wn.event_awake = func_detect_awake(inten_aw);

			/* 
			 * print out messages for primary function 
			 */
			#if PRINT_DEBUG_FUNC
				print_msg_func();
			#endif/* PRINT_DEBUG_FUNC */

			inten_aw = 0;
			cnt_aw = 0;
		}

		/*
		 * sleep monitoring algorithm based on indigital integration method 
		 */
		if (cnt_sl < THOLD_CNT_SL)
		{
			/* summation of SVM in 1 min */
			data_report_wn.motion_intensity += svm_val;
			cnt_sl++;
		}else{
			/* remove the error value in the beginning */
			if (!flag_start_sl)
			{
				data_report_wn.motion_intensity = 0;
				flag_start_sl = true;
			}

			/* sleep-wake state monitoring */
			data_report_wn.state = func_detect_state(data_report_wn.motion_intensity);

			data_report_wn.motion_intensity = 0;
			cnt_sl = 0;
		}


		/* initialize the flag_warn */
		data_report_wn.warn_hrate    = false;
	    data_report_wn.warn_btemp    = false;
	    data_report_wn.warn_downward = false;

	    /*
		 * detect warn of sleep on his stomach
		 */
		/* detect sleep downward event */
		data_report_wn.warn_downward = func_detect_downward(acc_vals.accl_z);


		/* read body temperature data */
		btemp_sensor_read(&data_report_wn.btemp);

		if (data_report_wn.btemp > WARN_BTEMP_H || data_report_wn.btemp < WARN_BTEMP_L)
			data_report_wn.warn_btemp = true;


		/* read heartrate data and process them by fft and filter */
		process_hrate(&data_report_wn.hrate);

		if (data_report_wn.hrate < WARN_HR_MIN || data_report_wn.hrate > WARN_HR_MAX)
			data_report_wn.warn_hrate = true;
	

		vTaskDelay(DELAY_TIME_SLICE); 

/*
**************************************************************
*  This part will be deleted in release version
*/
		/* 
		 * stop timer1 and print out the time of task running 
		 */		
		#if USED_TIMER1
		timer1_stop();
		#endif
/*
*  end of this part
**************************************************************
*/

	}

}

/** @} */

