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

void canbus_recv_msg(u8* buf, u8 src, u8 type);
void canbus_recv_start_cmd(u8* buf);

void InitCanSendQueue(void)
{
    sCAN_SEND_QUEUE *q;
    
    q = &canSendQueue;
    q->maxSize = canSendQueueSize;
    q->queue = canSendQueueBuff;
    q->front = q->rear = 0;
}
void AddCanSendData2Queue(sCanFrameExt x)
{
    sCAN_SEND_QUEUE *q = &canSendQueue;
    //队列满
    if((q->rear + 1) % q->maxSize == q->front)
    {
        return;
    }
    q->rear = (q->rear + 1) % q->maxSize; // 求出队尾的下一个位置
    q->queue[q->rear] = x; // 把x的值赋给新的队尾
}

u8 can_bus_send_one_frame(sCanFrameExt sTxMsg)
{
    CanTxMsg TxMessage;
    
    TxMessage.ExtId = (sTxMsg.extId.src_id)|((sTxMsg.extId.func_id&0xF)<<8)|((sTxMsg.extId.seg_num&0x7F)<<12)|((sTxMsg.extId.seg_polo&0x3)<<19) | ((sTxMsg.extId.dst_id & 0xFF) << 21);
    TxMessage.IDE = CAN_ID_EXT;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.DLC = sTxMsg.data_len;
    memcpy(TxMessage.Data, sTxMsg.data, TxMessage.DLC);
    
    return CAN_Transmit(CAN1,&TxMessage);
}

void can_bus_frame_receive(CanRxMsg rxMsg)
{
    sCanFrameExtID extID;
    u8 recv_finish_flag = 0;
    
    extID.seg_polo = (rxMsg.ExtId>>19)&0x3;
    extID.seg_num  = (rxMsg.ExtId>>12)&0x7F;
    extID.func_id  = (rxMsg.ExtId>>8)&0xF;
    extID.src_id = rxMsg.ExtId & 0xFF;
    extID.dst_id  = (rxMsg.ExtId>>21)&0xFF;
    
    //if(extID.func_id == CAN_FUNC_ID_BOOT_MODE && extID.dst_id == local_station)
    //{
    //    //BKP_WriteBackupRegister(BKP_DR8, 0x55);
    //    //NVIC_SystemReset();
    //    FLASH_Unlock();
    //    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
    //    if(FLASH_ErasePage(UserAppEnFlagAddress) == FLASH_COMPLETE)
    //    {
    //        FLASH_ProgramHalfWord(UserAppEnFlagAddress,0x0000);
    //    }
    //    FLASH_Lock();
    //    NVIC_SystemReset();
    //}
    //接收数据

    if (extID.dst_id != local_station) {
        return;
    }

    if(extID.seg_polo == CAN_SEG_POLO_NONE)
    {
        memcpy(can_recv_buff,rxMsg.Data,rxMsg.DLC);
        can_recv_len = rxMsg.DLC;
        recv_finish_flag = 1;
    }
    else if(extID.seg_polo == CAN_SEG_POLO_FIRST)
    {
        memcpy(can_recv_buff,rxMsg.Data,rxMsg.DLC);
        
        g_SegPolo = CAN_SEG_POLO_FIRST;
        g_SegNum = extID.seg_num;
        g_SegBytes = rxMsg.DLC;
    }
    else if(extID.seg_polo == CAN_SEG_POLO_MIDDLE)
    {
        if(   (g_SegPolo == CAN_SEG_POLO_FIRST) 
           && (extID.seg_num == (g_SegNum+1)) 
           && ((g_SegBytes+rxMsg.DLC) <= CAN_RX_BUFF_SIZE) )
        {
            memcpy(can_recv_buff+g_SegBytes,rxMsg.Data,rxMsg.DLC);
            
            g_SegNum ++;
            g_SegBytes += rxMsg.DLC;
        }
    }
    else if(extID.seg_polo == CAN_SEG_POLO_FINAL)
    {
        if(   (g_SegPolo == CAN_SEG_POLO_FIRST) 
           && (extID.seg_num == (g_SegNum+1)) 
           && ((g_SegBytes+rxMsg.DLC) <= CAN_RX_BUFF_SIZE) )
        {
            memcpy(can_recv_buff+g_SegBytes,rxMsg.Data,rxMsg.DLC);
            can_recv_len = g_SegBytes + rxMsg.DLC;
            recv_finish_flag = 1;
            
            g_SegPolo = CAN_SEG_POLO_NONE;
            g_SegNum = 0;
            g_SegBytes = 0;
        }
    }
    if(recv_finish_flag != 1) return;

    if (recv_finish_flag == 1) {
        recv_finish_flag = 0;
        canbus_recv_msg(can_recv_buff, extID.src_id , extID.func_id);
    }
    
}

void canbus_recv_msg(u8* buf, u8 src, u8 type)
{
    switch (type)
    {
    case CAN_FUNC_ID_START_STOP_CMD:
        canbus_recv_start_cmd(buf);
        break;
    default:
      break;
    }
}

void canbus_recv_start_cmd(u8* buf)
{
    u16 frametmp = 0;
    frametmp = buf[1] | (buf[2] << 8);
    if (frametmp == canrecv_framecnt_rise) {
        return;
    }
    canrecv_framecnt_rise = frametmp;


    if (buf[0] == ZONE_TYPE_RISE) {
        //if (moudlerisecmd.value = INVALUE) {
            moudlerisecmd.value = VALUE;
            moudlerisecmd.cmd = buf[3];
            moudlerisecmd.dir = buf[4];
            moudlerisecmd.type = buf[5];
        //}
    }
}


void can_bus_send_msg(u8* pbuf, u16 send_tot_len, u8 func_id, u8 dst_id )
{
    sCanFrameExt canTxMsg;
    u16 can_send_len;
    
    canTxMsg.extId.func_id  = func_id;
    canTxMsg.extId.dst_id   = dst_id;
    canTxMsg.extId.src_id = local_station;
    if(send_tot_len <= CAN_PACK_DATA_LEN)//不需分段传输
    {
        canTxMsg.extId.seg_polo = CAN_SEG_POLO_NONE;
        canTxMsg.extId.seg_num  = 0;
        canTxMsg.data_len = send_tot_len;
        memcpy(canTxMsg.data, pbuf, canTxMsg.data_len);
        AddCanSendData2Queue(canTxMsg);
    }
    else//需要分段传输
    {
        canTxMsg.extId.seg_polo = CAN_SEG_POLO_FIRST;
        canTxMsg.extId.seg_num  = 0;
        canTxMsg.data_len = CAN_PACK_DATA_LEN;
        memcpy(canTxMsg.data, pbuf, canTxMsg.data_len);
        AddCanSendData2Queue(canTxMsg);
        can_send_len = CAN_PACK_DATA_LEN;
        while(1)
        {
            if(can_send_len + CAN_PACK_DATA_LEN < send_tot_len)
            {
                canTxMsg.extId.seg_polo = CAN_SEG_POLO_MIDDLE;
                canTxMsg.extId.seg_num ++;
                canTxMsg.data_len = CAN_PACK_DATA_LEN;
                memcpy(canTxMsg.data, pbuf+can_send_len, canTxMsg.data_len);
                AddCanSendData2Queue(canTxMsg);
                can_send_len += CAN_PACK_DATA_LEN;
            }
            else
            {
                canTxMsg.extId.seg_polo = CAN_SEG_POLO_FINAL;
                canTxMsg.extId.seg_num ++;
                canTxMsg.data_len = send_tot_len-can_send_len;
                memcpy(canTxMsg.data, pbuf+can_send_len, canTxMsg.data_len);
                AddCanSendData2Queue(canTxMsg);
                break;
            }
        }
    }
}

void can_send_frame_process()
{
    sCAN_SEND_QUEUE *q = &canSendQueue;
    sCanFrameExt* canTxMsg = NULL;
    u16 front;
    u8  tx_mailbox;
    
    //队列空
    if(q->front == q->rear)
    {
        return;
    }
    front = q->front;
    front = (front + 1) % q->maxSize;
    canTxMsg = (sCanFrameExt*)(&(q->queue[front]));
    tx_mailbox = can_bus_send_one_frame(*canTxMsg);
    if(tx_mailbox != CAN_NO_MB)
    {
        q->front = front;//出列
    }
}