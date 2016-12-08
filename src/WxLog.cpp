#include "WxLog.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdarg.h>

#define BUFSIZE 100000

WxLog * WxLog::wx_log = NULL;

WxLog * WxLog::getInstance()
{
	if(wx_log == NULL)
		wx_log = new WxLog();
	return wx_log;
}

WxLog::WxLog()
{

}

WxLog::~WxLog()
{
	if(logHandle > 0)
		close(logHandle);
}

bool WxLog::wxLogInit(const char *path)
{	
	time_t now;
	struct tm *timenow;
	time(&now);
	timenow = localtime(&now);
	char timeToDay[20];
	memset(logPath,0,sizeof(logPath));
	memset(logfile,0,sizeof(logfile));
	memset(timeToDay,0,sizeof(timeToDay));
	sprintf(timeToDay,"%04d-%02d-%02d-%02d",timenow->tm_year+1900,timenow->tm_mon+1,timenow->tm_mday,timenow->tm_hour);
	if(mkdir(path,0755) == -1)
	{
		fprintf(stderr ,"Creat log path failed!\n");
		return false;
	}
	strcpy(logPath,path);
	sprintf(logfile,"%s/%s.log",logPath,timeToDay);
	logHandle = open(logfile,O_CREAT | O_RDWR | O_APPEND ,0644);
	if(logHandle < 0)
	{
		fprintf(stderr ,"Creat logfile failed!\n");
		return false;
	}
	return true;

}

void WxLog::Log(int level,const std::string loginfo)
{
	const char * infos[4] = {"ERROR","INFO","WARN","DEBUG"};
	time_t now;
	struct tm *timenow;
	time(&now);
	timenow = localtime(&now);
	std::string logbuf; 
	if(logHandle > 0)
	{
		char flage[70] = {0};
		sprintf(flage,"[%s]%04d-%02d-%02d %02d:%02d:%02d\n",infos[level],timenow->tm_year+1900 ,timenow->tm_mon+1  ,timenow->tm_mday ,timenow->tm_hour,timenow->tm_min,timenow->tm_sec);
		logbuf = flage + loginfo;
		int handle = write(logHandle,logbuf.c_str(),logbuf.size()+1);
		if(handle < 0)
		{
			fprintf(stderr,"Write log failed!\n");
		}
	}
}

void WxLog::Info(std::string parameter)
{
	Log(LOG_INFO,parameter);
}


void WxLog::Debug(std::string parameter)
{
	Log(LOG_DEBUG,parameter);
}
