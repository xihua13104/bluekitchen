/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */

/* Includes ------------------------------------------------------------------*/
#include "exception_config.h"
#include "exception_handler.h"
#include "exception_portable.h"
#include "syslog.h"
#include "hal_core_status.h"
#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
#ifdef MTK_FOTA_VIA_RACE_CMD
#include "fota_multi_info.h"
#endif /* MTK_FOTA_VIA_RACE_CMD */
#endif /* EXCEPTION_MEMDUMP_MODE */


/* Public define -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* exception slaves' status */
exception_slave_status_t exceptionSlaveStatus[EXCEPTION_SLAVES_TOTAL] = {0};

/* exception slaves' configurations */
const exception_slaves_dump_t exceptionSlavesDump[EXCEPTION_SLAVES_TOTAL] =
{
    {"dsp0", exception_alert_dsp0, exception_check_status_dsp0, exception_dump_dsp0, exception_forceddump_dsp0, 1},
    // {0, 0, 0, 0, 0, 0}
};


/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
void exception_log_service_init(void)
{
    /* feed wdt to keep time for log service init */
    exception_feed_wdt();

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT) || (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_BINARY)
#if 1
    extern void exception_syslog_callback(void);
	/* Init uart polling mode and set syslog exception mode */
    exception_syslog_callback();
    //exception_mux_callback();
#endif /* !defined(MTK_SAVE_LOG_TO_FLASH_ENABLE) */
#endif /* (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT) || (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_BINARY) */
}

void exception_log_service_post_process(void)
{
    /* feed wdt to keep time for log service post process*/
    exception_feed_wdt();

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
#if 1
    extern void exception_syslog_offline_dump_callback(void);
	/* dump tx buffer to flash */
    exception_syslog_offline_dump_callback();
#endif /* MTK_SAVE_LOG_TO_FLASH_ENABLE */
#endif /* (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP) */
}

void exception_core_status_update(void)
{
    hal_core_status_write(HAL_CORE_CM4, HAL_CORE_EXCEPTION);
}

