#ifndef _MOTO_H
#define _MOTO_H

#include "main.h"


#define  MAX_SPEED_CCR_VALUE    7660  //10V(��Ӧ���ת��)
#define  MIN_SPEED_CCR_VALUE    1838  //2.4V(��Ӧ��Сת��)
#define  TEN_MS_RAMP_CCR_VALUE  145   //0.19V(�Ӽ����¶ȿ���ʱÿ10ms������ֵ)


#define  RUN_STATE              1
#define  STOP_STATE             2
#define  RUN_STATE_DEFAULT      0 

enum
{
    MOT1 = 1,
    MOT2 = 2
};

enum
{
    DIR_CW = 0,
    DIR_CCW = 1
};

typedef struct {
    u16 value;                    //�Ƿ���Ч
    u16 set_start_status;         //���õ���ͣ״̬(0:ֹͣ 1:����)
    u16 set_speed_sel;            //���õ��ٶȵ�λ(0~7)(3.96V~10V)
    u16 set_dir_sel;              //���õķ���(0:CW 1:CCW)
    u16 set_ramp_sel;             //���õļӼ��ٵ�λ(0~4)(0:no ramp)
    u16 current_speed_ccr_value;  //��ǰ�ٶ����ռ�ձ�ֵ
    u16 target_speed_ccr_value;   //Ŀ���ٶ����ռ�ձ�ֵ(��Ҫ������ٶ�(����ֹͣ))
    u16 set_speed_ccr_value;      //���õ��ٶ����ռ�ձ�ֵ(����ʱ���ٶ�)
    u16 ramp_ccr_value;           //�Ӽ���ÿ10ms������ռ�ձ�ֵ
    u16 alarm_flag;               //�������״̬
    u16 opposite_dir;             //���÷���ķ�����
}MOT_Info;

typedef struct {
    u16 set_start_status;         //���õ���ͣ״̬
    u16 set_start_type;           //���õ���ͣ����   �����������ֹͣ ���Ǽ�������
    u16 roll_dir_set;             //��ת����
    u16 uploadcnt;
}sMoudleinfo;

extern MOT_Info mot1_info, mot2_info;
extern sMoudleinfo moudle1_info, moudle2_info;

extern u16 cansend_framecnt_one ;
extern u16 cansend_framecnt_two ;

extern u16 upload_state_cnt ;


void Mot_msg_init(void);
void Set_Mot_Roll_Paras(u8 motX, u8 speed_sel, u8 dir_sel, u8 ramp_sel);
void Mot_Set_Start_Cmd(u8 motX, u8 cmd);
void Mot_Speed_Output_Handle(void);
void MOT_Error_Detection_Process(void);
void Mot_upload_all_state(void);
void moto_upload_moudle_status(void);
void moto_set_run_cmd(u8 motX, u8 cmd, u8 type, u8 dir);

#endif