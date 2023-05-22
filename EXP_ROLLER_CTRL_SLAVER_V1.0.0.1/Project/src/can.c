#include "can.h"
#include "main.h"

#define canSendQueueSize  50
sCanFrameExt canSendQueueBuff[canSendQueueSize];
sCAN_SEND_QUEUE canSendQueue;



u8  can_send_buff[CAN_TX_BUFF_SIZE];
u16 can_send_len = 0;
u8  can_recv_buff[CAN_RX_BUFF_SIZE];
u16 can_recv_len = 0;

u8  g_SegPolo = CAN_SEG_POLO_NONE;
u8  g_SegNum = 0;
u16 g_SegBytes = 0;


void InitCanSendQueue(void)
{
    sCAN_SEND_QUEUE* q = NULL;

    q = &canSendQueue;
    q->maxSize = canSendQueueSize;
    q->queue = canSendQueueBuff;
    q->front = q->rear = 0;
}

void AddCanSendData2Queue(sCanFrameExt x)
{
    sCAN_SEND_QUEUE* q = &canSendQueue;
    //队列满
    if ((q->rear + 1) % q->maxSize == q->front)
    {
        return;
    }
    q->rear = (q->rear + 1) % q->maxSize; // 求出队尾的下一个位置
    q->queue[q->rear] = x; // 把x的值赋给新的队尾
}

u8 can_bus_send_one_frame(sCanFrameExt sTxMsg)
{
    CanTxMsg TxMessage;

    TxMessage.ExtId = (sTxMsg.extId.src_id & 0xFF) | ((sTxMsg.extId.func_id & 0xF) << 8) | ((sTxMsg.extId.seg_num & 0xFF) << 12) | ((sTxMsg.extId.seg_polo & 0x3) << 19) | ((sTxMsg.extId.dst_id & 0xFF) << 21);
    TxMessage.IDE = CAN_ID_EXT;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.DLC = sTxMsg.data_len;
    memcpy(TxMessage.Data, sTxMsg.data, TxMessage.DLC);

    return CAN_Transmit(CAN1, &TxMessage);
}


void para_data_recv_process(u8* pbuf, u16 recv_len)
{
    u16 data_len = 0;
    u8  cmd = 0;

    data_len = pbuf[0] | (pbuf[1] >> 8);
    cmd = pbuf[2];

    if (data_len != recv_len)
        return;

    switch (cmd)
    {
    case SET_PARA://接收写的参数
        can_recv_set_para(&(pbuf[2]),recv_len);
        can_reply_set_para();
        break;
    case RECV_READ_PARA://接收读参数命令并且回复
        //从机收到
        can_reply_read_para();
        break;
    default:
        break;
    }
}

void canbus_recv_start_cmd(u8* buf) 
{
    u8 cmd = 0;
    u8 type = 0;
    u8 index = 0;
    u16 frame = 0;
    u8 dir = 0;

    index = buf[0];
    frame = (buf[1]) | (buf[2] << 8);
    cmd = buf[3];
    dir = buf[4];
    type = buf[5];

    if (buf[0] == ZONE_TYPE_ONE) {
        if (frame != cansend_framecnt_one) {
            cansend_framecnt_one = frame;
            moto_set_run_cmd(MOT1, cmd, type, dir);
        }
    }
    else if (buf[0] == ZONE_TYPE_TWO) {
        if (frame != cansend_framecnt_two) {
            cansend_framecnt_two = frame;
            moto_set_run_cmd(MOT2, cmd, type, dir);
        }
    }

}


void canbus_recv_can_msg(u8* buf, u16 len, u8 bdindex, u8 type)
{
    switch (type) {
    case CAN_FUNC_ID_PARA_DATA:
        para_data_recv_process(buf, len);
        break;
    case CAN_FUNC_ID_START_STOP_CMD:
        canbus_recv_start_cmd(buf);
        break;
    default:
        break;
    }
}

void can_bus_frame_receive(CanRxMsg rxMsg)
{
    sCanFrameExtID extID;

    extID.seg_polo = (rxMsg.ExtId >> 19) & 0x3;
    extID.seg_num = (rxMsg.ExtId >> 12) & 0x7F;
    extID.func_id = (rxMsg.ExtId >> 8) & 0xF;
    extID.src_id = rxMsg.ExtId & 0xFF;
    extID.dst_id = (rxMsg.ExtId >> 21) & 0xFF;

    //if ((extID.func_id == CAN_FUNC_ID_BOOT_MODE) && (extID.dst_id == localStation))
    //{
    //    FLASH_Unlock();
    //    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    //    if (FLASH_ErasePage(UserAppEnFlagAddress) == FLASH_COMPLETE)
    //    {
    //        FLASH_ProgramHalfWord(UserAppEnFlagAddress, 0x0000);
    //    }
    //    FLASH_Lock();
    //    NVIC_SystemReset();
    //}

    if (extID.dst_id != localStation) {
        return;
    }

    //接收数据
    if (extID.seg_polo == CAN_SEG_POLO_NONE)
    {
        memcpy(can_recv_buff, rxMsg.Data, rxMsg.DLC);
        can_recv_len = rxMsg.DLC;
        canbus_recv_can_msg(can_recv_buff, can_recv_len, extID.src_id, extID.func_id);
    }
    else if (extID.seg_polo == CAN_SEG_POLO_FIRST)
    {
        memcpy(can_recv_buff, rxMsg.Data, rxMsg.DLC);
        g_SegPolo = CAN_SEG_POLO_FIRST;
        g_SegNum = extID.seg_num;
        g_SegBytes = rxMsg.DLC;
    }
    else if (extID.seg_polo == CAN_SEG_POLO_MIDDLE)
    {
        if ((g_SegPolo == CAN_SEG_POLO_FIRST)
            && (extID.seg_num == (g_SegNum + 1))
            && ((g_SegBytes + rxMsg.DLC) <= CAN_RX_BUFF_SIZE))
        {
            memcpy(can_recv_buff + g_SegBytes, rxMsg.Data, rxMsg.DLC);
            g_SegNum++;
            g_SegBytes += rxMsg.DLC;
        }
    }
    else if (extID.seg_polo == CAN_SEG_POLO_FINAL)
    {
        if ((g_SegPolo == CAN_SEG_POLO_FIRST)
            && (extID.seg_num == (g_SegNum + 1))
            && ((g_SegBytes + rxMsg.DLC) <= CAN_RX_BUFF_SIZE))
        {
            memcpy(can_recv_buff + g_SegBytes, rxMsg.Data, rxMsg.DLC);
            can_recv_len = g_SegBytes + rxMsg.DLC;
            canbus_recv_can_msg(can_recv_buff, can_recv_len, extID.src_id, extID.func_id);
            g_SegPolo = CAN_SEG_POLO_NONE;
            g_SegNum = 0;
            g_SegBytes = 0;
        }
    }
}


void can_bus_send_msg(u8* pbuf, u16 send_tot_len, u8 func_id, u8 src_id, u8 dst_id)
{
    sCanFrameExt canTxMsg;
    u16 can_send_len = 0;

    canTxMsg.extId.func_id = func_id;
    canTxMsg.extId.src_id = src_id;
    canTxMsg.extId.dst_id = dst_id;

    if (send_tot_len <= CAN_PACK_DATA_LEN)//不需分段传输
    {
        canTxMsg.extId.seg_polo = CAN_SEG_POLO_NONE;
        canTxMsg.extId.seg_num = 0;
        canTxMsg.data_len = send_tot_len;
        memcpy(canTxMsg.data, pbuf, canTxMsg.data_len);
        AddCanSendData2Queue(canTxMsg);
    }
    else//需要分段传输
    {
        canTxMsg.extId.seg_polo = CAN_SEG_POLO_FIRST;
        canTxMsg.extId.seg_num = 0;
        canTxMsg.data_len = CAN_PACK_DATA_LEN;
        memcpy(canTxMsg.data, pbuf, canTxMsg.data_len);
        AddCanSendData2Queue(canTxMsg);
        can_send_len = CAN_PACK_DATA_LEN;
        while (1)
        {
            if (can_send_len + CAN_PACK_DATA_LEN < send_tot_len)
            {
                canTxMsg.extId.seg_polo = CAN_SEG_POLO_MIDDLE;
                canTxMsg.extId.seg_num++;
                canTxMsg.data_len = CAN_PACK_DATA_LEN;
                memcpy(canTxMsg.data, pbuf + can_send_len, canTxMsg.data_len);
                AddCanSendData2Queue(canTxMsg);
                can_send_len += CAN_PACK_DATA_LEN;
            }
            else
            {
                canTxMsg.extId.seg_polo = CAN_SEG_POLO_FINAL;
                canTxMsg.extId.seg_num++;
                canTxMsg.data_len = send_tot_len - can_send_len;
                memcpy(canTxMsg.data, pbuf + can_send_len, canTxMsg.data_len);
                AddCanSendData2Queue(canTxMsg);
                break;
            }
        }
    }
}

void can_send_frame_process()
{
    sCAN_SEND_QUEUE* q = &canSendQueue;
    sCanFrameExt* canTxMsg = NULL;
    u16 front = 0;
    u8  tx_mailbox = 0;

    //队列空
    if (q->front == q->rear)
    {
        return;
    }
    front = q->front;
    front = (front + 1) % q->maxSize;
    canTxMsg = (sCanFrameExt*)(&(q->queue[front]));
    tx_mailbox = can_bus_send_one_frame(*canTxMsg);
    if (tx_mailbox != CAN_NO_MB)
    {
        q->front = front;//出列
    }
}


void can_recv_set_para(u8* buff,u16 len)
{
    u16 i = 0;
    u8 ctrl_index = buff[0] | (buff[1] << 8);
    u16 num = buff[2] | (buff[3] << 8);
    if (num > MAX_MOUDLE_NUM) {
        return;
    }
    mototemp.moudlenum = num;

    
    flash_upload_flag = VALUE;
    for (i = 0; i < mototemp.moudlenum; i++) {
        mototemp.moudlepara[i].moudleIndex = buff[10 * i + 4] | (buff[10 * i + 1 + 4] << 8);
        mototemp.moudlepara[i].funcConfig = buff[10 * i + 2 + 4] | (buff[10 * i + 3 + 4] << 8);
        mototemp.moudlepara[i].spd = buff[10 * i + 4 + 4] | (buff[10 * i + 5 + 4] << 8);
        mototemp.moudlepara[i].dir = buff[10 * i + 6 + 4] | (buff[10 * i + 7 + 4] << 8);
        mototemp.moudlepara[i].ramp = buff[10 * i + 8 + 4] | (buff[10 * i + 9 + 4] << 8);
    }

}

void can_reply_set_para(void)
{
    u8 buff[10] = { 0 };
    u16 sendlen = 0;

    sendlen = 3;
    buff[0] = sendlen & 0xFF;
    buff[1] = (sendlen>>8) & 0xFF;
    buff[2] = RECV_SET_PARA;

    can_bus_send_msg(buff, sendlen, CAN_FUNC_ID_PARA_DATA,  localStation, 1);
}

void can_reply_read_para(void)
{
    u8 buff[50] = { 0 };
    u8 sendlen = 0;
    u16 i = 0;
    if (motolocal.moudlenum != 0xFFFF) {
        buff[3] = motolocal.version[0];
        buff[4] = motolocal.version[1];
        buff[5] = motolocal.version[2];
        buff[6] = motolocal.version[3];
        buff[7] = localStation;
        buff[8] = 0;
        buff[9] = motolocal.moudlenum & 0xFF;
        buff[10] = (motolocal.moudlenum >> 8) & 0xFF;
        for (i = 0; motolocal.moudlenum; i++) {
            buff[10 * i + 11] = motolocal.moudlepara[i].moudleIndex & 0xFF;
            buff[10 * i + 1 + 11] = (motolocal.moudlepara[i].moudleIndex >> 8) & 0xFF;
            buff[10 * i + 2 + 11] = (motolocal.moudlepara[i].funcConfig) & 0xFF;
            buff[10 * i + 3 + 11] = (motolocal.moudlepara[i].funcConfig >> 8) & 0xFF;
            buff[10 * i + 4 + 11] = (motolocal.moudlepara[i].spd) & 0xFF;
            buff[10 * i + 5 + 11] = (motolocal.moudlepara[i].spd >> 8) & 0xFF;
            buff[10 * i + 6 + 11] = (motolocal.moudlepara[i].dir) & 0xFF;
            buff[10 * i + 7 + 11] = (motolocal.moudlepara[i].dir >> 8) & 0xFF;
            buff[10 * i + 8 + 11] = (motolocal.moudlepara[i].ramp) & 0xFF;
            buff[10 * i + 9 + 11] = (motolocal.moudlepara[i].ramp >> 8) & 0xFF;
        }

        sendlen = 8 + 3 + 10 * motolocal.moudlenum;
        buff[0] = sendlen & 0xFF;
        buff[1] = (sendlen >> 8) & 0xFF;
        buff[2] = RECV_READ_PARA;

        can_bus_send_msg(buff, sendlen, CAN_FUNC_ID_PARA_DATA,  localStation, 1);
    }
    else{
        buff[3] = motolocal.version[0];
        buff[4] = motolocal.version[1];
        buff[5] = motolocal.version[2];
        buff[6] = motolocal.version[3];
        buff[7] = localStation;
        buff[8] = 0;
        buff[9] = 0;
        buff[10] = 0;
        
        sendlen = 11;
        buff[0] = sendlen & 0xFF;
        buff[1] = (sendlen >> 8) & 0xFF;
        buff[2] = RECV_READ_PARA;
        can_bus_send_msg(buff, sendlen, CAN_FUNC_ID_PARA_DATA,  localStation, 1);
    }
}

