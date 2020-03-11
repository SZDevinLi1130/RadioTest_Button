/**
 * Copyright (c) 2014-2018 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 *
 * @defgroup nrf_radio_test_example_main main.c
 * @{
 * @ingroup nrf_radio_test_example
 * @brief Radio Test Example application main file.
 *
 * This file contains the source code for a sample application that uses the NRF_RADIO and is controlled through the serial port.
 *
 */


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "bsp.h"
#include "nrf.h"
#include "radio_test.h"
#include "app_uart.h"
#include "app_error.h"
#include "nordic_common.h"
#include "nrf_drv_clock.h"
#include "nrf_cli.h"
#include "nrf_cli_uart.h"
#include "app_button.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define LEDBUTTON_BUTTON                25   
#define BUTTON_DETECTION_DELAY         APP_TIMER_TICKS(50)                     /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */
#define LO_FREQ                        1
#define MI_FREQ                        2
#define HI_FREQ                        3
static uint8_t  but_index=0;

NRF_CLI_UART_DEF(m_cli_uart_transport, 0, 64, 16);
NRF_CLI_DEF(m_cli_uart,
            "uart_cli:~$ ",
            &m_cli_uart_transport.transport,
            '\r',
            CLI_EXAMPLE_LOG_QUEUE_SIZE);


/**@brief Function for starting a command line interface that works on the UART transport layer.
 */
static void cli_start(void)
{
    ret_code_t ret;

    ret = nrf_cli_start(&m_cli_uart);
    APP_ERROR_CHECK(ret);
}


/**@brief Function for configuring UART for CLI.
 */
static void cli_init(void)
{
    ret_code_t ret;


    nrf_drv_uart_config_t uart_config = NRF_DRV_UART_DEFAULT_CONFIG;

    uart_config.pseltxd = TX_PIN_NUMBER;
    uart_config.pselrxd = RX_PIN_NUMBER;
    uart_config.hwfc    = NRF_UART_HWFC_DISABLED;
    ret                 = nrf_cli_init(&m_cli_uart, &uart_config, true, true, NRF_LOG_SEVERITY_INFO);
    APP_ERROR_CHECK(ret);
}


/**@brief Function for initializing logging.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(app_timer_cnt_get);

    APP_ERROR_CHECK(err_code);

}


/** @brief Function for configuring all peripherals used in this example.
 */
static void init(void)
{
    NRF_RNG->TASKS_START = 1;

#ifdef NVMC_ICACHECNF_CACHEEN_Msk
    NRF_NVMC->ICACHECNF  = NVMC_ICACHECNF_CACHEEN_Enabled << NVMC_ICACHECNF_CACHEEN_Pos;
#endif // NVMC_ICACHECNF_CACHEEN_Msk

    // Start 64 MHz crystal oscillator.
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART    = 1;

    // Wait for the external oscillator to start up.
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0)
    {
        // Do nothing.
    }
}

/**@brief Function for handling events from the button handler module.
 *
 * @param[in] pin_no        The pin that the event applies to.
 * @param[in] button_action The button action (press/release).
 */
static void button_event_handler(uint8_t pin_no, uint8_t button_action)
{
    ret_code_t err_code;
    if(button_action == APP_BUTTON_PUSH)
    {
      switch (pin_no)
      {
        case LEDBUTTON_BUTTON:
           // NRF_LOG_INFO("Send button state change.");
            but_index++;
            if(but_index == LO_FREQ)
            {
             // but_index = LO_FREQ;
              radio_unmodulated_tx_carrier(RADIO_TXPOWER_TXPOWER_0dBm, RADIO_MODE_MODE_Ble_1Mbit, 0);
              NRF_LOG_INFO("Freq. is 2402MHz");
            }
            else if(but_index == MI_FREQ)
            {
             // but_index = MI_FREQ;
              radio_unmodulated_tx_carrier(RADIO_TXPOWER_TXPOWER_0dBm, RADIO_MODE_MODE_Ble_1Mbit, 40);
              NRF_LOG_INFO("Freq. is 2440MHz");
            }
            else if(but_index == HI_FREQ)
            {
              //but_index = HI_FREQ;
              radio_unmodulated_tx_carrier(RADIO_TXPOWER_TXPOWER_0dBm, RADIO_MODE_MODE_Ble_1Mbit, 80);
              NRF_LOG_INFO("Freq. is 2480MHz");
              but_index = 0;
            }
            else 
              but_index = 0;                        
            break;

        default:
            APP_ERROR_HANDLER(pin_no);
             but_index = 0;  
            break;
      }
    }
}


/**@brief Function for initializing the button handler module.
 */
static void buttons_init(void)
{
    ret_code_t err_code;
    //The array must be static because a pointer to it will be saved in the button handler module.
    static app_button_cfg_t buttons[] =
    {
        {LEDBUTTON_BUTTON, false, BUTTON_PULL, button_event_handler}
    };

    err_code = app_button_init(buttons, ARRAY_SIZE(buttons),
                               BUTTON_DETECTION_DELAY);
    APP_ERROR_CHECK(err_code);
}



/** @brief Function for the main application entry.
 */
int main(void)
{
    uint32_t err_code;

    log_init();

    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);

    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    init();
    cli_init();
    cli_start();

    NVIC_EnableIRQ(TIMER0_IRQn);
    __enable_irq();
   
    NRF_LOG_RAW_INFO("Radio test example started.\r\n");
    buttons_init();
    err_code = app_button_enable();    
    APP_ERROR_CHECK(err_code);
    while (true)
    {
        UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
        nrf_cli_process(&m_cli_uart);
    }
}


/** @} */