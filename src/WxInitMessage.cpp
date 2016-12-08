#include "WxInitMessage.h"
#include "WxLog.h"
#include "cJSON.h"
#include "WxSqlite.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>

WxInitMessage *WxInitMessage::wxInit = NULL;
WxInitMessage *WxInitMessage::getInstance()
{
	if(wxInit == NULL)
		wxInit = new WxInitMessage();
	return wxInit;
}

WxInitMessage::WxInitMessage()
{
	curl_global_init(CURL_GLOBAL_ALL);
}

WxInitMessage::~WxInitMessage()
{
	curl_global_cleanup();
}

std::string WxInitMessage::getDeviceId()
{
	deviceId = 'e';
	srand(time(0));
	int i = 0;
	for(;i < 15;i++)
	{
		deviceId += rand()%10 + 0x30;
	}
	return deviceId;

}

bool WxInitMessage::parseGetContact(const char *pMsg)
{
	//std::cout << "________________parseGetContact________________\n";
	WxLog::getInstance()->Debug("________________parseGetContact________________\n");	
	int i = 0;
	char buf[255] = {0};
	if(pMsg == NULL)
	{
		return false;
	}
	cJSON * pJson = cJSON_Parse(pMsg);
	if(pJson == NULL)
	{
		return false;
	}

	cJSON *pSub = cJSON_GetObjectItem(pJson,"MemberCount");
	if(NULL == pSub)
	{
		printf("=====>can't find MemberCount!\n");
		return false;
	}
	std::cout << "MemberCount is " << pSub->valueint << std::endl;
	cJSON *list = cJSON_GetObjectItem(pJson,"MemberList");
	if(NULL == list)
	{
		printf("=====>can't find MemberList!\n");
		return false;
	}

	for(;i < pSub->valueint;i++)
	{
		memset(buf,0,sizeof(buf));
		cJSON *arrayItem = cJSON_GetArrayItem(list,i);
		if(arrayItem)
		{
			 char *pr = cJSON_Print(arrayItem);
			 cJSON *item = cJSON_Parse(pr);
			 cJSON * UserName = cJSON_GetObjectItem(item,"UserName");
			 if(NULL == UserName)
			 {
			  	 printf("=====>can't find UserName!\n");
				 return false;
			 }
			 //printf("====> UserName : %s\n",UserName->valuestring);
			 cJSON *  NickName= cJSON_GetObjectItem(item,"NickName");
			 if(NULL == NickName)
			 {
				 printf("=====>can't find NickName!\n");
				 return false;
			 }
			 //printf("====> NickName : %s\n",NickName->valuestring);
			 cJSON *  RemarkName= cJSON_GetObjectItem(item,"RemarkName");
			 if(NULL == RemarkName)
			 {
				 printf("=====>can't find RemarkName!\n");
				 return false;
			 }
			 //printf("====> RemarkName : %s\n",RemarkName->valuestring);
			
			 WxSqlite::getInstance()->insertUserInfo(UserName->valuestring,NickName->valuestring,RemarkName->valuestring);
		 	 WxLog::getInstance()->Debug(cJSON_Print(UserName));
		 	 WxLog::getInstance()->Debug(cJSON_Print(NickName));
		 	 WxLog::getInstance()->Debug(cJSON_Print(RemarkName));
		}
	}
	cJSON_Delete(pJson);

	
	return true;
}

bool WxInitMessage::parseJson(const char *pMsg)
{
	//std::cout << "________________parseJson________________\n";
	WxLog::getInstance()->Debug("________________parseJson________________\n");	
	int i = 0;
	char buf[255] = {0};
	if(pMsg == NULL)
	{
		return false;
	}
	cJSON * pJson = cJSON_Parse(pMsg);
	if(pJson == NULL)
	{
		return false;
	}

	cJSON *pSub = cJSON_GetObjectItem(pJson,"User");
	if(NULL == pSub)
	{
		printf("=====>can't find!\n");
		return false;
	}
	cJSON *pPSub = cJSON_GetObjectItem(pSub,"UserName");
	if(NULL == pPSub)
	{
		printf("=====>can't find!\n");
		return false;
	}

	std::cout << "UserName is " << pPSub->valuestring << std::endl;
	userName = pPSub->valuestring;

	pSub = cJSON_GetObjectItem(pJson,"SyncKey");
	if(NULL == pSub)
	{
		printf("=====>can't find!\n");
		return false;
	}
	cJSON *SyncKey = cJSON_GetObjectItem(pSub,"Count");
	if(NULL == SyncKey)
	{
		printf("=====>can't find!\n");
		return false;
	}
	cJSON * list = cJSON_GetObjectItem(pSub,"List");
	if(NULL == list)
	{
		printf("=====>can't find list!\n");
		return false;
	}

	for(;i < SyncKey->valueint;i++)
	{
		WxLog::getInstance()->Debug(cJSON_Print(SyncKey));	
		std::cout << "SyncKey" << cJSON_Print(SyncKey) << "\n";
		memset(buf,0,sizeof(buf));
		cJSON *arrayItem = cJSON_GetArrayItem(list,i);
		if(arrayItem)
		{
			 char *pr = cJSON_Print(arrayItem);
			 cJSON *item = cJSON_Parse(pr);
			 cJSON * Key = cJSON_GetObjectItem(item,"Key");
			 if(NULL == Key)
			 {
			  	 printf("=====>can't find Key!\n");
				 return false;
			 }
			 cJSON * Val = cJSON_GetObjectItem(item,"Val");
			 if(NULL == Val)
			 {
				 printf("=====>can't find Val!\n");
				 return false;
			 }
			 
			 if(i == 0)
			 	sprintf(buf,"%d_%d",Key->valueint,Val->valueint);
			 else
			 	sprintf(buf,"%%7C%d_%d",Key->valueint,Val->valueint);
			 synckey += buf;
			 
			 synckeyVal[i] = Val->valueint;
			 std::cout << "synckeyVal" << synckeyVal[i] << "\n";
			 WxLog::getInstance()->Debug(cJSON_Print(Key));	
		 	 WxLog::getInstance()->Debug(cJSON_Print(Val));
			 WxLog::getInstance()->Info(synckey);
		}
	}
	pSub = cJSON_GetObjectItem(pJson,"User");
	if(NULL == pSub)
	{
		printf("=====>can't find User!\n");
		return false;
	}

	cJSON * UsrName = cJSON_GetObjectItem(pSub,"UserName");
	if(NULL == UsrName)
	{
		printf("===>can't find UserName!\n");
	}

	//printf("===> UserName is %s \n",UsrName->valuestring);
	myUserName = UsrName->valuestring;

	cJSON_Delete(pJson);
	return true;
}


bool WxInitMessage::webWXinit()
{
	std::cout << "________________webWXinit________________\n";
	WxLog::getInstance()->Debug("________________webWXinit________________\n");	
	std::string pagebuf;
	time_t tm;
	time(&tm);
	char url[255];
	char szJsonData[1024];
	memset(url,0,sizeof(url));
	memset(szJsonData,0,sizeof(szJsonData));
	sprintf(szJsonData,"{\"BaseRequest\":{\"Uin\":\"%s\", \"Sid\":\"%s\",\"Skey\":\"%s\", \"DeviceID\":\"%s\"}}",HttpSocket::getInstance()->wxuin.c_str(),HttpSocket::getInstance()->wxsid.c_str(),HttpSocket::getInstance()->skey.c_str(),deviceId.c_str());
//	std::cout << szJsonData << "\n";
	if(HttpSocket::getInstance()->WX_V2)
	{
		sprintf(url,"https://wx2.qq.com/cgi-bin/mmwebwx-bin/webwxinit?r=%ld&lang=zh_CN&pass_ticket=%s",tm,HttpSocket::getInstance()->pass_ticket.c_str());
	}else
	{
		sprintf(url,"https://wx.qq.com/cgi-bin/mmwebwx-bin/webwxinit?pass_ticket=%s&skey=%s&r=%ld",HttpSocket::getInstance()->pass_ticket.c_str(),HttpSocket::getInstance()->skey.c_str(),tm);
	}
	pagebuf = HttpSocket::getInstance()->urlPost(url,szJsonData);
	if(pagebuf.empty())
	{
		return false;
	}

	parseJson(pagebuf.c_str());
	return true;
}

bool WxInitMessage::webWXstatusnotify()
{
	std::cout << "________________webWXstatusnotify________________\n";
	WxLog::getInstance()->Debug("________________webWXstatusnotify________________\n");
	std::string pagebuf;
	time_t tm;
	time(&tm);
	char url[255];
	char szJsonData[1024];
	memset(url,0,sizeof(url));
	memset(szJsonData,0,sizeof(szJsonData));
	sprintf(szJsonData,"{\"BaseRequest\":{\"Uin\":%s,\"Sid\":\"%s\",\"Skey\":\"%s\",\"DeviceID\":\"%s\"},\"Code\":3,\"FromUserName\":\"%s\",\"ToUserName\":\"%s\",\"ClientMsgId\":%ld}",HttpSocket::getInstance()->wxuin.c_str(),HttpSocket::getInstance()->wxsid.c_str(),HttpSocket::getInstance()->skey.c_str(),deviceId.c_str(),userName.c_str(),userName.c_str(),tm);
//	std::cout << szJsonData << "\n";
	if(HttpSocket::getInstance()->WX_V2)
	{
		sprintf(url,"https://wx2.qq.com/cgi-bin/mmwebwx-bin/webwxstatusnotify?lang=zh_CN&pass_ticket=%s",HttpSocket::getInstance()->pass_ticket.c_str());
	}else
	{
		sprintf(url,"https://wx.qq.com/cgi-bin/mmwebwx-bin/webwxstatusnotify?lang=zh_CN&pass_ticket=%s",HttpSocket::getInstance()->pass_ticket.c_str());
	}
	pagebuf = HttpSocket::getInstance()->urlPost(url,szJsonData);
	if(pagebuf.empty())
	{
		return false;
	}

	return true;	
}

bool WxInitMessage::webWXgetcontact()
{
	std::cout << "________________webWXgetcontact________________\n";
	WxLog::getInstance()->Debug("________________webWXgetcontact________________\n");
	std::string pagebuf;
	time_t tm;
	time(&tm);
	char url[255];	
	char szJsonData[1024];
	memset(url,0,sizeof(url));
	memset(szJsonData,0,sizeof(szJsonData));
	sprintf(szJsonData,"{\"BaseRequest\":{\"Uin\":%s,\"Sid\":\"%s\",\"Skey\":\"%s\",\"DeviceID\":\"%s\"},\"Count\":2,\"List\":[{\"UserName\":\"%s\",\"EncryChatRoomId\":\"\"},{\"UserName\":\"%s\",\"EncryChatRoomId\":\"\"}]}",HttpSocket::getInstance()->wxuin.c_str(),HttpSocket::getInstance()->wxsid.c_str(),HttpSocket::getInstance()->skey.c_str(),deviceId.c_str(),userName.c_str(),userName.c_str());
//	std::cout << szJsonData << "\n";
	if(HttpSocket::getInstance()->WX_V2)
	{
		sprintf(url,"https://wx2.qq.com/cgi-bin/mmwebwx-bin/webwxgetcontact?lang=zh_CN&pass_ticket=%s&seq=0&skey=%s&r=%ld",HttpSocket::getInstance()->pass_ticket.c_str(),HttpSocket::getInstance()->skey.c_str(),tm);
	}else
	{
		sprintf(url,"https://wx.qq.com/cgi-bin/mmwebwx-bin/webwxgetcontact?lang=zh_CN&pass_ticket=%s&seq=0&skey=%s&r=%ld",HttpSocket::getInstance()->pass_ticket.c_str(),HttpSocket::getInstance()->skey.c_str(),tm);
	}
	pagebuf = HttpSocket::getInstance()->urlPost(url,szJsonData);
	if(pagebuf.empty())
	{
		return false;
	}
	parseGetContact(pagebuf.c_str());
	return true;
}

bool WxInitMessage::webWXbatchgetcontact()
{
	std::cout << "________________webWXbatchgetcontact________________\n";
	std::string pagebuf;
	time_t tm;
	time(&tm);
	char url[255];
	char szJsonData[1024];
	memset(url,0,sizeof(url));
	memset(szJsonData,0,sizeof(szJsonData));
	sprintf(szJsonData,"{\"BaseRequest\":{\"Uin\":%s,\"Sid\":\"%s\",\"Skey\":\"%s\",\"DeviceID\":\"%s\"},\"Count\":2,\"List\":[{\"UserName\":\"%s\",\"EncryChatRoomId\":\"\"},{\"UserName\":\"%s\",\"EncryChatRoomId\":\"\"}]}",HttpSocket::getInstance()->wxuin.c_str(),HttpSocket::getInstance()->wxsid.c_str(),HttpSocket::getInstance()->skey.c_str(),deviceId.c_str(),userName.c_str(),userName.c_str());
	//std::cout << szJsonData << "\n";
	if(HttpSocket::getInstance()->WX_V2)
	{
		sprintf(url,"https://wx2.qq.com/cgi-bin/mmwebwx-bin/webwxbatchgetcontact?type=ex&r=%ld&pass_ticket=%s",tm,HttpSocket::getInstance()->pass_ticket.c_str());

	}else
	{
		sprintf(url,"https://wx.qq.com/cgi-bin/mmwebwx-bin/webwxbatchgetcontact?type=ex&r=%ld&pass_ticket=%s",tm,HttpSocket::getInstance()->pass_ticket.c_str());
	}
	pagebuf = HttpSocket::getInstance()->urlPost(url,szJsonData);
	if(pagebuf.empty())
	{
		return false;
	}
	std::cout << "________________webWXbatchgetcontact________________\n";

	return true;
}

void WxInitMessage::GetMessageStart()
{
	HttpSocket::getInstance()->StartHttpSocket();
	getDeviceId();
	webWXinit();
	webWXstatusnotify();
	webWXgetcontact();
	webWXbatchgetcontact();
}
