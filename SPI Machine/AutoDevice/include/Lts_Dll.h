#ifndef __LTS_DLL
#define __LTS_DLL

//-----Ԥ���幫������--------------------
#define Bad_CommOpen	-101//�˿ڴ򿪴���
#define	Bad_CommSet		-102//�˿����ó���

#define	Bad_CommTimeouts	-999//�˿ڶ�д��ʱ���ô�
#define	Bad_CommQueue		-998//�˿ڻ�����������

//-----��д����-------------------------
#define Bad_CommRead		-103//���ڶ�����
#define	Bad_CommWrite		-104//����д��ʱ
#define PACKET_Timeout		-105//�������ݳ�ʱ
#define Bad_CommClose		-106//�رմ��ڴ�
//----------��������---------------------------------------------

#define __LTS_EXPORTS

//������������
#ifdef __LTS_EXPORTS
#define LTS_API __declspec(dllexport)
#else
#define LTS_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif


HANDLE LTS_API Lts_CommOpen(char *Port);
HANDLE LTS_API Lts_CommOpenWithBaud(char *Port, unsigned int BaudRate);
int LTS_API Lts_CommClose(HANDLE ComHandle);

int LTS_API Lts_InitLight(HANDLE ComHandle);
int LTS_API Lts_SaveData(HANDLE ComHandle);
int LTS_API Lts_RefreshData(HANDLE ComHandle);
int LTS_API Lts_SetChxData(HANDLE ComHandle, BYTE bChType, int iChData);
int LTS_API Lts_GetChxData(HANDLE ComHandle,BYTE bChType, int* iChData);
int LTS_API Lts_GetCurChxData(HANDLE ComHandle, int iChData[6]);


#ifdef __cplusplus
}
#endif

#endif