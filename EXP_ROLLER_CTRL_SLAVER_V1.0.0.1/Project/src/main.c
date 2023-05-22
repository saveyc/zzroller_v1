/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11/20/2009
  * @brief   Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SYSTEMTICK_PERIOD_MS  1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
uint32_t timingdelay;
u8 msone_flag = 0;
u16 upload_cnt = 0;


void main_init_process(void);
void main_onems_process(void);


//��ȡ��Դ��ѹ
float Get_Vcc_Volt(void)
{
    u16 adcx;
    float volt_t;
    
    adcx = Get_Adc(ADC_Channel_2);
    volt_t = ((float)adcx*(3.3/4096))*(15.0+2.0)/2.0;
    
    return volt_t;
}
//��ȡCPU�ڲ��¶�
float Get_Internal_Temperate(void)
{
    u16 adcx;
    float temperate;
    
    adcx = Get_Adc(ADC_Channel_16);
    temperate = (1.26-(float)adcx*(3.3/4096))/(-0.00423)+25;//������оƬ
    
    return temperate;
}


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    System_Setup();
    main_init_process();
    //X1_OUT(Bit_SET);
    //X2_OUT(Bit_SET);
    /* Infinite loop */
    while (1)
    {
        main_onems_process();

        //can.h
        can_send_frame_process();
    }
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: number of 10ms periods to wait for.
  * @retval None
  */
void Delay(uint32_t nCount)
{
    /* Capture the current local time */
    timingdelay = LocalTime + nCount;

    /* wait until the desired delay finish */
    while(timingdelay > LocalTime)
    {
    }
}

/**
  * @brief  Updates the system local time
  * @param  None
  * @retval None
  */
void Time_Update(void)
{
    LocalTime += SYSTEMTICK_PERIOD_MS;
    msone_flag = VALUE;
    upload_state_cnt++;

}

void main_init_process(void)
{
    //moto.h
    Mot_msg_init();

    //can
    InitCanSendQueue();

    //flash
    flash_init();

//    moto_set_run_cmd(MOT1,RUN_STATE,RUN_TRIGSTOP,RUN_BACK_TOAHEAD);
    moto_set_run_cmd(MOT1,RUN_STATE,RUN_TRIGSTOP,RUN_AHEAD_TOBACK);
//        moto_set_run_cmd(MOT2,RUN_STATE,RUN_TRIGSTOP,RUN_BACK_TOAHEAD);
//                moto_set_run_cmd(MOT2,RUN_STATE,RUN_TRIGSTOP,RUN_AHEAD_TOBACK);

}

void main_onems_process(void)
{
    if (msone_flag == VALUE) {
        msone_flag = INVALUE;

        //GPIO.H
        gpio_handleInputScan();
        gpio_Led_Blink_Handle();

        //moto.h
        Mot_Speed_Output_Handle();
        MOT_Error_Detection_Process();
        moto_upload_moudle_status();

        //flash
        flash_upload_write_buff();
        
          Mot_upload_all_state();
        
//       moto_set_run_cmd(MOT1,RUN_STATE,RUN_TRIGSTOP,RUN_BACK_TOAHEAD) 

    }
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {}
}
#endif


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
