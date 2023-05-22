#ifndef _DATA_H
#define _DATA_H

#include "list.h"
#include "main.h"

#define   BELT_ZONE_NUM                    254

#define   ZONE_NUM                         3

#define   MAX_PKG_NUM                      20

#define   VALUE                            1
#define   INVALUE                          0

#define   FINAL_CAR                       0xFFFF
#define   START_CAR                       0xFFFE


enum {
	ZONE_TYPE_ONE = 0x01,
	ZONE_TYPE_TWO = 0x02,
	ZONE_TYPE_RISE = 0x03,
};


enum {
	TRANS_ALLOW = 0,                      //����ּ�
	TRANS_FORBID = 1,                     //��ֹ�ּ�
	TRANS_ABORT = 2,                      //ȡ���ּ�
	TRANS_SUCCESS = 3,                    //�ɹ��ּ�
};

#pragma pack (1)

//����
//����ڵ�
typedef struct {
	u16  zoneIndex;                        //������  �����վ�� �ޱ�Ȼ��ϵ�� Ĭ��0xFFFF ��Ч;
	u16  ctrlIndex;                        //������վ�� 
	u16  beltMoudleIndex;                  //�����ӻ�����  ��һ����(0x01) �ڶ�����(0x02) ��������0x03)
	u16  front_zone;                       //ǰ������          �涨ǰ������  
	u16  rear_zone;                        //������
	u16  left_zone;                        //������          �涨�����򣨵��Ͳ��ת ע�ⰲװλ�ã�   
	u16  right_zone;                       //�ҷ�����  
}sData_zone_node;

//��������
typedef struct {
	sData_zone_node  zoneNode[BELT_ZONE_NUM];     //����ڵ���Ϣ
}sData_zone_config;

//���������Ϣ


//���ø����������
typedef struct {
	u16 zoneIndex;                        //ģ��1(0x01) ģ��2(0x02)  ����ģ��(0xA0)(����ģ�鲻��Ҫ����) 0xFFΪĬ��ֵ
	u16 FuncSeclet;                       //��������
	u16 speed;                            //�ٶȵ�λ 4��	
	u16 dir;                              //����  Ĭ��cw����
	u16 ramp;                             //�Ӽ��ٶȵ�λ
}sData_belt_node;
//ÿһ�����������
typedef struct {
	u8  version[4];                        //ģ��汾�ŵ��ֽ�
	u16 slaverIndex;                       //������վ��
	u16 slaver_zonenum;                    //�ôӻ���Ч�ĵ���������
	sData_belt_node  belt_node[ZONE_NUM];  //�ӻ��������
}sData_belt_msg;


//ʵʱ��Ϣ
typedef struct {
	u16 zoneIndex;                     //������
	u16 zoneState;                     //����״̬ bit0����״̬   Bit1������1����  bit2 ������2���� 
	                                   // bit3 �޻� bit4�л� bit5 ���ڳ��� bit6�ȴ��ӻ�  bit7����ɹ����䵽λ bit8 ����������2-0����(��Ӧ���Ͳ1��
	                                   // bit9 ����������2-1���������Ͳ1��bit10 ����������2-2���������Ͳ2��bit11 ����������2-3���������Ͳ2��
	                                   // bit12 �������޹�紥�� bit13 �������޹�紥��
	u16 zoneAlarm;                     //����״̬ Bit0 �������/δ����  bit1 Ƿѹ bit 2 ��ѹ bit3����
	                                   //bit4 ������1����  Bit5������2����  bit6 ��ģ��δ����  Bit8 ����ģ����1/δ����  Bit9 ����ģ��������2/δ����  Bit10 ����ģ��������3/δ���� bit11 ����������1-1����(��Ӧ���Ͳ1��
	                                   // bit12 ����������1-2���ϣ����Ͳ1��bit13 ����������2-1���ϣ����Ͳ2��bit14 ����������2-2���ϣ����Ͳ2��
	u32 zonePkg;                       //������� Ԥ��  Ĭ��0
	u16 transsuccess;                  //�ɹ��ּ�
}sData_RealtimeState;

typedef struct {
	u16 ctrlindex;
	u16 moudlenum;
	sData_RealtimeState  node[ZONE_NUM];
}sData_Realtime_statenode;

typedef struct {
	sData_Realtime_statenode  state[BELT_ZONE_NUM];
}sbelt_Moudle_state;



#pragma pack ()

//������Ϣ
typedef struct {
	struct xLIST_ITEM* index;
	u32 pkgId;                              //������
	u16 totalzoneNum;                       //������Ҫ;������������
	u16 zoneindex[BELT_ZONE_NUM];           //����;���ľ���·��
	u16 lastZonenum;                        //������һ��������������
	u16 curZonenum;                         //��ǰ����������������
	u16 curZoneIndex;                       //��ǰ��������
	u16 curZoneStatus;                      //�����ڵ�ǰ�����״̬
	u16 curZoneCnt;                         //�������ڵ�ǰ�����ʱ��
	u16 curZonedir;                         //�����ڵ�ǰ����Ĵ��䷽��
	u16 curZoneruntype;                     //��ǰ���������������
	u16 nextZoneIndex;                      //��ǰ������һ������ı��
	u16 nextZoneStatus;                     //��������ǰ������һ�������״̬
	u16 nextZoneCnt;                        //��ǰ������һ������Ľӻ�ʱ��
	u16 nextZonedir;                        //������ǰ������һ������Ĵ��䷽��
	u16 nextZoneruntype;                    //ǰ������һ������İ�����������
	u16 theThirdZone;                       //��һ���������һ������
    u16 allowState;                         //��ֹ���䡢�������͡�ȡ������
	u16 sendcmdcnt;                         //���Ӱ忨������ͣ�����ʱ
}sData_pkg_node;



//����������Ϣ
extern sData_zone_config     zoneConfig;
//��Ͳ���ò���
extern sData_belt_msg        beltMoudlepara;
//��Ͳģ��ʵʱ����
extern sbelt_Moudle_state    beltMoudlestate;


extern sData_pkg_node        pgkNodeDateItem[];
extern struct xLIST_ITEM     pkgNodeListItem[];

void data_msg_init(void);
void data_pkg_list_init(void);
void data_addto_pkg_list(sData_pkg_node x);
sData_zone_node* data_find_zone_moudlestate(u16 index);
sData_RealtimeState* data_find_ctrl_status(u16 ctrlindex, u16 moudleindex);
u8 data_find_nearzone_direction(sData_zone_node node, u16 nextzone);
u8 data_config_moudle_rundir(u8 predir, u8 nextdir);


#endif