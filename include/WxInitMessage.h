#ifndef _WXINITMESSAGE_H_
#define _WXINITMESSAGE_H_
#include <curl/curl.h>
#include <string>
#include "HttpSocket.h"

class WxInitMessage
{
	public:
		std::string deviceId;
		std::string userName;
		std::string synckey;
		std::string myUserName;
		int synckeyVal[10];
		
	public:
		WxInitMessage();
		~WxInitMessage();
		static WxInitMessage * getInstance();
		std::string getDeviceId();
		bool parseGetContact(const char *pMsg);
		bool parseJson(const char * pMsg);
		bool webWXinit();
		bool webWXstatusnotify();
		bool webWXgetcontact();
		bool webWXbatchgetcontact();
		void GetMessageStart();
	private:
		static WxInitMessage * wxInit;
		CURLcode res;
};
#endif
