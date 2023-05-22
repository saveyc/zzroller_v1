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

u8   canbus_readpara_buff[200];
u16  canbus_readpara_len = 0;

void InitCanSendQueue(void)
{
    sCAN_SEND_QUEUE *q = NULL;
    
    q = &canSendQueue;
    q->maxSize = canSendQueueSize;
    q->queue = canSendQueueBuff;
    q->front = q->rear = 0;
}

void AddCanSendData2Queue(sCanFrameExt x)
{
    sCAN_SEND_QUEUE *q = &canSendQueue;
    //������
    if((q->rear + 1) % q->maxSize == q->front)
    {
        return;
    }
    q->rear = (q->rear + 1) % q->maxSize; // �����β����һ��λ��
    q->queue[q->rear] = x; // ��x��ֵ�����µĶ�β
}

u8 can_bus_send_one_frame(sCanFrameExt sTxMsg)
{
    CanTxMsg TxMessage;
    
    TxMessage.ExtId = (sTxMsg.extId.src_id & 0xFF)|((sTxMsg.extId.func_id&0xF)<<8)|((sTxMsg.extId.seg_num&0xFF)<<12)|((sTxMsg.extId.seg_polo&0x3)<<19)| ((sTxMsg.extId.dst_id & 0x7F) << 21);
    TxMessage.IDE = CAN_ID_EXT;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.DLC = sTxMsg.data_len;
    memcpy(TxMessage.Data, sTxMsg.data, TxMessage.DLC);
    
    return CAN_Transmit(CAN1,&TxMessage);
}



void para_data_recv_process(u8* pbuf,u16 recv_len)
{
    u16 data_len = 0;
    u8  cmd = 0;
    
    data_len = pbuf[0]|(pbuf[1]>>8);
    cmd = pbuf[2];
    
    if (data_len != recv_len) {
        return;
    }
    
    switch(cmd)
    {
    case RECV_SET_PARA://д�����ظ�
        //�ӻ��ѱ�������ͨ��CAN���͸�����
        AddSendMsgToQueue(REPLY_RECV_MSG_PC2BD_MOTOPARA_CONFIG_TYPE);
        break;
    case RECV_READ_PARA://�������ظ�
        //�����յ��ӻ�������ͨ������ת������λ��
        canbus_recv_read_moudlepara(pbuf, recv_len);
        break;
    default:
        break;
    }
}

void module_status_recv_process(u8* buf,u16 recv_len,u8 src_id)
{
    u8 index = 0;
    index = src_id - 2;

    if (index >= BELT_ZONE_NUM) {
        return;
    }

    beltMoudlestate.state[index].ctrlindex = src_id;

    beltMoudlestate.state[index].node[0].zoneState = buf[0];
    beltMoudlestate.state[index].node[0].zoneAlarm = buf[1];
    beltMoudlestate.state[index].node[1].zoneState = buf[2];
    beltMoudlestate.state[index].node[1].zoneAlarm = buf[3];
    beltMoudlestate.state[index].node[2].zoneState = buf[4] | (buf[5] << 8);
    beltMoudlestate.state[index].node[2].zoneAlarm = buf[6] | (buf[7] << 8);
}

void canbus_recv_can_msg(u8* buf, u16 len, u8 bdindex,u8 type) 
{
    switch (type) {
    case CAN_FUNC_ID_PARA_DATA:
        para_data_recv_process(buf, len);
        break;
    case CAN_FUNC_ID_MODULE_STATUS:
        module_status_recv_process(buf, len, bdindex);
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
    
    //if((extID.func_id == CAN_FUNC_ID_BOOT_MODE) && (extID.dst_id == local_station))
    //{
    //    FLASH_Unlock();
    //    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
    //    if(FLASH_ErasePage(UserAppEnFlagAddress) == FLASH_COMPLETE)
    //    {
    //        FLASH_ProgramHalfWord(UserAppEnFlagAddress,0x0000);
    //    }
    //    FLASH_Lock();
    //    NVIC_SystemReset();
    //}
    //��������
    if ((extID.dst_id) != 1) {
        return;
    }
    if(extID.seg_polo == CAN_SEG_POLO_NONE)
    {
        memcpy(can_recv_buff,rxMsg.Data,rxMsg.DLC);
        can_recv_len = rxMsg.DLC;
        canbus_recv_can_msg(can_recv_buff, can_recv_len, extID.src_id, extID.func_id);
    }
    else if(extID.seg_polo == CAN_SEG_POLO_FIRST)
    {
        memcpy(can_recv_buff,rxMsg.Data,rxMsg.DLC);       
        g_SegPolo = CAN_SEG_POLO_FIRST;
        g_SegNum = extID.seg_num;
        g_SegBytes = rxMsg.DLC;
    }
    else if( extID.seg_polo == CAN_SEG_POLO_MIDDLE)
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
    
    canTxMsg.extId.func_id  = func_id;
    canTxMsg.extId.src_id   = src_id;
    canTxMsg.extId.dst_id = dst_id;

    if(send_tot_len <= CAN_PACK_DATA_LEN)//����ֶδ���
    {
        canTxMsg.extId.seg_polo = CAN_SEG_POLO_NONE;
        canTxMsg.extId.seg_num  = 0;
        canTxMsg.data_len = send_tot_len;
        memcpy(canTxMsg.data, pbuf, canTxMsg.data_len);
        AddCanSendData2Queue(canTxMsg);
    }
    else//��Ҫ�ֶδ���
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
    sCAN_SEND_QUEUE* q = &canSendQueue;
    sCanFrameExt* canTxMsg = NULL;
    u16 front = 0;
    u8  tx_mailbox = 0;

    //���п�
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
        q->front = front;//����
    }
}

// ���ôӰ忨��ز���
void canbus_send_user_moudlepara(u8* buf ,u16 len)
{
    u16 sendlen = 0;
    u8 dst = 0;
    u8 sendbuf[50] = { 0 };
    u16 i = 0;

    dst = buf[0];

    sendlen = len + 3;


    sendbuf[0] = (sendlen) & 0xFF;
    sendbuf[1] = (sendlen >> 8) & 0xFF;
    sendbuf[2] = SET_PARA;

    for (i = 0; i < len; i++) {
        sendbuf[i + 3] = buf[i];
    }

    can_bus_send_msg(sendbuf, sendlen, CAN_FUNC_ID_PARA_DATA, 1, dst);
}

void canbus_read_user_moudlepara(u8 dst) 
{
    u16 sendlen = 0;
    u8  sendbuf[20];

    sendlen = 3;

    sendbuf[0] = (sendlen) & 0xFF;
    sendbuf[1] = (sendlen >> 8) & 0xFF;
    sendbuf[2] = READ_PARA;

    can_bus_send_msg(sendbuf, sendlen, CAN_FUNC_ID_PARA_DATA, 1, dst);
}

void canbus_recv_read_moudlepara(u8* buff, u16 len)
{
    u16 i = 0;
    canbus_readpara_len = len - 3;

    for (i = 0; i < canbus_readpara_len; i++) {
        canbus_readpara_buff[i] = buff[i + 3];
    }
    AddSendMsgToQueue(REPLY_RECV_MSG_PC2BD_MOTOPARA_ACQUIRE_TYPE);


}

void vcanbus_send_start_cmd(sMoudle_cmd moudle, u16 frame, u8 dst)
{
    u8 buff[10];
    u16 sendlen = 0;
    buff[0] = moudle.moudle;
    buff[1] = frame & 0xFF;
    buff[2] = (frame >> 8) & 0xFF;
    buff[3] = moudle.cmd;
    buff[4] = moudle.dir;
    buff[5] = moudle.type;
    sendlen = 6;

    can_bus_send_msg(buff,sendlen, CAN_FUNC_ID_START_STOP_CMD, local_station,dst);
}



//





