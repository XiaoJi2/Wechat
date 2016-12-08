#ifndef _HttpSocket_H_
#define _HttpSocket_H_
#include <curl/curl.h>
#include <string>

class HttpSocket
{
	public:
		std::string skey;
		std::string wxsid;
		std::string wxuin;
		std::string pass_ticket;
		static bool WX_V2;

	public:
		HttpSocket();
		~HttpSocket();
		static HttpSocket * getInstance();
		std::string urlPost(const char *url,const char *jsonData);
		std::string urlGet(const char *url);
		bool GetUuid();
		bool GetQRcode();
		void StartHttpSocket();
		bool waitForLogin();
		void processHtml(std::string &strHtml);
	private:
		static HttpSocket * hSocket;
		CURLcode res;
		std::string code , uuid;
		HttpSocket *httpSocket;
};


#endif
