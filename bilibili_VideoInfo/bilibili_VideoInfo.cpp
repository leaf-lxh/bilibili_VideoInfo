// bilibili_VideoInfo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "LeafHTTPRequest.h"
#include <string>
#include <iostream>
#include <mysql.h>
#include <time.h>

#pragma warning (disable:4996)
#pragma comment (lib,"libmysql.lib")
int _tmain(int argc, _TCHAR* argv[])
{
	//完整url示例
    //https://api.bilibili.com/x/web-interface/archive/stat?callback=jQuery17204101056215863843_1502195878701&aid=12346865&jsonp=jsonp&_=1502195879211

	//网络请求类初始化
	LeafHTTPRequest request;
	std::string tempUrl = "";
	std::string tempData = "";
	int iResult = 0;
	
	//MySQL数据库存储初始化
	MYSQL thisSQL;
	MYSQL_RES thisSQLresult;
	MYSQL_ROW thisSQLrow;

	const char user[] = "root";
	const char pswd[] = "------------";
	const char host[] = "localhost";
	const char database[] = "bilibili_VideoInfo";
	unsigned int port = 3306;

	mysql_init(&thisSQL);
	std::cout << mysql_real_connect(&thisSQL, host, user, pswd, database, port, 0, 0);

	//计时初始化
	clock_t time1, time2;


	//进行请求并存储
	int i = 1;
	char aid[11] = "";
	time1 = clock();
	while (i<=1000)
	{
		
		sprintf(aid, "%d", i);
		tempUrl = "http://api.bilibili.com/x/web-interface/archive/stat?callback=jQuery17204101056215863843_1502195878701&aid=";
		tempUrl += aid;
		tempUrl += "&jsonp=jsonp&_=1502195879211";

		request.MakeHeader(tempUrl);
		request.RawRequest(request.FromedHeader);

		request.SortInfo(request.RawRequestReturn);
		if (request.Info.Retcode != "0")
		{
			std::cout << "unaccessable: aid";
			tempData = "insert into info(retcode,aid,view,danmaku,reply,favorite,coin,share) value(";
			tempData += request.Info.Retcode + ',';
			tempData += aid;
			tempData += ",\"--\",\"--\",\"--\",\"--\",\"--\",\"--\")";
		}
		else
		{
			//std::cout << "view: " << request.Info.view << "|danmaku: " << request.Info.danmaku << "|coin: " << request.Info.coin << std::endl;
			tempData = "insert into info(retcode,aid,view,danmaku,reply,favorite,coin,share) value(0,";
			tempData += request.Info.aid + ',';
			tempData += request.Info.view + ',';
			tempData += request.Info.danmaku + ',';
			tempData += request.Info.reply + ',';
			tempData += request.Info.favorite + ',';
			tempData += request.Info.coin + ',';
			tempData += request.Info.share + ')';
			
		}
		mysql_query(&thisSQL, tempData.c_str());

		std::cout << aid << "\n";
		++i;
		
		//Sleep(1000);
	}
	
	time2 = clock();
	


	std::cout << time2 - time1 << "ms";
	std::cin.get();

	
	
	return 0;
}

