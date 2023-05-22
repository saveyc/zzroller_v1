#ifndef __CAN_BUS_H
#define __CAN_BUS_H

#define CAN_PACK_DATA_LEN     8
#define CAN_TX_BUFF_SIZE      300
#define CAN_RX_BUFF_SIZE      300

enum
{
    CAN_FUNC_ID_PARA_DATA = 0x1,         //�û���������(��д)
    CAN_FUNC_ID_MODULE_STATUS = 0x2,     //ģ��ʵʱ״̬��Ϣ(����ָ��)
    CAN_FUNC_ID_START_STOP_CMD = 0x3,    //��ͣ����
    CAN_FUNC_ID_BOOT_MODE = 0xF          //boot
};

enum
{
    CAN_SEG_POLO_NONE   = 0,
    CAN_SEG_POLO_FIRST  = 1,
    CAN_SEG_POLO_MIDDLE = 2,
    CAN_SEG_POLO_FINAL  = 3
};


//ת������
enum {
    RUN_DIRDEFAULT = 0,
    RUN_AHEAD_TOBACK = 1,
    RUN_BACK_TOAHEAD = 2,
    RUN_LEFT_TORIGHT = 3,
    RUN_RIGHT_TOLEFT = 4,
    RUN_AHEAD_TOLEFT = 5,
    RUN_AHEAD_TORIGHT = 6,
    RUN_BACK_TOLEFT = 7,
    RUN_BACK_TORIGHT = 8,
    RUN_LEFT_TOAHEAD = 9,
    RUN_LEFT_TOBACK = 10,
    RUN_RIGHT_TOAHEAD = 11,
    RUN_RIGHT_TOBACK = 12,
};

//��ͣ������
enum {
    RUN_DEFAULT = 0,
    RUN_CMD = 1,
    STOP_CMD = 2,
};

//��ͣ��ʽ
enum {
    RUN_TYPEDEFAULT = 0,
    CONTINUE_RUN = 1,
    RUN_TRIGSTOP = 2,
};

//3������������
enum {
    ZONE_TYPE_ONE = 0x01,
    ZONE_TYPE_TWO = 0x02,
    ZONE_TYPE_RISE = 0x03,
};


#pragma pack (1)
/*
|    28~21    |    20~19      |    18~12     |     11~8     |    7~0      |    ExtID
|  dst_id(8)  |  seg_polo(2)  |  seg_num(7)  |  func_id(4)  |  src_id(8)  |
*/

typedef struct
{
    u8 seg_polo;
    u8 seg_num;
    u8 func_id;
    u8 src_id;
    u8 dst_id;
} sCanFrameExtID;

typedef struct
{
    sCanFrameExtID extId;
    u8 data_len;
    u8 data[8];
} sCanFrameExt;


#pragma pack ()

typedef struct {
    sCanFrameExt *queue; /* ָ��洢���е�����ռ� */
    u16 front, rear, len; /* ����ָ�루�±꣩����βָ�루�±꣩�����г��ȱ��� */
    u16 maxSize; /* queue���鳤�� */
} sCAN_SEND_QUEUE;




void InitCanSendQueue(void);
void can_bus_frame_receive(CanRxMsg rxMsg);
void can_bus_send_msg(u8* pbuf, u16 send_tot_len, u8 func_id, u8 dst_id);
void can_send_frame_process();

#endif