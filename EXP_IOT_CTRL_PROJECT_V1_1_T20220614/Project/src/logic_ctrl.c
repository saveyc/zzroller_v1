#include "main.h"
#include "logic_ctrl.h"

u16 cansend_framecnt_one[BELT_ZONE_NUM] = {0};
u16 cansend_framecnt_two[BELT_ZONE_NUM] = {0};
u16 cansend_framecnt_rise[BELT_ZONE_NUM] = {0};



// ������������߼� 1msִ��1��
void logic_pkg_trans_process(void)
{
	List_t* q = NULL;
	u16 itemvalue = 0;
	sData_pkg_node* pkgnode = NULL;
	sData_zone_node* prenode = NULL;
	sData_zone_node* curnode = NULL;
	sData_zone_node* nextnode = NULL;
	sData_RealtimeState* prestatenode = NULL;
	sData_RealtimeState* curstatenode = NULL;
	sData_RealtimeState* nextstatenode = NULL;
	sMoudle_cmd  moudlecmdcur;
	sMoudle_cmd  moudlecmdnext;
	sMoudle_cmd  moudletmp;
	u8 predir = DIR_NONE;
	u8 nextdir = DIR_NONE;


	moudlecmdcur.cmd = RUN_DEFAULT;
	moudlecmdnext.cmd = RUN_DEFAULT;
	moudletmp.cmd = RUN_DEFAULT;


	q = &pkg_list;
	// 
	itemvalue = q->xListEnd.xItemValue;

 	for (q->pxIndex = (ListItem_t*)(q->xListEnd.pxNext); q->pxIndex->xItemValue != itemvalue; q->pxIndex = q->pxIndex->pxNext)
	{
		pkgnode = q->pxIndex->pvOwner;
		if (pkgnode->allowState == TRANS_ALLOW) {      
			if (pkgnode->curZonenum <= (pkgnode->totalzoneNum - 1)) {
				pkgnode->sendcmdcnt++;
				//����ǰһ���������Ϣ
				if (pkgnode->curZonenum != 0) {
					prenode = data_find_zone_moudlestate(pkgnode->zoneindex[pkgnode->curZonenum - 1]);
					prestatenode = data_find_ctrl_status(prenode->ctrlIndex, prenode->beltMoudleIndex);
					prestatenode->zonePkg = 0;
					prestatenode->transsuccess = INVALUE;
					prestatenode->zoneState &= 0xFF0F;
				}
				else {
					prenode = NULL;
				}

			    //���µ�ǰ������Ϣ
				pkgnode->curZoneIndex = pkgnode->zoneindex[pkgnode->curZonenum];
				curnode = data_find_zone_moudlestate(pkgnode->curZoneIndex);
				curstatenode = data_find_ctrl_status(curnode->ctrlIndex,curnode->beltMoudleIndex);



				//������һ������Ϣ
				if ((pkgnode->curZonenum) >= (pkgnode->totalzoneNum - 1)) {
					pkgnode->nextZoneIndex = FINAL_CAR;
					nextnode = NULL;
					nextstatenode = NULL;
				}
				else {
					pkgnode->nextZoneIndex = pkgnode->zoneindex[pkgnode->curZonenum + 1];
					nextnode = data_find_zone_moudlestate(pkgnode->nextZoneIndex);
					nextstatenode = data_find_ctrl_status(nextnode->ctrlIndex, nextnode->beltMoudleIndex);
				}

				//������һ���������һ������
				if ((pkgnode->curZonenum) >= (pkgnode->totalzoneNum - 2)) {
					pkgnode->theThirdZone = FINAL_CAR;
				}
				else {
					pkgnode->theThirdZone = pkgnode->zoneindex[pkgnode->curZonenum + 2];
				}

				//���㵱ǰ����Ĵ��䷽��
				if (pkgnode->curZonenum == 0) {
					predir = data_find_nearzone_direction(*curnode, FINAL_CAR);
				}
				else {
			        predir = data_find_nearzone_direction(*curnode, pkgnode->zoneindex[pkgnode->curZonenum - 1]);
				}
				nextdir = data_find_nearzone_direction(*curnode, pkgnode->nextZoneIndex);
				pkgnode->curZonedir = data_config_moudle_rundir(predir, nextdir);
				
				//������һ����ǰ������Ĵ��䷽��
				if (pkgnode->nextZoneIndex != FINAL_CAR) {
					predir = data_find_nearzone_direction(*nextnode, pkgnode->curZoneIndex);
					nextdir = data_find_nearzone_direction(*nextnode, pkgnode->theThirdZone);
					pkgnode->nextZonedir = data_config_moudle_rundir(predir, nextdir);
				}

				//��ǰ����״̬����
				//�Ѿ��������һ������ 
				if ((pkgnode->curZonenum) == (pkgnode->totalzoneNum - 1)) {
					curstatenode->zoneState &= 0xFF17;
					curstatenode->zonePkg = pkgnode->pkgId;
					if (((((curstatenode->zoneState >> 2) & 0x1) == 1) && (pkgnode->curZonedir == RUN_BACK_TOAHEAD))
						|| ((((curstatenode->zoneState >> 1) & 0x1) == 1) && (pkgnode->curZonedir == RUN_AHEAD_TOBACK))) {
						     
						pkgnode->allowState = TRANS_SUCCESS;
						if (curstatenode->transsuccess == INVALUE) {
							// ����ǰ��ģ���ط�ָֹͣ��
							if (curnode->beltMoudleIndex == ZONE_TYPE_ONE) {
								cansend_framecnt_one[curnode->ctrlIndex - 2]++;
								moudletmp.cmd = STOP_CMD;
								moudletmp.moudle = ZONE_TYPE_ONE;
								vcanbus_send_start_cmd(moudletmp, cansend_framecnt_one[curnode->ctrlIndex - 2], curnode->ctrlIndex);

							}
							if (curnode->beltMoudleIndex == ZONE_TYPE_TWO) {
								cansend_framecnt_two[curnode->ctrlIndex - 2]++;
								moudletmp.cmd = STOP_CMD;
								moudletmp.moudle = ZONE_TYPE_TWO;
								vcanbus_send_start_cmd(moudletmp, cansend_framecnt_two[curnode->ctrlIndex - 2], curnode->ctrlIndex);
                                                                                                                                                                                                                                                   
							}

							//��ǰһ��ģ�鷢ָֹͣ��
							if (prenode != NULL) {
								if (prenode->beltMoudleIndex == ZONE_TYPE_ONE) {
									cansend_framecnt_one[prenode->ctrlIndex - 2]++;
									moudletmp.cmd = STOP_CMD;
									moudletmp.moudle = ZONE_TYPE_ONE;
									vcanbus_send_start_cmd(moudletmp, cansend_framecnt_one[prenode->ctrlIndex - 2], prenode->ctrlIndex);

								}
								if (prenode->beltMoudleIndex == ZONE_TYPE_TWO) {
									cansend_framecnt_two[prenode->ctrlIndex - 2]++;
									moudletmp.cmd = STOP_CMD;
									moudletmp.moudle = ZONE_TYPE_TWO;
									vcanbus_send_start_cmd(moudletmp, cansend_framecnt_two[prenode->ctrlIndex - 2], prenode->ctrlIndex);
								}
								if (prenode->beltMoudleIndex == ZONE_TYPE_RISE) {
									cansend_framecnt_rise[prenode->ctrlIndex - 2]++;
									moudletmp.cmd = STOP_CMD;
									moudletmp.moudle = ZONE_TYPE_RISE;
									vcanbus_send_start_cmd(moudletmp, cansend_framecnt_rise[prenode->ctrlIndex - 2], prenode->ctrlIndex);
								}
							}
							curstatenode->zonePkg = 0;
							curstatenode->zoneState &= 0;
//							curstatenode->transsuccess = VALUE;

						}
                        //��Ҫ���ǳɹ��ּ��Ժ� ���ϴ���ִ��ʲô����
						uxListRemove(pkgnode->index);
						continue;
//						return;
					}
				}
				else {
					curstatenode->zoneState &= 0xFF17;
					curstatenode->zonePkg = pkgnode->pkgId;
					if (((pkgnode->curZonedir == RUN_AHEAD_TOBACK) || (pkgnode->curZonedir == RUN_LEFT_TOBACK)
						|| (pkgnode->curZonedir == RUN_RIGHT_TOBACK)) && ((((curstatenode->zoneState >> 1) & 0x1) == 1) 
							|| (((curstatenode->zoneState >> 8) & 0x1) == 1))) {
						curstatenode->zoneState &= 0xFF27;
					}
					if (((pkgnode->curZonedir == RUN_BACK_TOAHEAD) || (pkgnode->curZonedir == RUN_LEFT_TOAHEAD)
						|| (pkgnode->curZonedir == RUN_RIGHT_TOAHEAD)) && ((((curstatenode->zoneState >> 2) & 0x1) == 1) 
							|| (((curstatenode->zoneState >> 9) & 0x1) == 1))) {
						curstatenode->zoneState &= 0xFF27;
					}
					if (((pkgnode->curZonedir == RUN_RIGHT_TOLEFT) || (pkgnode->curZonedir == RUN_AHEAD_TOLEFT)
						|| (pkgnode->curZonedir == RUN_BACK_TOLEFT)) && (((curstatenode->zoneState >> 10) & 0x1) == 1)) {
						curstatenode->zoneState &= 0xFF27;
					}
					if (((pkgnode->curZonedir == RUN_LEFT_TORIGHT) || (pkgnode->curZonedir == RUN_AHEAD_TORIGHT)
						|| (pkgnode->curZonedir == RUN_BACK_TORIGHT)) && (((curstatenode->zoneState >> 11) & 0x1) == 1)) {
						curstatenode->zoneState &= 0xFF27;
					}
				}

				//������һ�������״̬
				if (pkgnode->nextZoneIndex != FINAL_CAR) {
					nextstatenode->transsuccess = INVALUE;
					nextstatenode->zonePkg = 0;
					nextstatenode->zoneState  &= 0xFF47;

					//�жϲ��л�����
					if ((((nextstatenode->zoneState >> 1) & 0x1) == 1) || (((nextstatenode->zoneState >> 2) & 0x1) == 1)
						|| (((nextstatenode->zoneState >> 8) & 0x1) == 1) || (((nextstatenode->zoneState >> 9) & 0x1) == 1)
						|| (((nextstatenode->zoneState >> 10) & 0x1) == 1) || (((nextstatenode->zoneState >> 11) & 0x1) == 1)) {

						if (prenode != NULL) {
							//ֹͣǰһ��������
							if (prenode->beltMoudleIndex == ZONE_TYPE_ONE) {
								cansend_framecnt_one[prenode->ctrlIndex - 2]++;
								moudletmp.cmd = STOP_CMD;
								moudletmp.moudle = ZONE_TYPE_ONE;
								vcanbus_send_start_cmd(moudletmp, cansend_framecnt_one[prenode->ctrlIndex - 2], prenode->ctrlIndex);

							}
							if (prenode->beltMoudleIndex == ZONE_TYPE_TWO) {
								cansend_framecnt_two[prenode->ctrlIndex - 2]++;
								moudletmp.cmd = STOP_CMD;
								moudletmp.moudle = ZONE_TYPE_TWO;
								vcanbus_send_start_cmd(moudletmp, cansend_framecnt_two[prenode->ctrlIndex - 2], prenode->ctrlIndex);

							}
							if (prenode->beltMoudleIndex == ZONE_TYPE_RISE) {
								cansend_framecnt_rise[prenode->ctrlIndex - 2]++;
								moudletmp.cmd = STOP_CMD;
								moudletmp.moudle = ZONE_TYPE_RISE;
								vcanbus_send_start_cmd(moudletmp, cansend_framecnt_rise[prenode->ctrlIndex - 2], prenode->ctrlIndex);
							}
						}

						pkgnode->curZonenum++;
						continue;
//						return;
					}
				}

				//��ǰ����Ҫ���͵��������
				if ((pkgnode->curZonenum) >= (pkgnode->totalzoneNum - 1)) {
					moudlecmdcur.moudle = curnode->beltMoudleIndex;
					moudlecmdcur.cmd = RUN_CMD;
					moudlecmdcur.dir = pkgnode->curZonedir;
					moudlecmdcur.type = RUN_TRIGSTOP;
					
					moudlecmdnext.cmd = RUN_DEFAULT;

				}
				else if((pkgnode->curZonenum) == (pkgnode->totalzoneNum - 2)){
					moudlecmdcur.moudle = curnode->beltMoudleIndex;
					moudlecmdcur.cmd = RUN_CMD;
					moudlecmdcur.dir = pkgnode->curZonedir;
					moudlecmdcur.type = CONTINUE_RUN;

					moudlecmdnext.moudle = nextnode->beltMoudleIndex;
					moudlecmdnext.cmd = RUN_CMD;
					moudlecmdnext.dir = pkgnode->nextZonedir;
					moudlecmdnext.type = RUN_TRIGSTOP;                           
				}
				else {
					moudlecmdcur.moudle = curnode->beltMoudleIndex;
					moudlecmdcur.cmd = RUN_CMD;
					moudlecmdcur.dir = pkgnode->curZonedir;
					moudlecmdcur.type = CONTINUE_RUN;

					moudlecmdnext.moudle = nextnode->beltMoudleIndex;
					moudlecmdnext.cmd = RUN_CMD;
					moudlecmdnext.dir = pkgnode->nextZonedir;
					moudlecmdnext.type = CONTINUE_RUN;
				}

				if (pkgnode->curZonenum != pkgnode->lastZonenum)
				{
					pkgnode->lastZonenum = pkgnode->curZonenum;

					//����ǰ��ģ�鷢������ָ��
					if (moudlecmdcur.cmd != RUN_DEFAULT) {
						if (moudlecmdcur.moudle == ZONE_TYPE_ONE) {
							cansend_framecnt_one[curnode->ctrlIndex - 2]++;
							vcanbus_send_start_cmd(moudlecmdcur, cansend_framecnt_one[curnode->ctrlIndex - 2], curnode->ctrlIndex);
						}
						if (moudlecmdcur.moudle == ZONE_TYPE_TWO) {
							cansend_framecnt_two[curnode->ctrlIndex - 2]++;
							vcanbus_send_start_cmd(moudlecmdcur, cansend_framecnt_two[curnode->ctrlIndex - 2], curnode->ctrlIndex);
						}
						if (moudlecmdcur.moudle == ZONE_TYPE_RISE) {
							cansend_framecnt_rise[curnode->ctrlIndex - 2]++;
							vcanbus_send_start_cmd(moudlecmdcur, cansend_framecnt_rise[curnode->ctrlIndex - 2], curnode->ctrlIndex);
						}
					}

					//����һ��ģ�鷢������ָ��
					if (moudlecmdnext.cmd != RUN_DEFAULT) {
						if (moudlecmdnext.moudle == ZONE_TYPE_ONE) {
							cansend_framecnt_one[nextnode->ctrlIndex - 2]++;
							vcanbus_send_start_cmd(moudlecmdnext, cansend_framecnt_one[nextnode->ctrlIndex - 2], nextnode->ctrlIndex);
						}
						if (moudlecmdnext.moudle == ZONE_TYPE_TWO) {
							cansend_framecnt_two[nextnode->ctrlIndex - 2]++;
							vcanbus_send_start_cmd(moudlecmdnext, cansend_framecnt_two[nextnode->ctrlIndex - 2], nextnode->ctrlIndex);
						}
						if (moudlecmdnext.moudle == ZONE_TYPE_RISE) {
							cansend_framecnt_rise[nextnode->ctrlIndex - 2]++;
							vcanbus_send_start_cmd(moudlecmdnext, cansend_framecnt_rise[nextnode->ctrlIndex - 2], nextnode->ctrlIndex);
						}
					}

					pkgnode->sendcmdcnt = 0;

				}

				if (pkgnode->sendcmdcnt >= SEND_STARTCMD_CNT) {
					pkgnode->sendcmdcnt = 0;

					//����ǰ��ģ�鷢������ָ��
					if (moudlecmdcur.cmd != RUN_DEFAULT) {
						if (moudlecmdcur.moudle == ZONE_TYPE_ONE) {
							vcanbus_send_start_cmd(moudlecmdcur, cansend_framecnt_one[curnode->ctrlIndex - 2], curnode->ctrlIndex);
						}
						if (moudlecmdcur.moudle == ZONE_TYPE_TWO) {
							vcanbus_send_start_cmd(moudlecmdcur, cansend_framecnt_two[curnode->ctrlIndex - 2], curnode->ctrlIndex);
						}
						if (moudlecmdcur.moudle == ZONE_TYPE_RISE) {
							vcanbus_send_start_cmd(moudlecmdcur, cansend_framecnt_rise[curnode->ctrlIndex - 2], curnode->ctrlIndex);
						}
					}

					//����һ��ģ�鷢������ָ��
					if (moudlecmdnext.cmd != RUN_DEFAULT) {
						if (moudlecmdnext.moudle == ZONE_TYPE_ONE) {
							vcanbus_send_start_cmd(moudlecmdnext, cansend_framecnt_one[nextnode->ctrlIndex - 2], nextnode->ctrlIndex);
						}
						if (moudlecmdnext.moudle == ZONE_TYPE_TWO) {
							vcanbus_send_start_cmd(moudlecmdnext, cansend_framecnt_two[nextnode->ctrlIndex - 2], nextnode->ctrlIndex);
						}
						if (moudlecmdnext.moudle == ZONE_TYPE_RISE) {
							vcanbus_send_start_cmd(moudlecmdnext, cansend_framecnt_rise[nextnode->ctrlIndex - 2], nextnode->ctrlIndex);
						}
					}
				}
			}
		}
	}
}





