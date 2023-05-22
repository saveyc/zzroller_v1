#ifndef _LOGIC_H
#define _LOGIC_H


#define  RUN_STATE              1
#define  STOP_STATE             2
#define  RUN_STATE_DEFAULT      0 

enum
{
    MOT1 = 1,
    MOT2 = 2
};

#define  VALUE                 1
#define  INVALUE               0


#define  B_LIMIT_SW_1_IN_STATE IN1_0_STATE   //接近开关1输入
#define  B_LIMIT_SW_2_IN_STATE IN1_1_STATE   //接近开关2输入

#define  B_PHOTO_1_IN_STATE    IN2_0_STATE   //光电1输入(后)
#define  B_PHOTO_2_IN_STATE    IN2_1_STATE   //光电2输入(前)
#define  B_PHOTO_3_IN_STATE    IN2_2_STATE   //光电3输入(右)
#define  B_PHOTO_4_IN_STATE    IN2_3_STATE   //光电4输入(左)

#define  B_RUN_1_OUT_(BitVal)   OUT_1_0_(BitVal)   //滚筒1启动
#define  B_DIR_1_OUT_(BitVal)   OUT_1_1_(BitVal)   //滚筒1方向
#define  B_RUN_2_OUT_(BitVal)   OUT_1_2_(BitVal)   //滚筒2启动
#define  B_DIR_2_OUT_(BitVal)   OUT_1_3_(BitVal)   //滚筒2方向
#define  B_RUN_3_OUT_(BitVal)   OUT_1_4_(BitVal)   //滚筒3启动
#define  B_DIR_3_OUT_(BitVal)   OUT_1_5_(BitVal)   //滚筒3方向

#define  INPUT_TRIG_NULL    0
#define  INPUT_TRIG_UP      1
#define  INPUT_TRIG_DOWN    2

#define  HIGH_LEVEL         1
#define  LOW_LEVEL          0

#define  MAX_ERR_CNT        5000

typedef struct {
    u16 set_start_status;         //设置的启停状态
    u16 set_start_type;           //设置的启停类型   遇到光电立马停止 还是继续运行
    u16 roll_dir_set;             //旋转方向
    u16 uploadcnt;
}sMoudleinfo;

typedef struct {
    u16  input_state;
    u16  input_middle_state;
    u16 input_confirm_times;
    u16  input_trig_mode; //null,up,down
    u16  err;
    u16  level;
    u16  cnt;
}sInput_Info;

typedef struct
{
    u8  value;                  //是否有效
    u8  cmd;                    //启停命令
    u8  dir;                    //方向类型
    u8  type;                   //转动类型 遇到光电就停 还是一直转
}sMoudle_cmd;

enum
{
    POS_INIT  = 0,
    POS_UP    = 1,
    POS_DOWN  = 2,
    POS_ERROR = 3
};

enum
{
    DIR_FW = 0,
    DIR_BK = 1,
    DIR_LF = 2,
    DIR_RT = 3
};

extern sInput_Info input_in2_0;
extern sInput_Info input_in2_1;
extern sInput_Info input_in2_2;
extern sInput_Info input_in2_3;

extern sInput_Info input_in1_0;
extern sInput_Info input_in1_1;


extern u16 canrecv_framecnt_rise ;

extern u8 g_position_status;//顶升机构状态
extern u8 g_position_set;//设置的位置
extern u8 g_position_find_flag;
extern u16 g_position_find_timeout;

extern sMoudleinfo moudle1_info, moudle2_info;

extern sMoudle_cmd moudlerisecmd;


void InputScanProc();
void Init_Find_Position_Process(void);
void Select_Direction_Run(u8 direction);
void Select_Direction_Stop(u8 direction);
void moto_upload_moudle_status(void);
void moto_set_run_cmd(u8 motX, u8 cmd, u8 type, u8 dir);
void moto_roll_test_process(void);
void moto_ctrl_convey_process(void);
void moto_upload_moto_state(void);

#endif
