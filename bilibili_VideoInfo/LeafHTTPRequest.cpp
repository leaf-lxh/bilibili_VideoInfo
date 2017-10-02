#include "stdafx.h"
#include "LeafHTTPRequest.h"
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib,"Ws2_32.lib")
#pragma warning (disable:4996)

//根据请求提交信息构造Headers
void LeafHTTPRequest::MakeHeader(std::string RequestUrl)
{
	LeafHTTPRequest::FromedHeader = "";

	LeafHTTPRequest::FromedHeader += "GET " + RequestUrl+" HTTP/1.1 \r\n";
	LeafHTTPRequest::FromedHeader += "Host: api.bilibili.com\r\n";
	LeafHTTPRequest::FromedHeader += "Connection: keep-alive\r\n";
	LeafHTTPRequest::FromedHeader += "Cache-Control: max-age=0\r\n";
	LeafHTTPRequest::FromedHeader += "Upgrade-Insecure-Requests: 1\r\n";
	LeafHTTPRequest::FromedHeader += "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/59.0.3071.115 Safari/537.36\r\n";
	LeafHTTPRequest::FromedHeader += "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n";
	LeafHTTPRequest::FromedHeader += "Accept-Encoding: gzip, deflate, br\r\n";
	LeafHTTPRequest::FromedHeader += "Accept-Language: zh,en-US;q=0.8,en;q=0.6,zh-CN;q=0.4\r\n";
	LeafHTTPRequest::FromedHeader += "Cookie: buvid3=0350672E-5DCE-47C0-8D36-96C043E53A8010964infoc; finger=edc6ecda; UM_distinctid=15dc1da01a7ca-0c0556602e9c47-8383667-1fa400-15dc1da01a8292; fts=1502195876; pgv_pvi=314387456; pgv_si=s9420942336; sid=d7g5d8nm; biliMzIsnew=1; biliMzTs=null\r\n\r\n";
}

//整理一下返回数据
void LeafHTTPRequest::SortInfo(std::string RawResponse)
{
	//取出状态码
	int pos_code = RawResponse.find("code", 0);
	int pos_codeend = RawResponse.find(',', pos_code);
	LeafHTTPRequest::Info.Retcode = RawResponse.substr(pos_code + 6, pos_codeend - pos_code - 6);

	//如果视频能获取信息则返回0,否则则返回错误码如-403 -404，此情况不存入数据库
	if (Info.Retcode != "0")
	{
		return;
	}
	
	int pos_first = 0;
	int pos_last = 0;

	pos_first = RawResponse.find("aid", 0);
	pos_last = RawResponse.find(',', pos_first);
	LeafHTTPRequest::Info.aid = RawResponse.substr(pos_first + 5, pos_last - pos_first - 5);

	
	pos_first = RawResponse.find("view", 0);
	pos_last = RawResponse.find(',', pos_first);
	LeafHTTPRequest::Info.view = RawResponse.substr(pos_first + 6, pos_last - pos_first - 6);

	pos_first = RawResponse.find("danmaku", 0);
	pos_last = RawResponse.find(',', pos_first);
	LeafHTTPRequest::Info.danmaku = RawResponse.substr(pos_first + 9, pos_last - pos_first - 9);

	pos_first = RawResponse.find("reply", 0);
	pos_last = RawResponse.find(',', pos_first);
	LeafHTTPRequest::Info.reply = RawResponse.substr(pos_first + 7, pos_last - pos_first - 7);

	pos_first = RawResponse.find("favorite", 0);
	pos_last = RawResponse.find(',', pos_first);
	LeafHTTPRequest::Info.favorite = RawResponse.substr(pos_first + 10, pos_last - pos_first - 10);

	pos_first = RawResponse.find("coin", 0);
	pos_last = RawResponse.find(',', pos_first);
	LeafHTTPRequest::Info.coin = RawResponse.substr(pos_first + 6, pos_last - pos_first - 6);

	pos_first = RawResponse.find("share", 0);
	pos_last = RawResponse.find(',', pos_first);
	LeafHTTPRequest::Info.share = RawResponse.substr(pos_first + 7, pos_last - pos_first - 7);
}


//根据Headers进行请求
char ReturnBuffer[256] = "";
char *LeafHTTPRequest::RawRequest(std::string Header)
{
	DWORD iResult = 0;
	LeafHTTPRequest::RawRequestReturn = "";

	

	//init winsock
	WSADATA wsadata ;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (iResult != NO_ERROR)
	{
		sprintf(ReturnBuffer, "ERROR on WSAStartup: %d", iResult);
		return ReturnBuffer;
	}


	//creat socket
	SOCKET ConnectSocket = INVALID_SOCKET;
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET)
	{
		iResult = WSAGetLastError();
		WSACleanup();
		sprintf(ReturnBuffer, "ERROR on socket(): %d", iResult);
		return ReturnBuffer;
	}


	//ready for connect
	struct sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = inet_addr("27.221.106.4");
	sockaddr.sin_port = htons(80);


	//connect
	iResult = connect(ConnectSocket, (SOCKADDR*)&sockaddr, sizeof(sockaddr));
	if (iResult == SOCKET_ERROR) {
		iResult = WSAGetLastError();
		closesocket(ConnectSocket);
		WSACleanup();
		sprintf(ReturnBuffer, "ERROR on connect(): %d", iResult);
		return ReturnBuffer;
	}


	//send
	iResult = send(ConnectSocket, Header.c_str(), Header.length(), 0);
	if (iResult == SOCKET_ERROR) {
		iResult = WSAGetLastError();
		closesocket(ConnectSocket);
		WSACleanup();
		sprintf(ReturnBuffer, "ERROR on send(): %d", iResult);
		return ReturnBuffer;
	}


	//shutdown 
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		iResult = WSAGetLastError();
		closesocket(ConnectSocket);
		WSACleanup();
		sprintf(ReturnBuffer, "ERROR on shutdown(): %d", iResult);
		return ReturnBuffer;
	}


	//recive
	char recvbuf[65535] = "";
	iResult = recv(ConnectSocket, recvbuf, 65535, 0);
	if (iResult <0) 
	{
		iResult = WSAGetLastError();
		closesocket(ConnectSocket);
		WSACleanup();
		sprintf(ReturnBuffer, "ERROR on recv(): %d", iResult);
		return ReturnBuffer;
	}

	do
	{
		LeafHTTPRequest::RawRequestReturn += recvbuf;
		iResult = recv(ConnectSocket, recvbuf, 65535, 0);
	} while (iResult>0);

	

	//close the socket
	iResult = closesocket(ConnectSocket);
	if (iResult == SOCKET_ERROR) {
		iResult = WSAGetLastError();
		WSACleanup();
		sprintf(ReturnBuffer, "ERROR on recv(): %d", iResult);
		return ReturnBuffer;
	}

	WSACleanup();	
	return "success";
}