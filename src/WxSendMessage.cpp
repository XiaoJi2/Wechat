#include "WxSendMessage.h"
#include "WxSqlite.h"
#include "HttpSocket.h"
#include "WxInitMessage.h"
#include "cJSON.h"
#include <iostream>
#include <string.h>
#include <stdio.h>

WxSendMessage::WxSendMessage()
{

}

WxSendMessage::~WxSendMessage()
{

}

bool WxSendMessage::parseSendMessage(const char *pMsg)
{
	if(pMsg == NULL)
	{
		return false;
	}
	cJSON *pJson = cJSON_Parse(pMsg);
	if( NULL == pJson)
	{
		return false;
	}

	cJSON *pSub = cJSON_GetObjectItem(pJson,"BaseResponse");
	if(NULL == pSub)
	{
		return false;
	}

	cJSON *Ret = cJSON_GetObjectItem(pSub,"Ret");
	if(NULL == Ret)
	{
		return false;
	}
	if(Ret->valueint != 0)
	{
		return false;
	}

	cJSON_Delete(pJson);
	return true;

}

bool WxSendMessage::WebWxSendMsg(std::string content,std::string toUserName)
{
	//std::cout << "________________webWXSendMsg________________\n";
	std::string pagebuf;
	time_t tm;
	time(&tm);
	char url[255];
	char szJsonData[1024];
	char localID[17];
	memset(localID,0,sizeof(localID));
	memset(url,0,sizeof(url));
	memset(szJsonData,0,sizeof(szJsonData));
	sprintf(localID,"%ld%d",tm,8980483);
	sprintf(szJsonData,"{\"BaseRequest\":{\"Uin\":%s,\"Sid\":\"%s\",\"Skey\":\"%s\",\"DeviceID\":\"%s\"},\"Msg\":{\"Type\":1,\"Content\":\"%s\",\"FromUserName\":\"%s\",\"ToUserName\":\"%s\",\"LocalID\":\"%s\",\"ClientMsgId\":\"%s\"},\"Scene\":0}",HttpSocket::getInstance()->wxuin.c_str(),HttpSocket::getInstance()->wxsid.c_str(),HttpSocket::getInstance()->skey.c_str(),WxInitMessage::getInstance()->deviceId.c_str(),content.c_str(),WxInitMessage::getInstance()->myUserName.c_str(),toUserName.c_str(),localID,localID);
	//std::cout << szJsonData << "\n";
	if(HttpSocket::getInstance()->WX_V2)
	{
		sprintf(url,"https://wx2.qq.com/cgi-bin/mmwebwx-bin/webwxsendmsg?pass_ticket=%s",HttpSocket::getInstance()->pass_ticket.c_str());
	}else
	{
		sprintf(url,"https://wx.qq.com/cgi-bin/mmwebwx-bin/webwxsendmsg?pass_ticket=%s",HttpSocket::getInstance()->pass_ticket.c_str());
	}
	pagebuf = HttpSocket::getInstance()->urlPost(url,szJsonData);
	if(pagebuf.empty())
	{
		return false;
	}
	if(parseSendMessage(pagebuf.c_str()))
		return true;
	else 
		return false;
}

void WxSendMessage::WxSendMessageStart()
{
	std::string content;
	int len = 0;
	while(true)
	{
		content.clear();
		getline(std::cin,content);
		len = content.find("->");
		//std::cout << "-> len " << len <<"\n";
		if(len == 0)
		{
			len = content.find(":");
			if(len <= 0)
			{
				std::cout << "please input content again: " << "\n";
				continue ;
			}
			std::string name = content.substr(2,len-2);
			std::string toContent = content.substr(len+1,content.length());
			//std::cout << "name : " << name << "\n";
			//std::cout << "toContent : " << toContent << "\n";
			if(WxSqlite::getInstance()->selectUserInfo(name))
			{
				if(!WebWxSendMsg(toContent,WxSqlite::getInstance()->selectUserName))
					std::cout << "发送失败，请重新发送！\n";
			}
		}
	}

}
