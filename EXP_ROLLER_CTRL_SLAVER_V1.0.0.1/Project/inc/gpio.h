#ifndef _GPIO_H
#define _GPIO_H

#include "main.h"


#define  INPUT_TRIG_NULL    0
#define  INPUT_TRIG_UP      1
#define  INPUT_TRIG_DOWN    2

#define  HIGH_LEVEL         1
#define  LOW_LEVEL          0

#define  MAX_ERR_CNT        5000

enum
{
    LED_STATUS = 0,
    LED_MOT1 = 1,
    LED_MOT2 = 2
};

enum
{
    LED_OFF = 0,//����
    LED_ON = 1, //����
    LED_B = 2,  //����(0.5s��,0.5s��)
    LED_BL = 3  //����(0.5s��,1.5s��)
};


#pragma pack (1) 

typedef struct {
    u8  input_state;
    u8  input_middle_state;
    u16 input_confirm_times;
    u8  input_trig_mode; //null,up,down
    u8  err;
    u8  level;
    u16  cnt;
}sInput_Info;

#pragma pack () 

extern sInput_Info input_in1_1;
extern sInput_Info input_in1_2;
extern sInput_Info input_in2_1;
extern sInput_Info input_in2_2;
extern sInput_Info input_x1;
extern sInput_Info input_x2;
extern sInput_Info input_mot1_alarm;
extern sInput_Info input_mot2_alarm;
extern sInput_Info input_dip_speed;
extern sInput_Info input_dip_dir;
extern sInput_Info input_dip_mod;

extern u16 led_blink_cnt;
extern u8  led_status_blk_mode;
extern u8  led_mot1_blk_mode;
extern u8  led_mot2_blk_mode;
extern u8  g_alarm_flag;//ȫ�ֱ���״̬(bit0:��ѹ����,bit1:��ѹ����)(�������������)

extern u8  localStation;

void gpio_handleInputScan();
void gpio_Led_Set_Mode(u8 led, u8 mode);
void gpio_Led_Blink_Handle(void);




#endif