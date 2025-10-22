#ifndef OBJECTIVELENSCONVERT_H_
#define OBJECTIVELENSCONVERT_H_

#define DLL_EXPORTS extern "C" __declspec(dllexport)
#define DLL_CLASSEXP __declspec(dllexport)
#include "dataStructure.h"
namespace objectivelib
{
	/**
	 * @brief 测试dump接口，实现一个野指针赋值触发崩溃
	 */

	DLL_EXPORTS void OBJECTIVELIB_DumpTrigger();

	/**
	 * @brief 项目初始化操作
	 */
	DLL_EXPORTS void OBJECTIVELIB_Init();


/*
返回:  true已经连接,false未连接
*/
	DLL_EXPORTS bool OBJ_InitIsOpen();    //是否已经初始化


	DLL_EXPORTS void OBJ_SetQtObject(void *ptr);    //设置QT信号槽地址

/*
返回:  0         //正常
返回:  1         //失败
*/
	DLL_EXPORTS int OBJ_GetDeviceNum(int &deviceNum);	//获取驱动总数


	/*
返回:  0         //正常
返回:  1         //连接传感器的deviceId要大于等于0，且小于等于MAX_DEVICEID_NUM
返回:  2         //失败
*/
	DLL_EXPORTS int OBJ_GetDeviceInfo(int deviceId, char* strDeviceId, char* strFriendlyName, char* strDriverVersion);//获取驱动信息

/*
返回:  0         //正常
返回:  1         //连接传感器的deviceId要大于等于0，且小于等于MAX_DEVICEID_NUM
返回:  2         //连接失败
返回:  3         //连接已经存在
*/
	DLL_EXPORTS int OBJ_EthernetOpen(int deviceId, char* ip, int port);    //连接传感器

	/*
返回:  true已经连接,false未连接
*/
	DLL_EXPORTS bool OBJ_EthernetIsOpen(int deviceId);    //是否已经连接传感器

/*
返回:  0         //正常
返回:  1         //连接传感器的deviceId要大于等于0，且小于等于MAX_DEVICEID_NUM
*/
	DLL_EXPORTS int OBJ_CommunicationClose(int deviceId); //断开传感器

/*
返回:  0         //移动到孔位(非阻塞式)正常
返回:  1         //连接传感器的deviceId要大于等于0，且小于等于MAX_DEVICEID_NUM
返回:  2         //未连接
返回:  3		 //移动失败
*/
	DLL_EXPORTS int OBJ_RevolverMoveTo(int deviceId,char *objHolePos); //移动到孔位

/*
返回:  0         //正常
返回:  1         //连接传感器的deviceId要大于等于0，且小于等于MAX_DEVICEID_NUM
返回:  2         //未连接
返回:  3		 //获取失败
*/
	DLL_EXPORTS int OBJ_GetPosition(int deviceId, char *objHolePos); //获取当前孔位


/*
返回:  0         //正常
返回:  1         //连接传感器的deviceId要大于等于0，且小于等于MAX_DEVICEID_NUM
返回:  2         //未连接
返回:  3		 //获取失败
*/
	DLL_EXPORTS int OBJ_IsPosition(int deviceId, bool &isready); //是否移动到位
}
#endif
