#include "main.h"


sInput_Info input_in2_0;
sInput_Info input_in2_1;
sInput_Info input_in2_2;
sInput_Info input_in2_3;

sInput_Info input_in1_0;
sInput_Info input_in1_1;


u8 g_position_status = POS_INIT;//顶升机构状态
u8 g_position_set = POS_DOWN;//设置的位置
u8 g_position_find_flag = 0;
u16 g_position_find_timeout = 0;

u16 roll_test = 0;
u8  roll_flag = 0;

sMoudleinfo moudle1_info, moudle2_info;
sMoudle_cmd moudlerisecmd;


u16 canrecv_framecnt_rise = 0xFFFF;

/* scan every 1ms */
void InputScanProc()
{
    // handle IN2-0 single
    input_in2_0.err = INVALUE;
    if (input_in2_0.input_state != IN2_0_STATE
        && input_in2_0.input_confirm_times == 0)
    {
        input_in2_0.input_middle_state = IN2_0_STATE;
    }
    if (input_in2_0.input_middle_state == IN2_0_STATE
        && input_in2_0.input_middle_state != input_in2_0.input_state)
    {
        input_in2_0.input_confirm_times++;
        if (input_in2_0.input_confirm_times > 10)
        {
            input_in2_0.input_state = input_in2_0.input_middle_state;
            input_in2_0.input_confirm_times = 0;
            if (input_in2_0.input_state == 1)
            {
                input_in2_0.input_trig_mode = INPUT_TRIG_UP;
                input_in2_0.level = HIGH_LEVEL;
                moto_upload_moto_state();
            }
            else
            {
                input_in2_0.input_trig_mode = INPUT_TRIG_DOWN;
                input_in2_0.level = LOW_LEVEL;
                input_in2_0.cnt = 0;
            }
        }
    }
    else
    {
        input_in2_0.input_middle_state = input_in2_0.input_state;
        input_in2_0.input_confirm_times = 0;
    }
    if (input_in2_0.level == HIGH_LEVEL) {
        input_in2_0.cnt++;
        if (input_in2_0.cnt >= MAX_ERR_CNT) {
            input_in2_0.cnt = MAX_ERR_CNT;
            input_in2_0.err = VALUE;
        }
    }

    // handle IN2-1 single
    input_in2_1.err = INVALUE;
    if (input_in2_1.input_state != IN2_1_STATE
        && input_in2_1.input_confirm_times == 0)
    {
        input_in2_1.input_middle_state = IN2_1_STATE;
    }
    if (input_in2_1.input_middle_state == IN2_1_STATE
        && input_in2_1.input_middle_state != input_in2_1.input_state)
    {
        input_in2_1.input_confirm_times++;
        if (input_in2_1.input_confirm_times > 10)
        {
            input_in2_1.input_state = input_in2_1.input_middle_state;
            input_in2_1.input_confirm_times = 0;
            if (input_in2_1.input_state == 1)
            {
                input_in2_1.input_trig_mode = INPUT_TRIG_UP;
                input_in2_1.level = HIGH_LEVEL;
                moto_upload_moto_state();
            }
            else
            {
                input_in2_1.input_trig_mode = INPUT_TRIG_DOWN;
                input_in2_1.level = LOW_LEVEL;
                input_in2_1.cnt = 0;
            }
        }
    }
    else
    {
        input_in2_1.input_middle_state = input_in2_1.input_state;
        input_in2_1.input_confirm_times = 0;
    }
    if (input_in2_1.level == HIGH_LEVEL) {
        input_in2_1.cnt++;
        if (input_in2_1.cnt >= MAX_ERR_CNT) {
            input_in2_1.cnt = MAX_ERR_CNT;
            input_in2_1.err = VALUE;
        }
    }


    // handle IN2-2 single
    input_in2_2.err = INVALUE;
    if (input_in2_2.input_state != IN2_2_STATE
        && input_in2_2.input_confirm_times == 0)
    {
        input_in2_2.input_middle_state = IN2_2_STATE;
    }
    if (input_in2_2.input_middle_state == IN2_2_STATE
        && input_in2_2.input_middle_state != input_in2_2.input_state)
    {
        input_in2_2.input_confirm_times++;
        if (input_in2_2.input_confirm_times > 10)
        {
            input_in2_2.input_state = input_in2_2.input_middle_state;
            input_in2_2.input_confirm_times = 0;
            if (input_in2_2.input_state == 1)
            {
                input_in2_2.input_trig_mode = INPUT_TRIG_UP;
                input_in2_2.level = HIGH_LEVEL;
                moto_upload_moto_state();
            }
            else
            {
                input_in2_2.input_trig_mode = INPUT_TRIG_DOWN;
                input_in2_2.level = LOW_LEVEL;
                input_in2_2.cnt = 0;
            }
        }
    }
    else
    {
        input_in2_2.input_middle_state = input_in2_2.input_state;
        input_in2_2.input_confirm_times = 0;
    }
    if (input_in2_2.level == HIGH_LEVEL) {
        input_in2_2.cnt++;
        if (input_in2_2.cnt >= MAX_ERR_CNT) {
            input_in2_2.cnt = MAX_ERR_CNT;
            input_in2_2.err = VALUE;
        }
    }


    // handle IN2-3 single
    input_in2_3.err = INVALUE;
    if (input_in2_3.input_state != IN2_3_STATE
        && input_in2_3.input_confirm_times == 0)
    {
        input_in2_3.input_middle_state = IN2_3_STATE;
    }
    if (input_in2_3.input_middle_state == IN2_3_STATE
        && input_in2_3.input_middle_state != input_in2_3.input_state)
    {
        input_in2_3.input_confirm_times++;
        if (input_in2_3.input_confirm_times > 10)
        {
            input_in2_3.input_state = input_in2_3.input_middle_state;
            input_in2_3.input_confirm_times = 0;
            if (input_in2_3.input_state == 1)
            {
                input_in2_3.input_trig_mode = INPUT_TRIG_UP;
                input_in2_3.level = HIGH_LEVEL;
                moto_upload_moto_state();
            }
            else
            {
                input_in2_3.input_trig_mode = INPUT_TRIG_DOWN;
                input_in2_3.level = LOW_LEVEL;
                input_in2_3.cnt = 0;
            }
        }
    }
    else
    {
        input_in2_3.input_middle_state = input_in2_3.input_state;
        input_in2_3.input_confirm_times = 0;
    }
    if (input_in2_3.level == HIGH_LEVEL) {
        input_in2_3.cnt++;
        if (input_in2_3.cnt >= MAX_ERR_CNT) {
            input_in2_3.cnt = MAX_ERR_CNT;
            input_in2_3.err = VALUE;
        }
    }


    // handle IN1-0 single
    input_in1_0.err = INVALUE;
    if (input_in1_0.input_state != IN1_0_STATE
        && input_in1_0.input_confirm_times == 0)
    {
        input_in1_0.input_middle_state = IN1_0_STATE;
    }
    if (input_in1_0.input_middle_state == IN1_0_STATE
        && input_in1_0.input_middle_state != input_in1_0.input_state)
    {
        input_in1_0.input_confirm_times++;
        if (input_in1_0.input_confirm_times > 10)
        {
            input_in1_0.input_state = input_in1_0.input_middle_state;
            input_in1_0.input_confirm_times = 0;
            if (input_in1_0.input_state == 1)
            {
                input_in1_0.input_trig_mode = INPUT_TRIG_UP;
                input_in1_0.level = HIGH_LEVEL;
            }
            else
            {
                input_in1_0.input_trig_mode = INPUT_TRIG_DOWN;
                input_in1_0.level = LOW_LEVEL;
                input_in1_0.cnt = 0;
            }
        }
    }
    else
    {
        input_in1_0.input_middle_state = input_in1_0.input_state;
        input_in1_0.input_confirm_times = 0;
    }
    if (input_in1_0.level == HIGH_LEVEL) {
        input_in1_0.cnt++;
        if (input_in1_0.cnt >= MAX_ERR_CNT) {
            input_in1_0.cnt = MAX_ERR_CNT;
            input_in1_0.err = VALUE;
        }
    }

    // handle IN1-1 single
    input_in1_1.err = INVALUE;
    if (input_in1_1.input_state != IN1_1_STATE
        && input_in1_1.input_confirm_times == 0)
    {
        input_in1_1.input_middle_state = IN1_1_STATE;
    }
    if (input_in1_1.input_middle_state == IN1_1_STATE
        && input_in1_1.input_middle_state != input_in1_1.input_state)
    {
        input_in1_1.input_confirm_times++;
        if (input_in1_1.input_confirm_times > 10)
        {
            input_in1_1.input_state = input_in1_1.input_middle_state;
            input_in1_1.input_confirm_times = 0;
            if (input_in1_1.input_state == 1)
            {
                input_in1_1.input_trig_mode = INPUT_TRIG_UP;
                input_in1_1.level = HIGH_LEVEL;
            }
            else
            {
                input_in1_1.input_trig_mode = INPUT_TRIG_DOWN;
                input_in1_1.level = LOW_LEVEL;
                input_in1_1.cnt = 0;
            }
        }
    }
    else
    {
        input_in1_1.input_middle_state = input_in1_1.input_state;
        input_in1_1.input_confirm_times = 0;
    }
    if (input_in1_1.level == HIGH_LEVEL) {
        input_in1_1.cnt++;
        if (input_in1_1.cnt >= MAX_ERR_CNT) {
            input_in1_1.cnt = MAX_ERR_CNT;
            input_in1_1.err = VALUE;
        }
    }
}

//上电检测初始化顶升机构位置
//顶升机构动作执行
void Init_Find_Position_Process(void)
{
    if(g_position_status == POS_INIT)//未初始化
    {
        if(B_LIMIT_SW_2_IN_STATE)//下位
        {
            g_position_status = POS_DOWN;
        }
        else if(B_LIMIT_SW_1_IN_STATE)//上位
        {
            g_position_status = POS_UP;
        }
        else
        {
            g_position_set = POS_DOWN;//初始化到下位
        }
    }
    if((g_position_status != g_position_set) && (g_position_find_flag == 0))
    {
        if(g_position_set == POS_DOWN)
        {
            B_RUN_3_OUT_(Bit_SET);
            B_DIR_3_OUT_(Bit_SET);
            g_position_find_flag = 1;
            g_position_find_timeout = 0;
        }
        else if(g_position_set == POS_UP)
        {
            B_RUN_3_OUT_(Bit_SET);
            B_DIR_3_OUT_(Bit_RESET);
            g_position_find_flag = 1;
            g_position_find_timeout = 0;
        }
    }
    if(g_position_find_flag)
    {
        if(g_position_set == POS_DOWN)//下位
        {
            if(B_LIMIT_SW_2_IN_STATE)
            {
                B_RUN_3_OUT_(Bit_RESET);
                B_DIR_3_OUT_(Bit_RESET);
                g_position_status = POS_DOWN;
                g_position_find_flag = 0;
                g_position_find_timeout = 0;
            }
            else
            {
                g_position_find_timeout++;
            }
        }
        else if(g_position_set == POS_UP)//上位
        {
            if(B_LIMIT_SW_1_IN_STATE)
            {
                B_RUN_3_OUT_(Bit_RESET);
                B_DIR_3_OUT_(Bit_RESET);
                g_position_status = POS_UP;
                g_position_find_flag = 0;
                g_position_find_timeout = 0;
            }
            else
            {
                g_position_find_timeout++;
            }
        }

        if(g_position_find_timeout > 2000)
        {
            B_RUN_3_OUT_(Bit_RESET);
            g_position_status = POS_ERROR;
            g_position_set = POS_ERROR;
            g_position_find_flag = 0;
        }
    }
}

//选择一个方向的滚筒运行
void Select_Direction_Run(u8 direction)
{
    if(direction == DIR_FW)//前
    {
        g_position_set = POS_UP;//前
        B_RUN_1_OUT_(Bit_SET);
        B_DIR_1_OUT_(Bit_SET);
    }
    else if(direction == DIR_BK)//后
    {
        g_position_set = POS_UP;//上
        B_RUN_1_OUT_(Bit_SET);
        B_DIR_1_OUT_(Bit_RESET);
    }
    else if(direction == DIR_LF)//左
    {
        g_position_set = POS_DOWN;//下
        B_RUN_2_OUT_(Bit_SET);
        B_DIR_2_OUT_(Bit_RESET);
    }
    else if(direction == DIR_RT)//右
    {
        g_position_set = POS_DOWN;//下
        B_RUN_2_OUT_(Bit_SET);
        B_DIR_2_OUT_(Bit_SET);
    }
}
//选择一个方向的滚筒停止
void Select_Direction_Stop(u8 direction)
{
    if(direction == DIR_FW)//前
    {
        B_RUN_1_OUT_(Bit_RESET);
    }
    else if(direction == DIR_BK)//后
    {
        B_RUN_1_OUT_(Bit_RESET);
    }
    else if(direction == DIR_LF)//左
    {
        B_RUN_2_OUT_(Bit_RESET);
    }
    else if(direction == DIR_RT)//右
    {
        B_RUN_2_OUT_(Bit_RESET);
    }
}


void moto_upload_moudle_status(void)
{
    //MOTO1
    moudle1_info.uploadcnt++;
    if ((moudle1_info.set_start_type == RUN_TRIGSTOP) && (moudle1_info.set_start_status == RUN_STATE)) {
        if ((input_in2_1.input_state == 1) && (moudle1_info.roll_dir_set == DIR_BK)) {
            Select_Direction_Stop(DIR_BK);//停止
            moudle1_info.set_start_status = STOP_STATE;
            moudle1_info.set_start_type = CONTINUE_RUN;
        }
        if ((input_in2_0.input_state == 1) && ((moudle1_info.roll_dir_set == DIR_FW))) {
            Select_Direction_Stop(DIR_FW);//停止
            moudle1_info.set_start_status = STOP_STATE;
            moudle1_info.set_start_type = CONTINUE_RUN;
        }
    }

    if (moudle1_info.uploadcnt > 5) {
        moudle1_info.uploadcnt = 0;
        if (moudle1_info.set_start_status == RUN_STATE) {
            if (moudle1_info.roll_dir_set == DIR_FW)
            {
                Select_Direction_Run(DIR_FW);
            }
            if (moudle1_info.roll_dir_set == DIR_BK)
            {
                Select_Direction_Run(DIR_BK);
            }
        }
        if (moudle1_info.set_start_status == STOP_STATE) {
            Select_Direction_Stop(DIR_FW);//停止
        }
    }

    //MOTO2
    moudle2_info.uploadcnt++; //俩对光电接在 左前 和右后
    if ((moudle2_info.set_start_type == RUN_TRIGSTOP) && (moudle2_info.set_start_status == RUN_STATE)) {
        if (((input_in2_3.input_state == 1)|(input_in2_0.input_state == 1)) && (moudle2_info.roll_dir_set == DIR_LF)) {
            Select_Direction_Stop(DIR_LF);//停止
            moudle2_info.set_start_status = STOP_STATE;
            moudle2_info.set_start_type = CONTINUE_RUN;
        }
        if (((input_in2_2.input_state == 1) || (input_in2_1.input_state == 1)) && (moudle2_info.roll_dir_set == DIR_RT)) {
            Select_Direction_Stop(DIR_LF);//停止
            moudle2_info.set_start_status = STOP_STATE;
            moudle2_info.set_start_type = CONTINUE_RUN;
        }
    }

    //MOTO2
    //moudle2_info.uploadcnt++; //俩对光电接在 左后 和左前
    //if ((moudle2_info.set_start_type == RUN_TRIGSTOP) && (moudle2_info.set_start_status == RUN_STATE)) {
    //    if (((input_in2_3.input_state == 1) | (input_in2_0.input_state == 1)) && (moudle2_info.roll_dir_set == DIR_LF)) {
    //        Select_Direction_Stop(DIR_LF);//停止
    //        moudle2_info.set_start_status = STOP_STATE;
    //        moudle2_info.set_start_type = CONTINUE_RUN;
    //    }
    //    if (((input_in2_2.input_state == 1) || (input_in2_1.input_state == 1)) && (moudle2_info.roll_dir_set == DIR_RT)) {
    //        Select_Direction_Stop(DIR_LF);//停止
    //        moudle2_info.set_start_status = STOP_STATE;
    //        moudle2_info.set_start_type = CONTINUE_RUN;
    //    }
    //}


    if (moudle2_info.uploadcnt > 5) {
        moudle2_info.uploadcnt = 0;
        if (moudle2_info.set_start_status == RUN_STATE) {
            if (moudle2_info.roll_dir_set == DIR_LF)
            {
                Select_Direction_Run(DIR_LF);
            }
            if (moudle2_info.roll_dir_set == DIR_RT)
            {
                Select_Direction_Run(DIR_RT);
            }
        }
        if (moudle2_info.set_start_status == STOP_STATE) {
            Select_Direction_Stop(DIR_LF);//停止
        }
    }
}

void moto_set_run_cmd(u8 motX, u8 cmd, u8 type, u8 dir)
{
    if (motX == MOT1)
    {
        moudle1_info.set_start_type = type;
        moudle1_info.set_start_status = cmd;
        moudle1_info.roll_dir_set = dir;
    }
    else if (motX == MOT2)
    {
        moudle2_info.set_start_type = type;
        moudle2_info.set_start_status = cmd;
        moudle2_info.roll_dir_set = dir;
    }
}

void moto_ctrl_convey_process(void)
{
    if (moudlerisecmd.value == VALUE) {
        if (moudlerisecmd.cmd == STOP_CMD) {
            moto_set_run_cmd(MOT1, STOP_STATE, CONTINUE_RUN, DIR_BK);
            moto_set_run_cmd(MOT2, STOP_STATE, CONTINUE_RUN, DIR_LF);
            moudlerisecmd.value = INVALUE;
            return;
        }

        switch (moudlerisecmd.dir) {
        case RUN_AHEAD_TOBACK:
            if (moudlerisecmd.type == CONTINUE_RUN) {
                moto_set_run_cmd(MOT1, RUN_STATE, CONTINUE_RUN, DIR_BK);
                moudlerisecmd.value = INVALUE;
            }
            if (moudlerisecmd.type == RUN_TRIGSTOP) {
                moto_set_run_cmd(MOT1, RUN_STATE, RUN_TRIGSTOP, DIR_BK);
                moudlerisecmd.value = INVALUE;
            }
            break;
        case RUN_BACK_TOAHEAD:
            if (moudlerisecmd.type == CONTINUE_RUN) {
                moto_set_run_cmd(MOT1, RUN_STATE, CONTINUE_RUN, DIR_FW);
                moudlerisecmd.value = INVALUE;
            }
            if (moudlerisecmd.type == RUN_TRIGSTOP) {
                moto_set_run_cmd(MOT1, RUN_STATE, RUN_TRIGSTOP, DIR_FW);
                moudlerisecmd.value = INVALUE;
            }
            break;
        case RUN_LEFT_TORIGHT:
            if (moudlerisecmd.type == CONTINUE_RUN) {
                moto_set_run_cmd(MOT2, RUN_STATE, CONTINUE_RUN, DIR_RT);
                moudlerisecmd.value = INVALUE;
            }
            if (moudlerisecmd.type == RUN_TRIGSTOP) {
                moto_set_run_cmd(MOT2, RUN_STATE, RUN_TRIGSTOP, DIR_RT);
                moudlerisecmd.value = INVALUE;
            }
            break;
        case RUN_RIGHT_TOLEFT:
            if (moudlerisecmd.type == CONTINUE_RUN) {
                moto_set_run_cmd(MOT2, RUN_STATE, CONTINUE_RUN, DIR_LF);
                moudlerisecmd.value = INVALUE;
            }
            if (moudlerisecmd.type == RUN_TRIGSTOP) {
                moto_set_run_cmd(MOT2, RUN_STATE, RUN_TRIGSTOP, DIR_LF);
                moudlerisecmd.value = INVALUE;
            }
            break;
        case RUN_AHEAD_TOLEFT:
            if (moudlerisecmd.type == CONTINUE_RUN) {
                moto_set_run_cmd(MOT1, RUN_STATE, RUN_TRIGSTOP, DIR_BK);
                moudlerisecmd.type = RUN_TYPEDEFAULT;
            }
            if (moudle1_info.set_start_status == STOP_STATE) {
                moto_set_run_cmd(MOT2, RUN_STATE, CONTINUE_RUN, DIR_LF);
                moudlerisecmd.value = INVALUE;
            }
            break;
        case RUN_AHEAD_TORIGHT:
            if (moudlerisecmd.type == CONTINUE_RUN) {
                moto_set_run_cmd(MOT1, RUN_STATE, RUN_TRIGSTOP, DIR_BK);
                moudlerisecmd.type = RUN_TYPEDEFAULT;
            }
            if (moudle1_info.set_start_status == STOP_STATE) {
                moto_set_run_cmd(MOT2, RUN_STATE, CONTINUE_RUN, DIR_RT);
                moudlerisecmd.value = INVALUE;
            }
            break;
        case RUN_BACK_TOLEFT:
            if (moudlerisecmd.type == CONTINUE_RUN) {
                moto_set_run_cmd(MOT1, RUN_STATE, RUN_TRIGSTOP, DIR_FW);
                moudlerisecmd.type = RUN_TYPEDEFAULT;
            }
            if (moudle1_info.set_start_status == STOP_STATE) {
                moto_set_run_cmd(MOT2, RUN_STATE, CONTINUE_RUN, DIR_LF);
                moudlerisecmd.value = INVALUE;
            }
            break;
        case RUN_BACK_TORIGHT:
            if (moudlerisecmd.type == CONTINUE_RUN) {
                moto_set_run_cmd(MOT1, RUN_STATE, RUN_TRIGSTOP, DIR_FW);
                moudlerisecmd.type = RUN_TYPEDEFAULT;
            }
            if (moudle1_info.set_start_status == STOP_STATE) {
                moto_set_run_cmd(MOT2, RUN_STATE, CONTINUE_RUN, DIR_RT);
                moudlerisecmd.value = INVALUE;
            }
            break;
        case RUN_LEFT_TOAHEAD:
            if (moudlerisecmd.type == CONTINUE_RUN) {
                moto_set_run_cmd(MOT2, RUN_STATE, RUN_TRIGSTOP, DIR_RT);
                moudlerisecmd.type = RUN_TYPEDEFAULT;
            }
            if (moudle2_info.set_start_status == STOP_STATE) {
                moto_set_run_cmd(MOT1, RUN_STATE, CONTINUE_RUN, DIR_FW);
                moudlerisecmd.value = INVALUE;
            }
            break;
        case RUN_LEFT_TOBACK:
            if (moudlerisecmd.type == CONTINUE_RUN) {
                moto_set_run_cmd(MOT2, RUN_STATE, RUN_TRIGSTOP, DIR_RT);
                moudlerisecmd.type = RUN_TYPEDEFAULT;
            }
            if (moudle2_info.set_start_status == STOP_STATE) {
                moto_set_run_cmd(MOT1, RUN_STATE, CONTINUE_RUN, DIR_BK);
                moudlerisecmd.value = INVALUE;
            }
            break;
        case RUN_RIGHT_TOAHEAD:
            if (moudlerisecmd.type == CONTINUE_RUN) {
                moto_set_run_cmd(MOT2, RUN_STATE, RUN_TRIGSTOP, DIR_LF);
                moudlerisecmd.type = RUN_TYPEDEFAULT;
            }
            if (moudle2_info.set_start_status == STOP_STATE) {
                moto_set_run_cmd(MOT1, RUN_STATE, CONTINUE_RUN, DIR_FW);
                moudlerisecmd.value = INVALUE;
            }
            break;
        case RUN_RIGHT_TOBACK:
            if (moudlerisecmd.type == CONTINUE_RUN) {
                moto_set_run_cmd(MOT2, RUN_STATE, RUN_TRIGSTOP, DIR_LF);
                moudlerisecmd.type = RUN_TYPEDEFAULT;
            }
            if (moudle2_info.set_start_status == STOP_STATE) {
                moto_set_run_cmd(MOT1, RUN_STATE, CONTINUE_RUN, DIR_BK);
                moudlerisecmd.value = INVALUE;
            }
            break;
        default:
            moudlerisecmd.value = INVALUE;
            break;

        }
    }
}


void moto_roll_test_process(void)
{
    roll_test++;
    if (roll_test > 5000)
    {
        roll_test = 0;
        if (roll_flag == 0) {
            moto_set_run_cmd(MOT1, RUN_STATE, CONTINUE_RUN, 0);
            moto_set_run_cmd(MOT2, STOP_STATE, CONTINUE_RUN, DIR_LF);
        }
        if (roll_flag == 1) {
//            roll_flag = 0;
            moto_set_run_cmd(MOT1, STOP_STATE, CONTINUE_RUN, 0);
            moto_set_run_cmd(MOT2, RUN_STATE, CONTINUE_RUN, DIR_LF);

        }

        if (roll_flag == 0) {
            roll_flag = 1;
            return;
        }
        if (roll_flag == 1) {
            roll_flag = 0;
        }
    }
}


void moto_upload_moto_state(void)
{
    u8 buf[20] = { 0 };
    u16 sendlen = 0;

    buf[0] = 0;
    buf[1] = 0;
    buf[2] = 0;
    buf[3] = 0;

    if ((moudle1_info.set_start_status == RUN_STATE) || (moudle2_info.set_start_status == RUN_STATE))
    {
        buf[4] = 0x01;
    }
    else {
        buf[4] = 0x00;
    }
    if (input_in2_0.input_state == 1) {
        buf[5] |= 0x01;
    }
    if (input_in2_1.input_state == 1) {
        buf[5] |= 0x02;
    }
    if (input_in2_2.input_state == 1) {
        buf[5] |= 0x04;
    }
    if (input_in2_3.input_state == 1) {
        buf[5] |= 0x08;
    }
    if (input_in1_0.input_state == 1) {
        buf[5] |= 0x10;
    }
    if (input_in1_1.input_state == 1) {
        buf[5] |= 0x20;
    }
    buf[6] = 0;
    if (input_in2_0.err == VALUE) {
        buf[7] |= 0x08;
    }
    if (input_in2_1.err == VALUE) {
        buf[7] |= 0x10;
    }
    if (input_in2_2.err == VALUE) {
        buf[7] |= 0x20;
    }
    if (input_in2_3.err == VALUE) {
        buf[7] |= 0x40;
    }
    
    sendlen = 8;

    can_bus_send_msg(buf, sendlen, CAN_FUNC_ID_MODULE_STATUS, 1);

}
