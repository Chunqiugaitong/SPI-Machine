#ifndef __LTS_DLL
#define __LTS_DLL

//-----预定义公共错误--------------------
#define Bad_CommOpen	-101//端口打开错误
#define	Bad_CommSet		-102//端口设置出错

#define	Bad_CommTimeouts	-999//端口读写超时设置错
#define	Bad_CommQueue		-998//端口缓冲区操作错

//-----读写错误-------------------------
#define Bad_CommRead		-103//串口读出错
#define	Bad_CommWrite		-104//串口写超时
#define PACKET_Timeout		-105//接收数据超时
#define Bad_CommClose		-106//关闭串口错
//----------函数声明---------------------------------------------

#define __LTS_EXPORTS

//定义输入和输出
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