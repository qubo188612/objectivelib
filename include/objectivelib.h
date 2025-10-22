#ifndef OBJECTIVELENSCONVERT_H_
#define OBJECTIVELENSCONVERT_H_

#define DLL_EXPORTS extern "C" __declspec(dllexport)
#define DLL_CLASSEXP __declspec(dllexport)
#include "dataStructure.h"
namespace objectivelib
{
	/**
	 * @brief ����dump�ӿڣ�ʵ��һ��Ұָ�븳ֵ��������
	 */

	DLL_EXPORTS void OBJECTIVELIB_DumpTrigger();

	/**
	 * @brief ��Ŀ��ʼ������
	 */
	DLL_EXPORTS void OBJECTIVELIB_Init();


/*
����:  true�Ѿ�����,falseδ����
*/
	DLL_EXPORTS bool OBJ_InitIsOpen();    //�Ƿ��Ѿ���ʼ��


	DLL_EXPORTS void OBJ_SetQtObject(void *ptr);    //����QT�źŲ۵�ַ

/*
����:  0         //����
����:  1         //ʧ��
*/
	DLL_EXPORTS int OBJ_GetDeviceNum(int &deviceNum);	//��ȡ��������


	/*
����:  0         //����
����:  1         //���Ӵ�������deviceIdҪ���ڵ���0����С�ڵ���MAX_DEVICEID_NUM
����:  2         //ʧ��
*/
	DLL_EXPORTS int OBJ_GetDeviceInfo(int deviceId, char* strDeviceId, char* strFriendlyName, char* strDriverVersion);//��ȡ������Ϣ

/*
����:  0         //����
����:  1         //���Ӵ�������deviceIdҪ���ڵ���0����С�ڵ���MAX_DEVICEID_NUM
����:  2         //����ʧ��
����:  3         //�����Ѿ�����
*/
	DLL_EXPORTS int OBJ_EthernetOpen(int deviceId, char* ip, int port);    //���Ӵ�����

	/*
����:  true�Ѿ�����,falseδ����
*/
	DLL_EXPORTS bool OBJ_EthernetIsOpen(int deviceId);    //�Ƿ��Ѿ����Ӵ�����

/*
����:  0         //����
����:  1         //���Ӵ�������deviceIdҪ���ڵ���0����С�ڵ���MAX_DEVICEID_NUM
*/
	DLL_EXPORTS int OBJ_CommunicationClose(int deviceId); //�Ͽ�������

/*
����:  0         //�ƶ�����λ(������ʽ)����
����:  1         //���Ӵ�������deviceIdҪ���ڵ���0����С�ڵ���MAX_DEVICEID_NUM
����:  2         //δ����
����:  3		 //�ƶ�ʧ��
*/
	DLL_EXPORTS int OBJ_RevolverMoveTo(int deviceId,char *objHolePos); //�ƶ�����λ

/*
����:  0         //����
����:  1         //���Ӵ�������deviceIdҪ���ڵ���0����С�ڵ���MAX_DEVICEID_NUM
����:  2         //δ����
����:  3		 //��ȡʧ��
*/
	DLL_EXPORTS int OBJ_GetPosition(int deviceId, char *objHolePos); //��ȡ��ǰ��λ


/*
����:  0         //����
����:  1         //���Ӵ�������deviceIdҪ���ڵ���0����С�ڵ���MAX_DEVICEID_NUM
����:  2         //δ����
����:  3		 //��ȡʧ��
*/
	DLL_EXPORTS int OBJ_IsPosition(int deviceId, bool &isready); //�Ƿ��ƶ���λ
}
#endif
