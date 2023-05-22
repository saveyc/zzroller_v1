#ifndef _FUN_H
#define _FUN_H

/********�����**********/
#define		MSG_NULL_TYPE			                     0x0000
/**********������������*******/
#define		SEND_MSG_BD2PC_BDONLINE_TYPE                  0x1001        //������������Ϣ
#define     SEDN_MSG_BD2PC_MOTOMOUDLE_STATUS_TYPE         0x1008        //ģ��״̬��Ϣ

/**********�������͵�����Ļظ�******/
#define		SEND_MSG_BD2PC_BDONLINE_TYPE_ACK              0x9001

/**********��������**********/
#define     RECV_MSG_PC2BD_MOTOPARA_CONFIG_TYPE           0x1002        //���Ͳ������Ϣ
#define     RECV_MSG_PC2BD_MOTOPARA_ACQUIRE_TYPE          0x1003        //��ȡ���Ͳ������Ϣ
#define     RECV_MSG_PC2BD_ZONE_CONFIG_TYPE               0x1004        //��������
#define     RECV_MSG_PC2BD_PKG_TRANS_TYPE                 0x1005        //��������ָ��
#define     RECV_MSG_PC2BD_PKG_ALLOW_FORBID_TYPE          0x1007        //�����Ƿ���������ָ��
#define     RECV_MSG_PC2BD_MOTO_RUN_STOP_CMD_TYPE         0x1009        //��ͣ���Ͳָ��

#define     RECV_MSG_BOOT_CMD_TYPE                        0x1F01        //����BOOT
/**********�ظ���������******/
#define		REPLY_RECV_MSG_PC2BD_MOTOPARA_CONFIG_TYPE     0x9002
#define     REPLY_RECV_MSG_PC2BD_MOTOPARA_ACQUIRE_TYPE    0x9003
#define     REPLY_RECV_MSG_PC2BD_ZONE_CONFIG_TYPE         0x9004
#define     REPLY_RECV_MSG_PC2BD_PKG_TRANS_TYPE           0x9005    
#define     REPLY_RECV_MSG_PC2BD_PKG_ALLOW_FORBID_TYPE    0x9007   
#define     REPLY_RECV_MSG_PC2BD_MOTO_RUN_STOP_CMD_TYPE   0x9009        

/**********�ظ���������******/

#pragma pack (1) 
typedef struct {
    u8  MSG_TAG[2];
    u32 MSG_ID;
    u16 MSG_LENGTH;
    u8  MSG_CRC;
    u16 MSG_TYPE;
}MSG_HEAD_DATA;

typedef struct {
    u16 sendcnt;                        //�������������ʱ
    u16 checkcnt;                       //�Լ����
    u8 version[4];                      //�汾��
}sFun_Online_ctrl;

#pragma pack () 

typedef struct {
        u16 *queue; /* ָ��洢���е�����ռ� */
        u16 front, rear, len; /* ����ָ�루�±꣩����βָ�루�±꣩�����г��ȱ��� */
        u16 maxSize; /* queue���鳤�� */
}MSG_SEND_QUEUE;

extern sFun_Online_ctrl    funOnlineCtrl;

extern u8 upload_motostate_buff[];
extern u16 zonestate_index;
extern u16 upload_state_flag;
extern u16 upload_state_cnt;
extern u16 upload_pkg_cnt;


void InitSendMsgQueue(void);
void AddSendMsgToQueue(u16 msg);
u16 GetSendMsgFromQueue(void);
void recv_message_from_server(u8 *point,u16 *len);
void send_message_to_server(void);
void upload_moudle_state(void);

void fun_struct_init(void);

#endif
