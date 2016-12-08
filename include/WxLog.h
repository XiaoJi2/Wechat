#ifndef _WXLOG_H_
#define _WXLOG_H_
#include <string>

class WxLog{
	public :
		WxLog();
		~WxLog();
		static WxLog* getInstance();
		bool wxLogInit(const char *path);
		void Error(const char *parameter,...);
		void Info(std::string parameter);
		void Debug(std::string parameter);
		void Warn(const char *parameter,...);
		void Log(int level,std::string loginfo);

	private:
		static WxLog *wx_log;
		char logPath[100];
		char logfile[100];
		int logHandle;
		enum{LOG_ERROR,LOG_INFO,LOG_WARN,LOG_DEBUG};
};

#endif
