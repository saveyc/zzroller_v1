#include "main.h"
#include "udpclient.h"

#define msgSendQueueSize  100
u16 msgQueueBuff[msgSendQueueSize];
MSG_SEND_QUEUE msgSendQueue;

sFun_Online_ctrl    funOnlineCtrl;

//����ģ��״̬�õ�
u8 upload_motostate_buff[1000] = { 0 };
u16 zonestate_index = 0;
u16 upload_state_flag = INVALUE;
u16 upload_state_cnt = 0;
u16 upload_pkg_cnt = 0;

void fun_struct_init(void)
{
    funOnlineCtrl.sendcnt = 50;
    funOnlineCtrl.checkcnt = 0;
    funOnlineCtrl.version[0] = 1;
    funOnlineCtrl.version[1] = 0;
    funOnlineCtrl.version[2] = 0;
    funOnlineCtrl.version[3] = 0;
}

void initQueue(MSG_SEND_QUEUE *q, u16 ms)
{
    q->maxSize = ms;
    q->queue = msgQueueBuff;
    q->front = q->rear = 0;
}
void enQueue(MSG_SEND_QUEUE *q, u16 x)
{
    //������
    if((q->rear + 1) % q->maxSize == q->front)
    {
        return;
    }
    q->rear = (q->rear + 1) % q->maxSize; // �����β����һ��λ��
    q->queue[q->rear] = x; // ��x��ֵ�����µĶ�β
}
u16 outQueue(MSG_SEND_QUEUE *q)
{
    //���п�
    if(q->front == q->rear)
    {
        return 0;
    }
    q->front = (q->front + 1) % q->maxSize; // ʹ����ָ��ָ����һ��λ��
    return q->queue[q->front]; // ���ض���Ԫ��
}
void InitSendMsgQueue(void)
{
    initQueue(&msgSendQueue, msgSendQueueSize);
}
void AddSendMsgToQueue(u16 msg)
{
    enQueue(&msgSendQueue, msg);
}
u16 GetSendMsgFromQueue(void)
{
    return (outQueue(&msgSendQueue));
}

void fun_recv_beltMoudle_para(u8* buf,u16 len)
{
    canbus_send_user_moudlepara(buf, len);
}

void funrec_zoneConfig_cmd(u8* buf)
{
    u16 i = 0;
    u16 total = 0;
    u16 j = 0;
    u16 flag = 0;

    sData_RealtimeState* statenode = NULL;

    total = (buf[0] | (buf[1] << 8));
    if (total > 50) {
        return;
    }

    for (i = 0; i < total; i++) {
        for (j = 0; j < BELT_ZONE_NUM; j++) {
            if (zoneConfig.zoneNode[j].zoneIndex == 0xFFFF) {
                break;
            }
        }

        if (j != BELT_ZONE_NUM) {
            //ctrlindex = (buf[i * 14 + 2 + 2] | (buf[i * 14 + 3 + 2] << 8));
            zoneConfig.zoneNode[j].zoneIndex = (buf[i * 14 + 0 + 2] | (buf[i * 14 + 1 + 2] << 8));
            zoneConfig.zoneNode[j].ctrlIndex = (buf[i * 14 + 2 + 2] | (buf[i * 14 + 3 + 2] << 8));
            zoneConfig.zoneNode[j].beltMoudleIndex = (buf[i * 14 + 4 + 2] | (buf[i * 14 + 5 + 2] << 8));
            zoneConfig.zoneNode[j].front_zone = (buf[i * 14 + 6 + 2] | (buf[i * 14 + 7 + 2] << 8));
            zoneConfig.zoneNode[j].rear_zone = (buf[i * 14 + 8 + 2] | (buf[i * 14 + 9 + 2] << 8));
            zoneConfig.zoneNode[j].left_zone = (buf[i * 14 + 10 + 2] | (buf[i * 14 + 11 + 2] << 8));
            zoneConfig.zoneNode[j].right_zone = (buf[i * 14 + 12 + 2] | (buf[i * 14 + 13 + 2] << 8));

            statenode = data_find_ctrl_status(zoneConfig.zoneNode[j].ctrlIndex, zoneConfig.zoneNode[j].beltMoudleIndex);
            statenode->zoneIndex = zoneConfig.zoneNode[j].zoneIndex;
        }
    }

} 

void funrecv_pkg_trans_cmd(u8* buff)
{
    sData_pkg_node node;
    node.index = NULL;
    u16 i = 0;

    node.pkgId = buff[0] | (buff[1] << 8) | (buff[2] << 16) | (buff[3] << 24);
    node.totalzoneNum = buff[4] | (buff[5] << 8);

    for (i = 0; i < node.totalzoneNum; i++) {
        node.zoneindex[i] = buff[i * 2 + 6] | (buff[i * 2 + 1 + 6] << 8);
    }


    data_addto_pkg_list(node);

}

void funrecv_pkg_state_cmd(u8* buff)
{
    sData_pkg_node* node;
    u16 itemvalue = 0;
    List_t* q;
    u32 idtemp = 0;
    u16 state = 0;

    idtemp = buff[0] | (buff[1] << 8) | (buff[2] << 16) | (buff[3] << 24);
    state = buff[4] | (buff[5] << 8);

    q = &pkg_list;

    itemvalue = q->uxNumberOfItems;

    for (q->pxIndex = (ListItem_t*)q->xListEnd.pxNext; q->pxIndex->xItemValue != itemvalue; q->pxIndex = q->pxIndex->pxNext)
    {
        node = q->pxIndex->pvOwner;
        if (node->pkgId == idtemp) {
            node->allowState = state;
        }
    }

}

void funrecv_start_stop_moudle(u8* buf)
{
    sData_zone_node* zonenode = NULL;
    u16 i = 0;
    sMoudle_cmd moudletem;
    u16 num = 0;
    u16 zoneindex = 0;
    if ((buf[0] | (buf[1] << 8)) == 0) {
        moudletem.cmd = RUN_CMD;
    }
    if ((buf[0] | (buf[1] << 8)) == 1) {
        moudletem.cmd = STOP_CMD;
    }
    if ((buf[2] | (buf[3] << 8)) == 0) {
        moudletem.dir = RUN_BACK_TOAHEAD;
    }
    if ((buf[2] | (buf[3] << 8)) == 1) {
        moudletem.dir = RUN_AHEAD_TOBACK;
    }
    moudletem.type = CONTINUE_RUN;

    num = (buf[4] | (buf[5] << 8));

    for (i = 0; i < num; i++) {
        zoneindex = (buf[2 * i + 6] | (buf[2 * i + 1 + 6] << 8));
        zonenode = data_find_zone_moudlestate(zoneindex);
        if (zonenode != NULL) {
            moudletem.moudle = zonenode->beltMoudleIndex;

            if (zonenode->beltMoudleIndex == ZONE_TYPE_ONE) {
                cansend_framecnt_one[zonenode->ctrlIndex - 2]++;
                moudletem.moudle = ZONE_TYPE_ONE;
                vcanbus_send_start_cmd(moudletem, cansend_framecnt_one[zonenode->ctrlIndex - 2], zonenode->ctrlIndex);

            }
            if (zonenode->beltMoudleIndex == ZONE_TYPE_TWO) {
                cansend_framecnt_two[zonenode->ctrlIndex - 2]++;
                moudletem.moudle = ZONE_TYPE_TWO;
                vcanbus_send_start_cmd(moudletem, cansend_framecnt_two[zonenode->ctrlIndex - 2], zonenode->ctrlIndex);
            }
            if (zonenode->beltMoudleIndex == ZONE_TYPE_RISE) {
                cansend_framecnt_rise[zonenode->ctrlIndex - 2]++;
                moudletem.moudle = ZONE_TYPE_RISE;
                vcanbus_send_start_cmd(moudletem, cansend_framecnt_rise[zonenode->ctrlIndex - 2], zonenode->ctrlIndex);
            }

        }

    }
}

u8 recv_msg_check(u8 *point, u16 len)
{
    u16 i = 0;
    u8  sum = 0;

    if((point[0] != 0xAA) || (point[1] != 0xAA))
        return 0;
    if((point[6] | point[7] << 8) != len)
        return 0;
    sum = point[9];
    for(i = 1 ; i < len - 9 ; i++)
    {
        sum ^= point[9 + i];
    }
//    if(sum != point[8])
//        return 0;

    return 1;
}

void recv_msg_process(u8 *point)
{
    MSG_HEAD_DATA *head = (MSG_HEAD_DATA *)point;
    
    if( head->MSG_TYPE == RECV_MSG_BOOT_CMD_TYPE )
    {
        FLASH_Unlock();
        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
        if(FLASH_ErasePage(UserAppEnFlagAddress) == FLASH_COMPLETE)
        {
            FLASH_ProgramHalfWord(UserAppEnFlagAddress,0x0000);
        }
        FLASH_Lock();
        NVIC_SystemReset();
    }

    switch(head->MSG_TYPE)
    {
    case SEND_MSG_BD2PC_BDONLINE_TYPE_ACK:
        funOnlineCtrl.sendcnt = 0;
        funOnlineCtrl.checkcnt = 0;
        break;
    case RECV_MSG_PC2BD_MOTOPARA_CONFIG_TYPE:
        fun_recv_beltMoudle_para(point + sizeof(MSG_HEAD_DATA), head->MSG_LENGTH - 11);
        break;
    case RECV_MSG_PC2BD_MOTOPARA_ACQUIRE_TYPE:
        canbus_read_user_moudlepara(point[11]);
        break;
    case RECV_MSG_PC2BD_ZONE_CONFIG_TYPE:
        funrec_zoneConfig_cmd(point + sizeof(MSG_HEAD_DATA));
        AddSendMsgToQueue(REPLY_RECV_MSG_PC2BD_ZONE_CONFIG_TYPE);
        break;
    case RECV_MSG_PC2BD_PKG_TRANS_TYPE:
        funrecv_pkg_trans_cmd(point + sizeof(MSG_HEAD_DATA));
        AddSendMsgToQueue(REPLY_RECV_MSG_PC2BD_PKG_TRANS_TYPE);
        break;
    case RECV_MSG_PC2BD_PKG_ALLOW_FORBID_TYPE:
        funrecv_pkg_state_cmd(point + sizeof(MSG_HEAD_DATA));
        AddSendMsgToQueue(REPLY_RECV_MSG_PC2BD_PKG_ALLOW_FORBID_TYPE);
        break;
    case RECV_MSG_PC2BD_MOTO_RUN_STOP_CMD_TYPE:
        funrecv_start_stop_moudle(point + sizeof(MSG_HEAD_DATA));
        AddSendMsgToQueue(REPLY_RECV_MSG_PC2BD_MOTO_RUN_STOP_CMD_TYPE);
        break;
    default:
        break;
    }
}
void recv_message_from_server(u8 *point, u16 *len)
{
    if(recv_msg_check(point, *len) == 0)
    {
        *len = 0;
        return;
    }
    recv_msg_process(point);
    *len = 0;
}

void send_normal_relpy_msg(u8* buf, u16* len, u16 type)
{
    u8 sum = (type & 0xFF) ^ (type >> 8);

    buf[0] = 0xAA;
    buf[1] = 0xAA;
    buf[2] = 0x01;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = 0x0B;
    buf[7] = 0x00;
    buf[8] = sum;
    buf[9] = type & 0xFF;
    buf[10] = (type >> 8) & 0xFF;

    *len = 11;
}

void send_bdonline_msg(u8 *buf, u16 *len, u16 type)
{
    u8 sum = 0;
    u16 sendlen = 0;
    u16 i = 0;

    sendlen = 15;

    buf[0] = 0xAA;
    buf[1] = 0xAA;
    buf[2] = 0x01;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = 0x0B;
    buf[7] = 0x00;
    buf[8] = sum;
    buf[9] = type & 0xFF;
    buf[10] = (type >> 8) & 0xFF;

    buf[11] = funOnlineCtrl.version[0];
    buf[12] = funOnlineCtrl.version[0];
    buf[13] = funOnlineCtrl.version[0];
    buf[14] = funOnlineCtrl.version[0];

    sum = 0;
    for (i = 0; i < sendlen - 9; i++) {
        sum ^= buf[9 + i];
    }
    buf[8] = sum;

    *len = sendlen;
}
void send_reply_read_para_cmd(u8 *buf, u16 *len, u16 type)
{
    u8 sum;
    u16 sendlen;
    u16 i;

    sendlen = 11 + canbus_readpara_len;
    buf[9] = type & 0xFF;
    buf[10] = (type >> 8) & 0xFF;
    for(i = 0; i < sendlen - 11 ; i++)
    {
        buf[11 + i] = canbus_readpara_buff[i];
    }
    sum = buf[9];
    for(i = 1 ; i < sendlen - 9 ; i++)
    {
        sum ^= buf[9 + i];
    }
    buf[0] = 0xAA;
    buf[1] = 0xAA;
    buf[2] = 0x01;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = sendlen & 0xFF;
    buf[7] = (sendlen >> 8) & 0xFF;
    buf[8] = sum;

    *len = sendlen;
}

void send_msg_module_status_cmd(u8 *buf, u16 *len, u16 type)
{
    u8 sum;
    u16 sendlen;
    u16 i;

    sendlen = 11 + 2 + (upload_motostate_buff[0] | (upload_motostate_buff[1] << 8)) * 10;
    buf[9] = type & 0xFF;
    buf[10] = (type >> 8) & 0xFF;
    for (i = 0; i < sendlen - 11; i++)
    {
        buf[11 + i] = upload_motostate_buff[i];
    }
    sum = buf[9];
    for (i = 1; i < sendlen - 9; i++)
    {
        sum ^= buf[9 + i];
    }
    buf[0] = 0xAA;
    buf[1] = 0xAA;
    buf[2] = 0x01;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = sendlen & 0xFF;
    buf[7] = (sendlen >> 8) & 0xFF;
    buf[8] = sum;

    *len = sendlen;

}
void send_message_to_server(void)
{
    u16 msg_type;

    if(tcp_client_list[0].tcp_send_en == 0)//tcp���ͳɹ���û�ڷ���
    {
        msg_type = GetSendMsgFromQueue();
    }
    else//tcp���ڷ���
    {
        return;
    }

    switch(msg_type)
    {
    case REPLY_RECV_MSG_PC2BD_MOTOPARA_CONFIG_TYPE:
    case REPLY_RECV_MSG_PC2BD_ZONE_CONFIG_TYPE:
    case REPLY_RECV_MSG_PC2BD_PKG_TRANS_TYPE:
    case REPLY_RECV_MSG_PC2BD_PKG_ALLOW_FORBID_TYPE:
    case REPLY_RECV_MSG_PC2BD_MOTO_RUN_STOP_CMD_TYPE:
        send_normal_relpy_msg(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), msg_type);
        tcp_client_list[0].tcp_send_en = 1;
        break;
    case REPLY_RECV_MSG_PC2BD_MOTOPARA_ACQUIRE_TYPE:
        send_reply_read_para_cmd(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), msg_type);
        tcp_client_list[0].tcp_send_en = 1;
        break;
    case SEND_MSG_BD2PC_BDONLINE_TYPE:
        send_bdonline_msg(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), msg_type);
        tcp_client_list[0].tcp_send_en = 1;
        break;
    case SEDN_MSG_BD2PC_MOTOMOUDLE_STATUS_TYPE:
        send_msg_module_status_cmd(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), msg_type);
        tcp_client_list[0].tcp_send_en = 1;
        break;
    default:
        if(tcp_client_list[0].tcp_client_statue == CLIENT_CONNECT_OK)
        {
//            if( heart_dely == 0 )
//            {
//                send_heart_msg(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), HEART_MSG_TYPE);
//                tcp_client_list[0].tcp_send_en = 1;
//                heart_dely = HEART_DELAY;
//            }
        }
        break;
    }
}

void upload_moudle_state(void)
{
    u16 i = 0;
    u16 num = 0;
    u16 j = 0;
    sData_zone_node  zonenode;
    sData_RealtimeState* statenode;



    if (upload_state_flag == VALUE) {
        upload_pkg_cnt++;
        if (upload_pkg_cnt >= 2) {
            upload_pkg_cnt = 0;
            for (i = zonestate_index; i < BELT_ZONE_NUM; i++) {
                zonenode = zoneConfig.zoneNode[i];
                if (zonenode.zoneIndex != 0xFFFF) {
                    statenode = data_find_ctrl_status(zonenode.ctrlIndex, zonenode.beltMoudleIndex);
                    upload_motostate_buff[10 * j + 2] = zonenode.zoneIndex & 0xFF;
                    upload_motostate_buff[10 * j + 2 + 1] = (zonenode.zoneIndex >> 8) & 0xFF;
                    upload_motostate_buff[10 * j + 2 + 2] = (statenode->zoneState) & 0xFF;
                    upload_motostate_buff[10 * j + 2 + 3] = (statenode->zoneState >> 8) & 0xFF;
                    upload_motostate_buff[10 * j + 2 + 4] = (statenode->zoneAlarm) & 0xFF;
                    upload_motostate_buff[10 * j + 2 + 5] = (statenode->zoneAlarm >> 8) & 0xFF;
                    upload_motostate_buff[10 * j + 2 + 6] = (statenode->zonePkg) & 0xFF;
                    upload_motostate_buff[10 * j + 2 + 7] = (statenode->zonePkg >> 8) & 0xFF;
                    upload_motostate_buff[10 * j + 2 + 8] = (statenode->zonePkg >> 16) & 0xFF;
                    upload_motostate_buff[10 * j + 2 + 9] = (statenode->zonePkg >> 14) & 0xFF;
                    num++;
                    j++;
                    if (num >= 90) break;
                }
            }
            if (i == BELT_ZONE_NUM) {
                upload_state_flag = INVALUE;
                zonestate_index = 0;
                upload_motostate_buff[0] = num;
                upload_motostate_buff[1] = (num >> 8) & 0xFF;

            }
            else {
                zonestate_index = i + 1;
                upload_motostate_buff[0] = num;
                upload_motostate_buff[1] = (num >> 8) & 0xFF;
            }
            AddSendMsgToQueue(SEDN_MSG_BD2PC_MOTOMOUDLE_STATUS_TYPE);
        }
    }
}
