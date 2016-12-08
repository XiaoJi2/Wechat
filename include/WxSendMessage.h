#ifndef _WXSENDMESSAGE_H_
#define _WXSENDMESSAGE_H_

#include <string>

class WxSendMessage{
	public :
		WxSendMessage();
		~WxSendMessage();
		bool parseSendMessage(const char * pMsg);
		bool WebWxSendMsg(std::string content,std::string toUserName);
		void WxSendMessageStart();

	private:
};


#endif
