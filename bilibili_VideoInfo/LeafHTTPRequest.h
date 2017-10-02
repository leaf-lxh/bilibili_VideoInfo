#include "stdafx.h"
#include <string>



 class LeafHTTPRequest
{
	
public:	
	std::string RawRequestReturn ;
	std::string FromedHeader ;

	struct VideoInfo
	{
		std::string Retcode;
		std::string aid;
		std::string view;
		std::string danmaku;
		std::string reply;
		std::string favorite;
		std::string coin;
		std::string share;
	};
	VideoInfo Info;
	void MakeHeader(std::string RequestUrl);
	void SortInfo(std::string RawResponse);
	char *RawRequest(std::string Header);


};