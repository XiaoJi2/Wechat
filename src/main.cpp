#include <iostream>
#include "WxRecvMessage.h"
#include "WxLog.h"
#include "WxSendMessage.h"

int main(int argc,char **argv)
{
	WxRecvMessage wxRecvMessage;
	WxSendMessage wxSendMessage;
	WxLog::getInstance()->wxLogInit("./log");
	wxRecvMessage.webWXsyncStart();
	wxSendMessage.WxSendMessageStart();

	return 0;
}
