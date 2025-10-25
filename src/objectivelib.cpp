#include "objectivelib.h"
#include "commitid.h"
#include "alg_base_common.h"
#include "pthread.h"
#include <mutex>
#include "XTcp.h"
#include <QVariantMap>
#include <QThread>

namespace  objectivelib
{
	static bool b_sdk_init = false;

	#define USE_TYPE				2			//�ͺ�:0�����ͺ�,1�ɿ��ͺ�,2qt�ź�
	#define MAX_DEVICEID_NUM        10          //�������10̨�ﾵת����
	#define RECVBUFFER_FTP_MAX		255			//����TCP����
	#define TCP_TIMEOUT			    0.5			//TCP�շ�ʱ��
	
	static bool link_ftp_state[MAX_DEVICEID_NUM];		//�Ƿ��������ﾵת����
	static std::mutex mtx; // ȫ�ֻ�����
	static void *m_qtobject;//qt�źŲ۵�ַ
	struct struct_errinfo
	{
		bool b_err;//�Ƿ����
		std::string str;//������Ϣ
	};
	static struct_errinfo m_errinfo[MAX_DEVICEID_NUM];	    //ÿ̨�ﾵת������Ϣ
	struct struct_objectiveinfo
	{
		int objeccount;//�ܿ�λ��
		std::string position;//����λ��
		bool moveing;//����Ƿ��ٹ�����
		std::string targetposition;//Ŀ���ƶ�λ��
	};
	static struct_objectiveinfo m_objectiveinfo[MAX_DEVICEID_NUM];//�ﾵת������Ϣ

#if USE_TYPE == 0
	static pthread_t _virtualobjectivethread[MAX_DEVICEID_NUM];
	void *_virtualobjective(void *des);
	static bool b_virtualobjectivethread_stop[MAX_DEVICEID_NUM];
	static bool b_virtualobjectivethread[MAX_DEVICEID_NUM];
	struct Paramvirtualobjectivethread
	{
		int deviceId;
	};
	static Paramvirtualobjectivethread param_virtualobjectivethread[MAX_DEVICEID_NUM];
	static pthread_mutex_t mutex_virtualobjective = PTHREAD_MUTEX_INITIALIZER;
#elif USE_TYPE == 1
	static pthread_t _oldobjectivethread[MAX_DEVICEID_NUM];
	void *_oldobjective(void *des);
	static bool b_oldobjectivethread_stop[MAX_DEVICEID_NUM];
	static bool b_oldobjectivethread[MAX_DEVICEID_NUM];
	static XTcp client_ftp[MAX_DEVICEID_NUM];			//TCP����
	char *rcv_oldobjectivebuf[MAX_DEVICEID_NUM];
	struct Paramoldobjectivethread
	{
		int deviceId;
	};
	static Paramoldobjectivethread param_oldobjectivethread[MAX_DEVICEID_NUM];
	static pthread_mutex_t mutex_oldobjective = PTHREAD_MUTEX_INITIALIZER;
	enum Rcv_Old_Event_List
	{
		OLD_EVENT_NULL = 0,
		OLD_EVENT_SENT_MOVE = 1,
		OLD_EVENT_GET_POSTION = 2
	};
	static Rcv_Old_Event_List rev_old_event[MAX_DEVICEID_NUM];		//�Ƿ��������ﾵת����
	static std::string rev_old_string[MAX_DEVICEID_NUM];	//���յ����ַ�
#endif
	/*******************/
	//�����ﾵת������Ϣ
#define VIRAXIS_OBJCOUNT						6			//�����λ����
#define VIRAXIS_POSITION					   "A"			//����Ĭ��λ��
	/***************/

	void SLEEP_S(double time)
	{
		Sleep(int(time * 1000));
	}

	std::string getCommitID_appObjectivelib()
	{
		return COMMIT_ID_APP_OBJECTIVELIB;
	}

	void OBJECTIVELIB_DumpTrigger()
    {
		dumpTrigger_baseCommon();
        return;
    }

	void printComId()
	{
		std::string strCommitId_baseCommon = getCommitID_baseCommon();
		LOG_INFO("default", "CommitId_baseCommon:" + strCommitId_baseCommon);
		std::cout << "CommitId_baseCommon: " << strCommitId_baseCommon << std::endl;

		std::string strCommitId_appObjectivelib = getCommitID_appObjectivelib();
		LOG_INFO("default", "CommitId_Objectivelib:" + strCommitId_appObjectivelib);
		std::cout << "CommitId_Objectivelib: " << strCommitId_appObjectivelib << std::endl;
	}

	Qt::ConnectionType getconnectionType()
	{
		QThread* currentThread = QThread::currentThread();
		QThread* targetThread = ((QObject*)m_qtobject)->thread();
		Qt::ConnectionType connectionType = (currentThread != targetThread)
			? Qt::BlockingQueuedConnection
			: Qt::DirectConnection;
		return connectionType;
	}

	void OBJECTIVELIB_Init()
    {
		std::lock_guard<std::mutex> lock(mtx); // ���뺯��ʱ�������뿪ʱ�Զ�����
		if (b_sdk_init == false)
		{
			for (int i = 0; i < MAX_DEVICEID_NUM; i++)
			{
				link_ftp_state[i] = false;
				m_objectiveinfo[i].objeccount = 0;
				m_objectiveinfo[i].position = "A";
				m_objectiveinfo[i].moveing = false;
				m_objectiveinfo[i].targetposition = "A";
			}
			commonInit();
			printComId();
			b_sdk_init = true;
		}
        return ;
    }

	void OBJ_SetQtObject(void *ptr)    //����QT�źŲ۵�ַ
	{
		std::lock_guard<std::mutex> lock(mtx); // ���뺯��ʱ�������뿪ʱ�Զ�����
		m_qtobject = ptr;
	}

	bool OBJ_InitIsOpen()    //�Ƿ��Ѿ���ʼ��
	{
		return b_sdk_init;
	}

	void clearErrorInfo(int deviceId)
	{
		if (deviceId < 0 || deviceId >= MAX_DEVICEID_NUM)
		{
			return;
		}
		m_errinfo[deviceId].b_err = false;
		m_errinfo[deviceId].str.clear();
	}

	void pushErrorInfo(int deviceId, std::string str)
	{
		if (deviceId < 0 || deviceId >= 10)
		{
			return;
		}
		m_errinfo[deviceId].b_err = true;
		if (!m_errinfo[deviceId].str.empty())
		{
			m_errinfo[deviceId].str += ";";  // �÷ֺŷָ���������
		}
		m_errinfo[deviceId].str += str;  // ׷���´�����Ϣ
	}

#if USE_TYPE == 0
	void virtualObjectiveThreadStart(int deviceId)
	{
		if (deviceId < 0 || deviceId >= MAX_DEVICEID_NUM)
		{
			return;
		}
		b_virtualobjectivethread[deviceId] = true;
		b_virtualobjectivethread_stop[deviceId] = false;
		param_virtualobjectivethread[deviceId].deviceId = deviceId;
		pthread_create(&_virtualobjectivethread[deviceId], NULL, _virtualobjective, (void *)(&param_virtualobjectivethread[deviceId]));
	}

	void virtualObjectiveThreadStop(int deviceId)
	{
		if (b_virtualobjectivethread[deviceId] == true)
		{
			b_virtualobjectivethread_stop[deviceId] = false;
			b_virtualobjectivethread[deviceId] = false;
			while (b_virtualobjectivethread_stop[deviceId] == false)
			{
				SLEEP_S(0);
			}
			pthread_join(_virtualobjectivethread[deviceId], NULL);
		}
	}

	void *_virtualobjective(void *des)
	{
		Paramvirtualobjectivethread _p = *((Paramvirtualobjectivethread *)des);
		int deviceId = _p.deviceId;
		while (1)
		{
			if (b_virtualobjectivethread[deviceId] == true)
			{
				pthread_mutex_lock(&mutex_virtualobjective);
				//��������ⲿ����
				if (m_objectiveinfo[deviceId].moveing==true)
				{
					if (m_objectiveinfo[deviceId].position!=m_objectiveinfo[deviceId].targetposition)
					{
						m_objectiveinfo[deviceId].position = m_objectiveinfo[deviceId].targetposition;
						m_objectiveinfo[deviceId].moveing = false;
					}
					else
					{
						m_objectiveinfo[deviceId].moveing = false;
					}
				}
				pthread_mutex_unlock(&mutex_virtualobjective);
			}
			else
			{
				b_virtualobjectivethread_stop[deviceId] = true;
				break;
			}
			SLEEP_S(2);//ÿ2msˢ��һ������
		}
		return nullptr;
	}
#elif USE_TYPE == 1
	void oldAxisiThreadStart(int deviceId)
	{
		if (deviceId < 0 || deviceId >= MAX_DEVICEID_NUM)
		{
			return;
		}
		b_oldobjectivethread[deviceId] = true;
		b_oldobjectivethread_stop[deviceId] = false;
		param_oldobjectivethread[deviceId].deviceId = deviceId;
		pthread_create(&_oldobjectivethread[deviceId], NULL, _oldobjective, (void *)(&param_oldobjectivethread[deviceId]));
	}

	void oldObjectiveThreadStop(int deviceId)
	{
		if (b_oldobjectivethread[deviceId] == true)
		{
			b_oldobjectivethread_stop[deviceId] = false;
			b_oldobjectivethread[deviceId] = false;
			while (b_oldobjectivethread_stop[deviceId] == false)
			{
				SLEEP_S(0);
			}
			pthread_join(_oldobjectivethread[deviceId], NULL);
		}
	}

	void *_oldobjective(void *des)
	{
		Paramoldobjectivethread _p = *((Paramoldobjectivethread *)des);
		int deviceId = _p.deviceId;
		rcv_oldobjectivebuf[deviceId] = new char[RECVBUFFER_FTP_MAX];
		while (1)
		{
			if (b_oldobjectivethread[deviceId] == true)
			{
				//��������ⲿ����
				int rcvnum = client_ftp[deviceId].Recv((char*)rcv_oldobjectivebuf[deviceId], RECVBUFFER_FTP_MAX);
				if (rcvnum > 0)
				{
					pthread_mutex_lock(&mutex_oldobjective);
					if (rev_old_event[deviceId] == OLD_EVENT_SENT_MOVE)
					{
						rev_old_string[deviceId] = std::string(rcv_oldobjectivebuf[deviceId]);
						rev_old_event[deviceId] = OLD_EVENT_NULL;
					}
					else if (rev_old_event[deviceId] == OLD_EVENT_GET_POSTION)
					{
						rev_old_string[deviceId] = std::string(rcv_oldobjectivebuf[deviceId]);
						rev_old_event[deviceId] = OLD_EVENT_NULL;
					}
					pthread_mutex_unlock(&mutex_oldobjective);
				}
			}
			else
			{
				b_oldobjectivethread_stop[deviceId] = true;
				break;
			}
			SLEEP_S(0);
		}
		delete[]rcv_oldobjectivebuf[deviceId];
		return nullptr;
	}
#endif

	bool OBJ_EthernetIsOpen(int deviceId)    //�Ƿ��Ѿ����Ӵ�����
	{
		std::lock_guard<std::mutex> lock(mtx); // ���뺯��ʱ�������뿪ʱ�Զ�����
		if (deviceId < 0 || deviceId >= MAX_DEVICEID_NUM)
		{
			pushErrorInfo(deviceId, "Device number exceeds threshold");
			return false;
		}
		return link_ftp_state[deviceId];
	}

	int OBJ_GetDeviceNum(int &deviceNum)	//��ȡ��������
	{
		std::lock_guard<std::mutex> lock(mtx); // ���뺯��ʱ�������뿪ʱ�Զ�����
#if USE_TYPE == 0
		deviceNum = MAX_DEVICEID_NUM;	//���ⷵ��10��������
#elif USE_TYPE == 1
		deviceNum = 1;	//ʵ�ʷ���
#elif USE_TYPE == 2
		deviceNum = 1;	//ʵ�ʷ���
#endif
		return 0;
	}

	int OBJ_GetErrorInfo(int deviceId, char *err)
	{
		std::lock_guard<std::mutex> lock(mtx); // ���뺯��ʱ�������뿪ʱ�Զ�����
		std::string s_err;
		if (deviceId < 0 || deviceId >= MAX_DEVICEID_NUM)
		{
			pushErrorInfo(deviceId, "Device number exceeds threshold");
			return 1;
		}
		if (m_errinfo[deviceId].b_err == false)
		{
			s_err = "no err";
		}
		else
		{
			s_err = m_errinfo[deviceId].str;
		}
		strcpy(err, s_err.c_str());
		return 0;
	}

	int OBJ_GetDeviceInfo(int deviceId, char* strDeviceId, char* strFriendlyName, char* strDriverVersion)//��ȡ������Ϣ
	{
		std::lock_guard<std::mutex> lock(mtx); // ���뺯��ʱ�������뿪ʱ�Զ�����
		clearErrorInfo(deviceId);
		if (deviceId < 0 || deviceId >= MAX_DEVICEID_NUM)
		{
			pushErrorInfo(deviceId, "Device number exceeds threshold");
			return 1;
		}
#if USE_TYPE == 0
		std::string s_str1 = "VirtualOBJDevice ";
		std::string s_str2 = "VirtualOBJ Stage";
#elif USE_TYPE == 1
		std::string s_str1 = "OldOBJDevice ";
		std::string s_str2 = "OldOBJ Stage";
#elif USE_TYPE == 2
		std::string s_str1 = "OldQtOBJDevice ";
		std::string s_str2 = "OldQtOBJ Stage";
#endif
		std::string s_strDriverVersion = "v 1.0.0";
		std::string s_deviceId = std::to_string(deviceId); // ������ת��Ϊ�ַ���
		std::string s_strDeviceId = s_str1 + s_deviceId;
		std::string s_strFriendlyName = s_str2 + s_deviceId;
		strcpy(strDeviceId, s_strDeviceId.c_str());
		strcpy(strFriendlyName, s_strFriendlyName.c_str());
		strcpy(strDriverVersion, s_strDriverVersion.c_str());
		return 0;
	}

	int OBJ_EthernetOpen(int deviceId, char* ip, int port)    //���Ӵ�����
	{
		std::lock_guard<std::mutex> lock(mtx); // ���뺯��ʱ�������뿪ʱ�Զ�����
		clearErrorInfo(deviceId);
		if (deviceId < 0 || deviceId >= MAX_DEVICEID_NUM)
		{
			pushErrorInfo(deviceId, "Device number exceeds threshold");
			return 1;
		}
		if (link_ftp_state[deviceId] != true)
		{
#if USE_TYPE == 1
			if (0 > client_ftp[deviceId].CreateSocket()) { pushErrorInfo(deviceId, "Tcp socket creat failed"); return 2; };
			if(false==client_ftp[deviceId].Connect(ip, port)) { pushErrorInfo(deviceId, "Tcp socket conncet failed"); return 2; };
			if(false == client_ftp[deviceId].SetBlock(0)){client_ftp[deviceId].Close(); pushErrorInfo(deviceId, "Tcp socket setblock failed"); return 2;}
			if(false == client_ftp[deviceId].SetRcvBufferlong(RECVBUFFER_FTP_MAX)){ client_ftp[deviceId].Close(); pushErrorInfo(deviceId, "Tcp socket setrcvbufferlong failed"); return 2;}
			m_objectiveinfo[deviceId].objeccount = 5;
			b_oldobjectivethread[deviceId] = false;
			b_oldobjectivethread_stop[deviceId] = false;
			rev_old_event[deviceId] = OLD_EVENT_NULL;
			rev_old_string[deviceId].clear();
			oldAxisiThreadStart(deviceId);
#elif USE_TYPE == 2
			m_objectiveinfo[deviceId].objeccount = 5;
#endif

#if USE_TYPE == 0
			m_objectiveinfo[deviceId].position = VIRAXIS_POSITION;
			m_objectiveinfo[deviceId].targetposition = VIRAXIS_POSITION;
			m_objectiveinfo[deviceId].objeccount = VIRAXIS_OBJCOUNT;
			b_virtualobjectivethread[deviceId] = false;
			b_virtualobjectivethread_stop[deviceId] = false;
			virtualObjectiveThreadStart(deviceId);
#endif
			m_objectiveinfo[deviceId].moveing = false;

			link_ftp_state[deviceId] = true;
		}
		else
		{
			pushErrorInfo(deviceId, "The drive has been connected");
			return 3;
		}
		return 0;
	}

	int OBJ_CommunicationClose(int deviceId) //�Ͽ�������
	{
		std::lock_guard<std::mutex> lock(mtx); // ���뺯��ʱ�������뿪ʱ�Զ�����
		clearErrorInfo(deviceId);
		if (deviceId < 0 || deviceId >= MAX_DEVICEID_NUM)
		{
			pushErrorInfo(deviceId, "Device number exceeds threshold");
			return 1;
		}
		if (link_ftp_state[deviceId] == true)
		{
#if USE_TYPE == 0
			virtualObjectiveThreadStop(deviceId);
#elif USE_TYPE == 1
			oldObjectiveThreadStop(deviceId);
#elif USE_TYPE == 2

#endif
			
#if USE_TYPE == 1
			client_ftp[deviceId].Close();
			rev_old_event[deviceId] = OLD_EVENT_NULL;
			rev_old_string[deviceId].clear();
#elif USE_TYPE == 2

#endif
			link_ftp_state[deviceId] = false;
		}
		return 0;
	}

	int OBJ_RevolverMoveTo(int deviceId, char *objHolePos) //�ƶ�����λ
	{
		std::lock_guard<std::mutex> lock(mtx); // ���뺯��ʱ�������뿪ʱ�Զ�����
		clearErrorInfo(deviceId);
		if (deviceId < 0 || deviceId >= MAX_DEVICEID_NUM)
		{
			pushErrorInfo(deviceId, "Device number exceeds threshold");
			return 1;
		}
		if (link_ftp_state[deviceId] == false)
		{
			pushErrorInfo(deviceId, "The drive is not connected.");
			return 2;
		}
		std::string targetposition = std::string(objHolePos);
		m_objectiveinfo[deviceId].targetposition = targetposition;
#if USE_TYPE == 0

#elif USE_TYPE == 1
		int errorCode = 0;
		std::string CR = "\x0d";
		std::string LF = "\x0a";
		std::string msg = std::string("RWRMV") + targetposition + CR + LF;
		rev_old_event[deviceId] = OLD_EVENT_SENT_MOVE;
		if (msg.size() != client_ftp[deviceId].Send(msg.c_str(), msg.size())) { pushErrorInfo(deviceId, "Tcp socket send failed"); return 3;}
		SLEEP_S(TCP_TIMEOUT);
		if (rev_old_event[deviceId]== OLD_EVENT_SENT_MOVE){pushErrorInfo(deviceId, "Tcp socket recv failed"); rev_old_event[deviceId] = OLD_EVENT_NULL; return 3;}
		std::string s_rcv = rev_old_string[deviceId];
		if (s_rcv.size() == 0) { pushErrorInfo(deviceId, "Tcp socket recv nothing"); return 3;}
		if (s_rcv!="OK") { pushErrorInfo(deviceId, s_rcv); return 3; }
#elif USE_TYPE == 2
		bool result;
		QVariantMap outinfo;
		QVariantMap ininfo;
		ininfo.insert("cmd", QString("RevolverMoveTo"));
		ininfo.insert("pos", QString::fromStdString(targetposition));
		bool success = QMetaObject::invokeMethod((QObject*)m_qtobject, "onOBJcmd",getconnectionType(), Q_RETURN_ARG(QVariantMap, outinfo), Q_ARG(QVariantMap, ininfo));
		if (false == success) {
			pushErrorInfo(deviceId, "onRevolverMoveTo is err");
			return 3;
		}
		result = outinfo.value("result").toBool();
		if (result ==false)
		{
			pushErrorInfo(deviceId, "onRevolverMoveTo start err");
			return 3;
		}
#endif
		if (m_objectiveinfo[deviceId].position!= m_objectiveinfo[deviceId].targetposition)
		{
			m_objectiveinfo[deviceId].moveing = true;
		}
		return 0;
	}

	int OBJ_GetPosition(int deviceId,char *objHolePos) //��ȡ��ǰ��λ
	{
		std::lock_guard<std::mutex> lock(mtx); // ���뺯��ʱ�������뿪ʱ�Զ�����
		clearErrorInfo(deviceId);
		if (deviceId < 0 || deviceId >= MAX_DEVICEID_NUM)
		{
			pushErrorInfo(deviceId, "Device number exceeds threshold");
			return 1;
		}
		if (link_ftp_state[deviceId] == false)
		{
			pushErrorInfo(deviceId, "The drive is not connected.");
			return 2;
		}
#if USE_TYPE == 1
		int errorCode = 0;
		std::string CR = "\x0d";
		std::string LF = "\x0a";
		std::string msg = std::string("RRDSTU") + CR + LF;
		rev_old_event[deviceId] = OLD_EVENT_GET_POSTION;
		if (msg.size() != client_ftp[deviceId].Send(msg.c_str(), msg.size())) { pushErrorInfo(deviceId, "Tcp socket send failed"); return 3; }
		SLEEP_S(TCP_TIMEOUT);
		if (rev_old_event[deviceId] == OLD_EVENT_GET_POSTION) { pushErrorInfo(deviceId, "Tcp socket recv failed"); rev_old_event[deviceId] = OLD_EVENT_NULL; return 3; }
		std::string s_rcv = rev_old_string[deviceId];
		if (s_rcv.size() == 0) { pushErrorInfo(deviceId, "Tcp socket recv nothing"); return 3; }
		std::string result = s_rcv.substr(0, 2);
		if (result != "OK") { pushErrorInfo(deviceId, s_rcv); return 3; }
		if (s_rcv.size() < 10) { pushErrorInfo(deviceId, s_rcv); return 3; }
		std::string s_pos = s_rcv.substr(9, 1);
		int pos = atoi(s_pos.c_str());
		if (pos==0)return 3;
		else if(pos == 1)m_objectiveinfo[deviceId].position = "A";
		else if (pos == 2)m_objectiveinfo[deviceId].position = "B";
		else if (pos == 3)m_objectiveinfo[deviceId].position = "C";
		else if (pos == 4)m_objectiveinfo[deviceId].position = "D";
		else if (pos == 5)m_objectiveinfo[deviceId].position = "E";
#elif USE_TYPE == 2
		QString result;
		QVariantMap outinfo;
		QVariantMap ininfo;
		ininfo.insert("cmd", QString("GetPosition"));
		bool success = QMetaObject::invokeMethod((QObject*)m_qtobject, "onOBJcmd",getconnectionType(),Q_RETURN_ARG(QVariantMap, outinfo), Q_ARG(QVariantMap, ininfo));
		if (false == success) {
			pushErrorInfo(deviceId, "GetPosition is err");
			return 3;
		}
		result = outinfo.value("result").toString();
		m_objectiveinfo[deviceId].position = result.toStdString();
#endif
		std::string position = m_objectiveinfo[deviceId].position;
		position.copy(objHolePos, position.size());
		objHolePos[position.size()] = '\0'; // �ֶ���ӽ�����
		return 0;
	}

	int OBJ_IsPosition(int deviceId, bool &isready)
	{
		std::lock_guard<std::mutex> lock(mtx); // ���뺯��ʱ�������뿪ʱ�Զ�����
		clearErrorInfo(deviceId);
		if (deviceId < 0 || deviceId >= MAX_DEVICEID_NUM)
		{
			pushErrorInfo(deviceId, "Device number exceeds threshold");
			return 1;
		}
		if (link_ftp_state[deviceId] == false)
		{
			pushErrorInfo(deviceId, "The drive is not connected.");
			return 2;
		}
#if USE_TYPE==0

#elif USE_TYPE == 1
		int errorCode = 0;
		std::string CR = "\x0d";
		std::string LF = "\x0a";
		std::string msg = std::string("RRDSTU") + CR + LF;
		rev_old_event[deviceId] = OLD_EVENT_GET_POSTION;
		if (msg.size() != client_ftp[deviceId].Send(msg.c_str(), msg.size())) { pushErrorInfo(deviceId, "Tcp socket send failed"); return 3; }
		SLEEP_S(TCP_TIMEOUT);
		if (rev_old_event[deviceId] == OLD_EVENT_GET_POSTION) { pushErrorInfo(deviceId, "Tcp socket recv failed"); rev_old_event[deviceId] = OLD_EVENT_NULL; return 3; }
		std::string s_rcv = rev_old_string[deviceId];
		if (s_rcv.size() == 0) { pushErrorInfo(deviceId, "Tcp socket recv nothing"); return 3; }
		std::string result = s_rcv.substr(0, 2);
		if (result != "OK") { pushErrorInfo(deviceId, s_rcv); return 3; }
		if (s_rcv.size() < 10) { pushErrorInfo(deviceId, s_rcv); return 3; }
		std::string s_pos = s_rcv.substr(9, 1);
		int pos = atoi(s_pos.c_str());
		if (pos == 0)return m_objectiveinfo[deviceId].moveing = true;
		else
		{
			if (pos == 1)m_objectiveinfo[deviceId].position = "A";
			else if (pos == 2)m_objectiveinfo[deviceId].position = "B";
			else if (pos == 3)m_objectiveinfo[deviceId].position = "C";
			else if (pos == 4)m_objectiveinfo[deviceId].position = "D";
			else if (pos == 5)m_objectiveinfo[deviceId].position = "E";
			if (m_objectiveinfo[deviceId].position == m_objectiveinfo[deviceId].targetposition)
			{
				m_objectiveinfo[deviceId].moveing = false;
			}
			else
			{
				m_objectiveinfo[deviceId].moveing = true;
			}
		}
#elif USE_TYPE == 2
		bool result;
		QVariantMap outinfo;
		QVariantMap ininfo;
		ininfo.insert("cmd", QString("IsPosition"));
		ininfo.insert("pos", QString::fromStdString(m_objectiveinfo[deviceId].targetposition));
		bool success = QMetaObject::invokeMethod((QObject*)m_qtobject, "onOBJcmd",getconnectionType(), Q_RETURN_ARG(QVariantMap, outinfo), Q_ARG(QVariantMap, ininfo));
		if (false == success) {
			pushErrorInfo(deviceId, "IsPosition is err");
			return 3;
		}
		result = outinfo.value("result").toBool();
		if (result==true)
		{
			m_objectiveinfo[deviceId].moveing = false;
		}
		else
		{
			m_objectiveinfo[deviceId].moveing = true;
		}
#endif
		if (m_objectiveinfo[deviceId].moveing==true)
		{
			isready = false;
		}
		else
		{
			isready = true;
		}
		return 0;
	}
}

