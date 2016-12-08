#ifndef _WXRECVMESSAGE_H_
#define _WXRECVMESSAGE_H_

#include "WxInitMessage.h"
#include <string>

class WxRecvMessage{
	public:
		std::string synckey;
		int skCount;
		int skKey[100];
		int skVal[100];
	public:
		WxRecvMessage();
		~WxRecvMessage();
		bool parseName(const char *pMsg);
		bool getNameById(std::string id);
		bool parseJson(const char *pMsg);
		char * makeJson();
		bool webWXsync();
		bool synccheck();
		void webWXsyncStart();

	private:
		std::string nickName;
		std::string remarkName;
		std::string msgId;

		friend void * getMessage(void * arg);
};
#endif
