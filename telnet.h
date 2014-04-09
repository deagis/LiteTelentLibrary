/***************************************/
/*
*用法：
*
*	telnet_Session pTelnetSession = NULL;
*	char* szCmd = "ifconfig\r\n";
*	char strRecvBuf[256] = {0};
*
*	telnet_WSAStartup();	//完全可以直接执行WSAStartup
*	......
*	pTelnetSession = telnet_OpenSessionWithLogin("login: ", "# ", "192.168.0.130", 23, "root", "123456");
*	......
*	telnet_SendCmdWithComfirmS(pTelnetSession, "/root/bin/do\r\n");
*	......
*	telnet_SendCmdBasic(pTelnetSession, szCmd, strlen(szCmd), strRecvBuf, 256);
*	sscanf(strRecvBuf, "something", ...);
*	......
*	telnet_CloseSession(pTelnetSession);
*	......
*	telnet_WSACleanup();	//完全可以直接执行WSACleanup
*
*/
/***************************************/
#ifndef _TELNET_H_
#define _TELNET_H_

#include <Windows.h>

#define TELNET_CMD_LEN_IN_FILE 256	/*指定telnet_SendCmdFromFile从文件读取命令时，命令的最大长度*/
#define TELNET_MARK_LEN 128	/*指定登录及命令行提示符最大长度*/

//telnet_Session结构体
typedef struct telnet_Session_s {
	char m_szIp[20];	//ip地址
	int m_iPort;	//端口
	char m_szLineMark[TELNET_MARK_LEN];	//换行符
	SOCKET m_TelnetSocket;	//建立的socket句柄
} telnet_Session_t, *telnet_Session;

/*
*功能：telnet_SendCmdWithComfirm简化，无需指定字符串长度
*/
#define telnet_SendCmdWithComfirmS(pTelnetSession, szCmd) telnet_SendCmdWithComfirm(pTelnetSession, szCmd, strlen(szCmd))

/*
*功能：telnet_SendCmdBasic简化，不记录服务器响应
*/
#define telnet_SendCmd(pTelnetSession, pcCmd, iCmdLen) telnet_SendCmdBasic(pTelnetSession, pcCmd, iCmdLen, NULL, 0)

/*
*功能：telnet_ConnectWithLogin简化，无需登录时使用
*/
#define telnet_OpenSession(szLineMark, szIp, iPort) telnet_OpenSessionWithLogin(NULL, szLineMark, szIp, iPort, NULL, NULL)

/*
*功能：WSAStartup封装
*参数：
*返回值：-1=失败，0=成功
*/
int telnet_WSAStartup();

/*
*功能：WSACleanup封装
*参数：
*返回值：-1=失败，0=成功
*/
int telnet_WSACleanup();

/*
*功能：连接至指定telnet服务器，并指定登录提示符以及命令行提示符
*参数：szLoginMark=登录提示符，szLineMark=命令行提示符，szIp=服务器IP，iPort=服务器端口，szUserName=用户名，szPassWord=密码
*返回值：返回指向telnet_Session_s结构体的指针，NULL=失败
*注意：若szLoginMark为空，跳过登录步骤
	   szPassWord为空表示无密码
*/
telnet_Session telnet_OpenSessionWithLogin(const char* szLoginMark, const char* szLineMark, const char* szIp, const int iPort, const char* szUserName, const char* szPassWord);

/*
*功能：关闭连接
*参数：pTelnetSession=需要关闭的telnet会话
*返回值：-1=失败，0=成功
*/
int telnet_CloseSession(telnet_Session pTelnetSession);

/*
*功能：向指定会话发送指定命令，并将读取服务器的响应
*参数：pTelnetSession=指定会话，pcCmd=命令字符串，iCmdLen=命令字符串长度， pcRecvBuf=接收服务器响应缓冲，iRecvBufLen=缓冲区长度
*返回值：-1=失败，0=成功
*注意：以下一个命令行提示符的出现作为命令执行完成的依据，命令回显及命令行提示符均会被保存在pcRecvBuf中
	   命令中应包含换行符，如："ls\r\n",命令长度为4
*/
int telnet_SendCmdBasic(const telnet_Session pTelnetSession, const char* pcCmd, const int iCmdLen, char* pcRecvBuf, const int iRecvBufLen);

/*
*功能：向指定会话确认最近一条命令的返回值
*参数：pTelnetSession=指定会话
*返回值：-1=失败，0=成功
*注意：实际上是发送echo $?并读取其返回值来做判断
*/
int telnet_ComfirmLastCmdResult(const telnet_Session pTelnetSession);

/*
*功能：向指定会话发送指定命令，并检查其返回值
*参数：pTelnetSession=指定会话，pcCmd=命令字符串，iCmdLen=命令字符串长度
*返回值：-1=失败，0=成功
*注意：命令中应包含换行符，如："ls\r\n",命令长度为4
*/
int telnet_SendCmdWithComfirm(const telnet_Session pTelnetSession, const char* pcCmd, const int iCmdLen);

/*
*功能：读取指定文本文件，将文件中命令依次发送向指定会话
*参数：pTelnetSession=指定会话，szFileName=命令所在文件路径
*返回值：-1=失败，0=成功
*注意：文件中每一行认为是一条命令，只有上一条命令执行完成才执行下一条命令
*文件格式：
*	cd /test
*	mkdir sudir
*	cd subdir
*	touch test.txt
*/
int telnet_SendCmdFromFile(const telnet_Session pTelnetSession, const char* szFilePath);

#endif /*_TELNET_H_*/