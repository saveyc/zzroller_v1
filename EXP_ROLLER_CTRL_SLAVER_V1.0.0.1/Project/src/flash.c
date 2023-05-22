#include "flash.h"
#include "main.h"

sFlash_moto_msg    motolocal;
sFlash_moto_msg    mototemp;

u16  flash_upload_flag = INVALUE;

void flash_init(void)
{
    u16 temp = 0;
    u16 i = 0;
    motolocal.version[0] = 1;
    motolocal.version[1] = 0;
    motolocal.version[2] = 0;
    motolocal.version[3] = 1;
    motolocal.moudlenum = 0xFFFF; 

    mototemp.version[0] = 1;
    mototemp.version[1] = 0;
    mototemp.version[2] = 0;
    mototemp.version[3] = 1;
    mototemp.moudlenum = 0xFFFF;

    temp = *((u16*)(UserParaAddress));
    if ((temp != 0) && (temp != 0xFFFF)) {
        motolocal.moudlenum = temp;
        if (motolocal.moudlenum <= MAX_MOUDLE_NUM) {
            for (i = 0; i < motolocal.moudlenum; i++) {
                temp = *((u16*)(UserParaAddress + i * 10 + 2));
                if ((temp != 0) && (temp != 0xFFFF)) {
                    motolocal.moudlepara[i].moudleIndex = temp;
                }
                temp = *((u16*)(UserParaAddress + i * 10 + 4));
                if ((temp != 0) && (temp != 0xFFFF)) {
                    motolocal.moudlepara[i].funcConfig = temp;
                }
                temp = *((u16*)(UserParaAddress + i * 10 + 6));
                if ((temp != 0) && (temp != 0xFFFF)) {
                    motolocal.moudlepara[i].spd = temp;
                }
                temp = *((u16*)(UserParaAddress + i * 10 + 8));
                if ((temp != 0) && (temp != 0xFFFF)) {
                    motolocal.moudlepara[i].dir = temp;
                }
                temp = *((u16*)(UserParaAddress + i * 10 + 10));
                if ((temp != 0) && (temp != 0xFFFF)) {
                    motolocal.moudlepara[i].ramp = temp;
                }
            }
        }
        else {
            motolocal.moudlenum = 0xFFFF;
        }
    }

    if (motolocal.moudlenum != 0xFFFF) {
        for (i = 0; i < motolocal.moudlenum; i++) {
            switch (motolocal.moudlepara[i].moudleIndex) {
            case ZONE_TYPE_ONE:
                //if ((motolocal.moudlepara[i].funcConfig & 0x01) == 1) {
                //    mot1_info.value = VALUE;
                //}
                //else {
                //    mot1_info.value = INVALUE;
                //}
                mot1_info.set_dir_sel = motolocal.moudlepara[i].dir;
                mot1_info.set_ramp_sel = motolocal.moudlepara[i].ramp;
                mot1_info.set_speed_sel = motolocal.moudlepara[i].spd;
                if (mot1_info.set_dir_sel == DIR_CW) {
                    mot1_info.opposite_dir = DIR_CCW;
                }
                else if (mot1_info.set_dir_sel == DIR_CCW) {
                    mot1_info.opposite_dir = DIR_CW;
                }
                Set_Mot_Roll_Paras(MOT1, mot1_info.set_speed_sel, mot1_info.set_dir_sel, mot1_info.set_ramp_sel);
                break;
            case ZONE_TYPE_TWO:
                //if ((motolocal.moudlepara[i].funcConfig & 0x01) == 1) {
                //    mot2_info.value = VALUE;
                //}
                //else {
                //    mot2_info.value = INVALUE;
                //}
                mot2_info.set_dir_sel = motolocal.moudlepara[i].dir;
                mot2_info.set_ramp_sel = motolocal.moudlepara[i].ramp;
                mot2_info.set_speed_sel = motolocal.moudlepara[i].spd;
                if (mot2_info.set_dir_sel == DIR_CW) {
                    mot2_info.opposite_dir = DIR_CCW;
                }
                else if (mot2_info.set_dir_sel == DIR_CCW) {
                    mot2_info.opposite_dir = DIR_CW;
                }
                Set_Mot_Roll_Paras(MOT2, mot2_info.set_speed_sel, mot2_info.set_dir_sel, mot2_info.set_ramp_sel);
                break;
            }
        }
    }
    else {
        mot1_info.value = VALUE;
        mot1_info.set_dir_sel = DIR_CW;
        mot1_info.set_ramp_sel = 4;
        mot1_info.set_speed_sel = 6;
        mot1_info.opposite_dir = DIR_CCW;
        Set_Mot_Roll_Paras(MOT1, mot1_info.set_speed_sel, mot1_info.set_dir_sel, mot1_info.set_ramp_sel);

        mot2_info.value = VALUE;
        mot2_info.set_dir_sel = DIR_CW;
        mot2_info.set_ramp_sel = 4;
        mot2_info.set_speed_sel = 6;
        mot2_info.opposite_dir = DIR_CCW;
        Set_Mot_Roll_Paras(MOT2, mot2_info.set_speed_sel, mot2_info.set_dir_sel, mot2_info.set_ramp_sel);

    }
}

void flash_write_buff(u16* buff,u16 len) 
{
    u16 i = 0;

    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    if (FLASH_ErasePage(UserParaAddress) == FLASH_COMPLETE)
    {
        for (i = 0; i < len; i++) {
            FLASH_ProgramHalfWord(UserParaAddress, buff[i]);
        }
    }
    FLASH_Lock();
}

void flash_upload_write_buff(void)
{
    u16 i = 0;
    u16 len = 0;
    u16 buff[30];
    

    if (flash_upload_flag == INVALUE) {
        return;
    }
    flash_upload_flag = INVALUE;

    buff[0] = mototemp.moudlenum;
    if (mototemp.moudlenum > MAX_MOUDLE_NUM) {
        return;
    }

    for (i = 0; i < mototemp.moudlenum; i++) {
        buff[5 * i + 1] = mototemp.moudlepara[i].moudleIndex;
        buff[5 * i + 1 + 1] = mototemp.moudlepara[i].funcConfig;
        buff[5 * i + 2 + 1] = mototemp.moudlepara[i].spd;
        buff[5 * i + 3 + 1] = mototemp.moudlepara[i].dir;
        buff[5 * i + 4 + 1] = mototemp.moudlepara[i].ramp;
    }

    buff[0] = 0xAAAA;
    len = 1 + 5 * i;
    flash_write_buff(buff, len);
    flash_init();
}