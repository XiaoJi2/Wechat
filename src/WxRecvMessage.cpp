#include "WxRecvMessage.h"
#include "WxLog.h"
#include "cJSON.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

static int flag = 0;
static int syncflag = 0;

WxRecvMessage::WxRecvMessage()
{

}

WxRecvMessage::~WxRecvMessage()
{

}

bool WxRecvMessage::parseName(const char *pMsg)
{
	//std::cout << "________________parseName________________\n";
	if(pMsg == NULL)
	{
		return false;
	}
	cJSON * pJson = cJSON_Parse(pMsg);
	if(pJson == NULL)
	{
		return false;
	}

	cJSON *pSub = cJSON_GetObjectItem(pJson,"ContactList");
	if(NULL == pSub)
	{
		printf("=====>can't find ContactList!\n");
		return false;
	}
	cJSON *arrayItem = cJSON_GetArrayItem(pSub,0);
	if(arrayItem)
	{
		char *pr = cJSON_Print(arrayItem);
		cJSON *item = cJSON_Parse(pr);
		cJSON * nName = cJSON_GetObjectItem(item,"NickName");
		if(NULL == nName)
		{
			printf("=====>can't find NickName!\n");
			return false;
		}
		nickName = nName->valuestring;
		//std::cout << "NickName is " << nName->valuestring << std::endl;
		cJSON * rName = cJSON_GetObjectItem(item,"RemarkName");
		if(NULL == rName)
		{
			printf("=====>can't find RemarkName!\n");
			return false;
		}
		remarkName = rName->valuestring;
		//std::cout << "RemarkName is " << rName->valuestring << std::endl;
		WxLog::getInstance()->Debug(cJSON_Print(rName));	
		WxLog::getInstance()->Debug(cJSON_Print(nName));
	}

	cJSON_Delete(pJson);
	return true;


}

bool WxRecvMessage::getNameById(std::string id)
{
	//std::cout << "________________GetNameById________________\n";
	std::string pagebuf;
	time_t tm;
	time(&tm);
	char url[255];
	char szJsonData[1024];
	memset(url,0,sizeof(url));
	memset(szJsonData,0,sizeof(szJsonData));
	sprintf(szJsonData,"{\"BaseRequest\":{\"Uin\":%s,\"Sid\":\"%s\",\"Skey\":\"%s\",\"DeviceID\":\"%s\"},\"Count\":1,\"List\":[{\"UserName\":\"%s\",\"EncryChatRoomId\":\"\"}]}",HttpSocket::getInstance()->wxuin.c_str(),HttpSocket::getInstance()->wxsid.c_str(),HttpSocket::getInstance()->skey.c_str(),WxInitMessage::getInstance()->deviceId.c_str(),id.c_str());
	//std::cout << szJsonData << "\n";
	if(HttpSocket::getInstance()->WX_V2)
	{
		sprintf(url,"https://wx2.qq.com/cgi-bin/mmwebwx-bin/webwxbatchgetcontact?type=ex&r=%ld&pass_ticket=%s",tm,HttpSocket::getInstance()->pass_ticket.c_str());

	}else
	{
		sprintf(url,"https://wx.qq.com/cgi-bin/mmwebwx-bin/webwxbatchgetcontact?type=ex&r=%ld&pass_ticket=%s",tm,HttpSocket::getInstance()->pass_ticket.c_str());
	}
	pagebuf = HttpSocket::getInstance()->urlPost(url,szJsonData);
	//std::cout << pagebuf << "\n";
	if(pagebuf.empty())
	{
		return false;
	}
	parseName(pagebuf.c_str());

	return true;

}

bool WxRecvMessage::parseJson(const char *pMsg)
{
	//std::cout << "________________RecvMessage ParseJson________________\n";
	int i = 0;
	char buf[1024];
	memset(buf, 0 ,sizeof(buf));
	synckey.clear();
	if(pMsg == NULL)
	{
		return false;
	}
	cJSON * pJson = cJSON_Parse(pMsg);
	if(pJson == NULL)
	{
		return false;
	}


	cJSON *pSub = cJSON_GetObjectItem(pJson,"AddMsgCount");
	if(NULL == pSub)
	{
		printf("=====>can't find!\n");
		return false;
	}
	if(pSub->valueint != 0)
		std::cout << "[*]收到" << pSub->valueint <<"信息" << "\n";

	cJSON * addMsgList = cJSON_GetObjectItem(pJson,"AddMsgList");
	for(; i < pSub->valueint ;i++)
	{
		cJSON *arrayItem = cJSON_GetArrayItem(addMsgList,i);

		char *pr = cJSON_Print(arrayItem);
		cJSON *item = cJSON_Parse(pr);

		cJSON *mId = cJSON_GetObjectItem(item,"MsgId");
		if(NULL == mId)
		{
			printf("=====>can't find MsgId!\n");
			return false;
		}

		//std::cout << "====>MsgId is" << mId->valuestring << "\n";
		msgId = mId->valuestring;


		cJSON * fromUserName = cJSON_GetObjectItem(item,"FromUserName");
		if(NULL == fromUserName)
		{
			printf("=====>can't find fromUserName!\n");
			return false;
		}

		//std::cout << buf << "\n";

		cJSON * msgType = cJSON_GetObjectItem(item,"MsgType");
		if(NULL == msgType)
		{
			printf("=====>can't find msgType!\n");
			return false;

		}
		if(msgType->valueint == 1)
		{
			//std::cout << msgType->valueint << "\n";
			getNameById(fromUserName->valuestring);
			cJSON * content = cJSON_GetObjectItem(item,"Content");
			if(NULL == content)
			{
				printf("=====>can't find content!\n");
				return false;
			}
			if(!nickName.empty())
			{
				std::cout << nickName <<":"<<content->valuestring <<"\n";
			}else
			{
				std::cout << remarkName <<":"<<content->valuestring <<"\n";
			}

		}
		if(msgType->valueint == 3)
		{
			if(!nickName.empty())
			{
				std::cout << nickName <<": "<< "发送了一张图片，暂时还没处理"<<"\n";
			}else
			{
				std::cout << remarkName <<": "<< "发送了一张图片，暂时还没处理" <<"\n";
			}

		}
		if(msgType->valueint == 34)
		{	
			if(!nickName.empty())
			{
				std::cout << nickName <<": "<< "语音消息，暂时还没处理"<<"\n";
			}else
			{
				std::cout << remarkName <<": "<< "语音消息，暂时还没处理" <<"\n";
			}


		}
		if(msgType->valueint == 37)
		{
			if(!nickName.empty())
			{
				std::cout << nickName <<": "<< "，暂时还没处理"<<"\n";
			}else
			{
				std::cout << remarkName <<": "<< "，暂时还没处理" <<"\n";
			}

		}
		if(msgType->valueint == 40)
		{
			if(!nickName.empty())
			{
				std::cout << nickName <<": "<< "，暂时还没处理"<<"\n";
			}else
			{
				std::cout << remarkName <<": "<< "，暂时还没处理" <<"\n";
			}


		}
		if(msgType->valueint == 42)
		{
			if(!nickName.empty())
			{
				std::cout << nickName <<": "<< "共享名片，暂时还没处理"<<"\n";
			}else
			{
				std::cout << remarkName <<": "<< "共享名片，暂时还没处理" <<"\n";
			}


		}
		if(msgType->valueint == 43)
		{
			if(!nickName.empty())
			{
				std::cout << nickName <<": "<< "视频通话消息，暂时还没处理"<<"\n";
			}else
			{
				std::cout << remarkName <<": "<< "视频通话消息，暂时还没处理" <<"\n";
			}


		}
		if(msgType->valueint == 47)
		{
			if(!nickName.empty())
			{
				std::cout << nickName <<": "<< "动画表情，暂时还没处理"<<"\n";
			}else
			{
				std::cout << remarkName <<": "<< "动画表情，暂时还没处理" <<"\n";
			}


		}
		if(msgType->valueint == 48)
		{
			if(!nickName.empty())
			{
				std::cout << nickName <<": "<< "位置消息，暂时还没处理"<<"\n";
			}else
			{
				std::cout << remarkName <<": "<< "位置消息，暂时还没处理" <<"\n";
			}


		}
		if(msgType->valueint == 49)
		{
			if(!nickName.empty())
			{
				std::cout << nickName <<": "<< "分享链接，暂时还没处理"<<"\n";
			}else
			{
				std::cout << remarkName <<": "<< "分享链接，暂时还没处理" <<"\n";
			}


		}
		if(msgType->valueint == 50)
		{
			if(!nickName.empty())
			{
				std::cout << nickName <<": "<< "，暂时还没处理"<<"\n";
			}else
			{
				std::cout << remarkName <<": "<< "，暂时还没处理" <<"\n";
			}


		}
		if(msgType->valueint == 51)
		{
			if(!nickName.empty())
			{
			//	std::cout << nickName <<": "<< "微信初始化消息，暂时还没处理"<<"\n";
			}else
			{
			//	std::cout << remarkName <<": "<< "微信初始化消息，暂时还没处理" <<"\n";
			}


		}
		if(msgType->valueint == 52)
		{
			if(!nickName.empty())
			{
				std::cout << nickName <<": "<< "，暂时还没处理"<<"\n";
			}else
			{
				std::cout << remarkName <<": "<< "，暂时还没处理" <<"\n";
			}


		}
		if(msgType->valueint == 53)
		{
			if(!nickName.empty())
			{
				std::cout << nickName <<": "<< "，暂时还没处理"<<"\n";
			}else
			{
				std::cout << remarkName <<": "<< "，暂时还没处理" <<"\n";
			}


		}
		if(msgType->valueint == 62)
		{
			if(!nickName.empty())
			{
				std::cout << nickName <<": "<< "小视频，暂时还没处理"<<"\n";
			}else
			{
				std::cout << remarkName <<": "<< "小视频，暂时还没处理" <<"\n";
			}


		}
		if(msgType->valueint == 999)
		{
			if(!nickName.empty())
			{
				std::cout << nickName <<": "<< "，暂时还没处理"<<"\n";
			}else
			{
				std::cout << remarkName <<": "<< "，暂时还没处理" <<"\n";
			}


		}
		if(msgType->valueint == 10000)
		{
			if(!nickName.empty())
			{
				std::cout << nickName <<": "<< "系统消息，暂时还没处理"<<"\n";
			}else
			{
				std::cout << remarkName <<": "<< "系统消息，暂时还没处理" <<"\n";
			}


		}
		if(msgType->valueint == 10002)
		{
			if(!nickName.empty())
			{
				std::cout << nickName <<": "<< "撤回消息，暂时还没处理"<<"\n";
			}else
			{
				std::cout << remarkName <<": "<< "撤回消息，暂时还没处理" <<"\n";
			}


		}



	}


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
	skCount = SyncKey->valueint;
	cJSON * list = cJSON_GetObjectItem(pSub,"List");
	if(NULL == list)
	{
		printf("=====>can't find list!\n");
		return false;
	}

	for(i = 0;i < SyncKey->valueint;i++)
	{
		WxLog::getInstance()->Debug(cJSON_Print(SyncKey));	
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
			skKey[i] = (int )Key->valueint;
			cJSON * Val = cJSON_GetObjectItem(item,"Val");
			if(NULL == Val)
			{
				printf("=====>can't find Val!\n");
				return false;
			}

			skVal[i] =(int ) Val->valueint;

			if(i == 0)
				sprintf(buf,"%d_%d",Key->valueint,Val->valueint);
			else
				sprintf(buf,"|%d_%d",Key->valueint,Val->valueint);
			synckey += buf;

			WxLog::getInstance()->Debug(cJSON_Print(Key));	
			WxLog::getInstance()->Debug(cJSON_Print(Val));
			WxLog::getInstance()->Info(synckey);
		}
	}

	cJSON_Delete(pJson);
	return true;
}

char * WxRecvMessage::makeJson()
{
	time_t tm;
	time(&tm);
	cJSON * pJsonRoot = NULL;

	pJsonRoot = cJSON_CreateObject();
	if(NULL == pJsonRoot)
	{
		return NULL;
	}
	cJSON * BaseRequest = NULL;
	BaseRequest = cJSON_CreateObject();
	if(NULL == BaseRequest)
	{
		return NULL;
	}
	cJSON_AddNumberToObject(BaseRequest,"Uin",atoi(HttpSocket::getInstance()->wxuin.c_str()));
	cJSON_AddStringToObject(BaseRequest,"Sid",HttpSocket::getInstance()->wxsid.c_str());
	cJSON_AddStringToObject(BaseRequest,"SKey",HttpSocket::getInstance()->skey.c_str());
	cJSON_AddStringToObject(BaseRequest,"DeviceID",WxInitMessage::getInstance()->deviceId.c_str());

	cJSON_AddItemToObject(pJsonRoot,"BaseRequest",BaseRequest);

	cJSON * SyncKey = NULL;
	SyncKey = cJSON_CreateObject();
	if(NULL == SyncKey)
	{
		return NULL;
	}

	cJSON_AddNumberToObject(SyncKey,"Count",skCount);
	cJSON * List = NULL;
	List = cJSON_CreateArray();
	if(NULL == List)
	{
		return NULL;
	}
	int i = 0;
	for(;i < skCount;i++)
	{
		cJSON * array = NULL;
		array = cJSON_CreateObject();
		if(NULL == array)
		{
			return NULL;
		}

		cJSON_AddNumberToObject(array,"Key",skKey[i]);
		cJSON_AddNumberToObject(array,"Val",skVal[i]);

		cJSON_AddItemToObject(List,"",array);
	}

	cJSON_AddItemToObject(SyncKey,"List",List);


	cJSON_AddItemToObject(pJsonRoot,"SyncKey",SyncKey);
	cJSON_AddNumberToObject(pJsonRoot,"rr",-tm);


	char *p = cJSON_Print(pJsonRoot);

	cJSON_Delete(pJsonRoot);

	return p;

}

bool WxRecvMessage::webWXsync()
{
	//std::cout << "________________webWXsync________________\n";
	std::string pagebuf;
	time_t tm;
	time(&tm);
	char url[255];
	char szJsonData[1024];
	char * mkJsonData;
	memset(url,0,sizeof(url));
	memset(szJsonData,0,sizeof(szJsonData));
	if(syncflag == 0)
	{
		sprintf(szJsonData,"{\"BaseRequest\":{\"Uin\":%s,\"Sid\":\"%s\",\"Skey\":\"%s\",\"DeviceID\":\"%s\"},\"SyncKey\":{\"Count\":4,\"List\":[{\"Key\":1,\"Val\":%d},{\"Key\":2,\"Val\":%d},{\"Key\":3,\"Val\":%d},{\"Key\":1000,\"Val\":%d}]},\"rr\":%ld}",HttpSocket::getInstance()->wxuin.c_str(),HttpSocket::getInstance()->wxsid.c_str(),HttpSocket::getInstance()->skey.c_str(),WxInitMessage::getInstance()->deviceId.c_str(),WxInitMessage::getInstance()->synckeyVal[0],WxInitMessage::getInstance()->synckeyVal[1],WxInitMessage::getInstance()->synckeyVal[2],WxInitMessage::getInstance()->synckeyVal[3],-tm);
	//	std::cout << szJsonData << "\n";
		syncflag = 1;
		/*________*/
		if(HttpSocket::getInstance()->WX_V2)
		{
			sprintf(url,"https://wx2.qq.com/cgi-bin/mmwebwx-bin/webwxsync?sid=%s&skey=%s&lang=zh_CN&pass_ticket=%s",HttpSocket::getInstance()->wxsid.c_str(),HttpSocket::getInstance()->skey.c_str(),HttpSocket::getInstance()->pass_ticket.c_str());
		}else
		{
			sprintf(url,"https://wx.qq.com/cgi-bin/mmwebwx-bin/webwxsync?sid=%s&skey=%s&lang=zh_CN&pass_ticket=%s",HttpSocket::getInstance()->wxsid.c_str(),HttpSocket::getInstance()->skey.c_str(),HttpSocket::getInstance()->pass_ticket.c_str());
		}
		printf("syncflag 00000000\n");
		pagebuf = HttpSocket::getInstance()->urlPost(url,szJsonData);
		if(pagebuf.empty())
		{
			return false;
		}

	}else{
		mkJsonData = makeJson();
	//	printf("%s\n",mkJsonData);

		if(HttpSocket::getInstance()->WX_V2)
		{
			sprintf(url,"https://wx2.qq.com/cgi-bin/mmwebwx-bin/webwxsync?sid=%s&skey=%s&lang=zh_CN&pass_ticket=%s",HttpSocket::getInstance()->wxsid.c_str(),HttpSocket::getInstance()->skey.c_str(),HttpSocket::getInstance()->pass_ticket.c_str());
		}else
		{
			sprintf(url,"https://wx.qq.com/cgi-bin/mmwebwx-bin/webwxsync?sid=%s&skey=%s&lang=zh_CN&pass_ticket=%s",HttpSocket::getInstance()->wxsid.c_str(),HttpSocket::getInstance()->skey.c_str(),HttpSocket::getInstance()->pass_ticket.c_str());
		}
//		printf("syncflag 1111111\n");
		pagebuf = HttpSocket::getInstance()->urlPost(url,mkJsonData);
		if(pagebuf.empty())
		{
			return false;
		}

	}
	parseJson(pagebuf.c_str());
	return true;

}

bool WxRecvMessage::synccheck()
{
	//std::cout << "________________synccheck________________\n";
	std::string pagebuf;
	time_t tm;
	time(&tm);
	char url[512];
	memset(url,0,sizeof(url));
	if(HttpSocket::getInstance()->WX_V2)
	{
		if(flag == 0)
		{
			sprintf(url,"https://webpush.wx2.qq.com/cgi-bin/mmwebwx-bin/synccheck?r=%ld&skey=%s&sid=%s&uin=%s&deviceid=%s&synckey=%s&_=%ld",tm,HttpSocket::getInstance()->skey.c_str(),HttpSocket::getInstance()->wxsid.c_str(),HttpSocket::getInstance()->wxuin.c_str(),WxInitMessage::getInstance()->deviceId.c_str(),WxInitMessage::getInstance()->synckey.c_str(),tm);
			flag = 1;
		}else
		{
			sprintf(url,"https://webpush.wx2.qq.com/cgi-bin/mmwebwx-bin/synccheck?r=%ld&skey=%s&sid=%s&uin=%s&deviceid=%s&synckey=%s&_=%ld",tm,HttpSocket::getInstance()->skey.c_str(),HttpSocket::getInstance()->wxsid.c_str(),HttpSocket::getInstance()->wxuin.c_str(),WxInitMessage::getInstance()->deviceId.c_str(),synckey.c_str(),tm);
		}
	}else
	{
		if(flag == 0)
		{
			sprintf(url,"https://webpush.wx.qq.com/cgi-bin/mmwebwx-bin/synccheck?r=%ld&skey=%s&sid=%s&uin=%s&deviceid=%s&synckey=%s&_=%ld",tm,HttpSocket::getInstance()->skey.c_str(),HttpSocket::getInstance()->wxsid.c_str(),HttpSocket::getInstance()->wxuin.c_str(),WxInitMessage::getInstance()->deviceId.c_str(),WxInitMessage::getInstance()->synckey.c_str(),tm);
			flag = 1;
		}else
		{
			sprintf(url,"https://webpush.wx.qq.com/cgi-bin/mmwebwx-bin/synccheck?r=%ld&skey=%s&sid=%s&uin=%s&deviceid=%s&synckey=%s&_=%ld",tm,HttpSocket::getInstance()->skey.c_str(),HttpSocket::getInstance()->wxsid.c_str(),HttpSocket::getInstance()->wxuin.c_str(),WxInitMessage::getInstance()->deviceId.c_str(),synckey.c_str(),tm);
		}
	}
	pagebuf = HttpSocket::getInstance()->urlGet(url);
	if(pagebuf.empty())
	{
		return false;
	}
	int retcode_N0 = pagebuf.find("retcode:\"0\"");
	int retcode_N1000 = pagebuf.find("retcode:\"1100\"");
	int retcode_N1001 = pagebuf.find("retcode:\"1101\"");
	int selector_N0 = pagebuf.find("selector:\"0\"");
	int selector_N2 = pagebuf.find("selector:\"2\"");
	int selector_N6 = pagebuf.find("selector:\"6\"");
	int selector_N7 = pagebuf.find("selector:\"7\"");
	if(retcode_N0 > 0)
	{
		//	std::cout << "正常" << std::endl;
		if(selector_N0 > 0)
		{
			//std::cout << "正常" << std::endl;
		}else if(selector_N2 > 0)
		{
			//std::cout << "收到消息" << std::endl;
			webWXsync();
		}else if( selector_N6 > 0)
		{
			std::cout << "收到疑似红包消息" << std::endl;
		}else if(selector_N7 > 0)
		{
			std::cout << "手机" << std::endl;
			webWXsync();
		}
	}else if(retcode_N1000 > 0)
	{
		std::cout << "失败/登出微信" << std::endl;
		exit(0);
	}else if(retcode_N1001 > 0)
	{
		std::cout << "失败/登出微信" << std::endl;
		exit(0);
	}
	return true;


}

void * getMessage(void *arg)
{
	WxRecvMessage wxRecvMessage;
	while(1)
	{
		wxRecvMessage.synccheck();
		sleep(1);
	}
	return 0;
}

void WxRecvMessage::webWXsyncStart()
{
	pthread_t pid;
	WxInitMessage::getInstance()->GetMessageStart();

	pthread_create(&pid,NULL,getMessage,NULL);
}
