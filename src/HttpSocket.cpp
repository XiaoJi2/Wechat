#include "HttpSocket.h"
#include "WxLog.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ParserDom.h>
#include <iconv.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
using namespace htmlcxx;

bool HttpSocket::WX_V2 = false;

HttpSocket *HttpSocket::hSocket = NULL;

HttpSocket *HttpSocket::getInstance()
{
	if(hSocket == NULL)
		hSocket = new HttpSocket();
	return hSocket;
}

HttpSocket::HttpSocket()
{
	curl_global_init(CURL_GLOBAL_ALL);
	std::cout << "creat HttpSocket\n";
}

HttpSocket::~HttpSocket()
{
	curl_global_cleanup(); 
	std::cout << "delete HttpSocket\n";
}

size_t write_data(char *ptr,size_t size,size_t nember,std::string *date)
{
	if(date == NULL)
		return 0;
	unsigned int sizes = size * nember;
	date->append(ptr,sizes);
	return sizes;
}

size_t write_image(void *ptr, size_t size, size_t nmemb, FILE *stream) {  
	    size_t written = fwrite(ptr, size, nmemb, stream);  
		    return written;  
}  
/*
int code_convert(char *from_chardata,char *to_chardata,char *inbuf,unsigned int inlen, char * outbuf,unsigned int outlen)
{
	iconv_t iv;
	char **pin = &inbuf;
	char **pout = &outbuf;

	iv = iconv_open(to_chardata,from_chardata);
	if(iv == 0)
		return -1;
	memset(outbuf,0,outlen);
	if(iconv(iv,pin,&inlen,pout,&outlen) == -1)
		return -1;
	iconv_close(iv);
	return 0;
}

int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
	return code_convert("gb2312","utf-8",inbuf,inlen,outbuf,outlen);
}
*/

std::string HttpSocket::urlGet(const char *url)
{
	std::string pagebuf;
	CURL *curl = curl_easy_init();
	WxLog::getInstance()->Debug(url);	
	curl_easy_setopt(curl,CURLOPT_URL, url);//处理url
	curl_easy_setopt(curl,CURLOPT_COOKIESESSION, true);
	curl_easy_setopt(curl,CURLOPT_COOKIEJAR, "./wx.cookie");
	curl_easy_setopt(curl,CURLOPT_COOKIEFILE, "./wx.cookie");
	curl_easy_setopt(curl,CURLOPT_CONNECTTIMEOUT,60);//在发起链接前等待的时间，如果为0，无限等
	curl_easy_setopt(curl,CURLOPT_TIMEOUT,60);//允许执行的最长时间
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_data);//检测到数据，回调函数被调用，必须为write_data函数原型
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&pagebuf);//配合CURLOPT_WRIYEFUNCTION 写入数据
	//curl_easy_setopt(curl,CURLOPT_VERBOSE,1);//打印调试信息
	if((res = curl_easy_perform(curl)) == CURLE_OK)
	{
		//std::cout << "OK" << std::endl;
		WxLog::getInstance()->Info("OK");	
	}
	else
	{
		const char * err = curl_easy_strerror(res);
		std::cout << err << std::endl;
	}
	//std::cout << pagebuf << std::endl;
	WxLog::getInstance()->Info(pagebuf);	
	curl_easy_cleanup(curl);
	return pagebuf;
}

std::string HttpSocket::urlPost(const char *url,const char *jsonData)
{
	std::string pagebuf;
	CURL *curl = curl_easy_init();
	WxLog::getInstance()->Debug(url);	
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(curl, CURLOPT_URL,url);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3L);
	curl_slist *plist = NULL;
	plist = curl_slist_append(plist,"Content-Type: application/json;charset=UTF-8");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, plist);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData);
	//curl_easy_setopt(curl,CURLOPT_VERBOSE,1);//打印调试信息
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_data);//检测到数据，回调函数被调用，必须为write_data函数原型
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&pagebuf);//配合CURLOPT_WRIYEFUNCTION 写入数据

	if((res = curl_easy_perform(curl)) == CURLE_OK)
	{
		//std::cout << "OK" << std::endl;
		WxLog::getInstance()->Info("OK");	
	}
	else
	{
		const char * err = curl_easy_strerror(res);
		std::cout << err << std::endl;
	}
	//std::cout << pagebuf << std::endl;
	WxLog::getInstance()->Info(pagebuf);	
	curl_easy_cleanup(curl);
	return pagebuf;
}

bool HttpSocket::GetUuid()
{
	std::cout << "______________________GET UUID______________________\n";
	std::string pagebuf;
	char url[] = "https://login.weixin.qq.com/jslogin?appid=wx782c26e4c19acffb&fun=new&lang=zh_CN";
	
	pagebuf = urlGet(url);
	if(pagebuf.empty())
	{
		return false;
	}
	code = pagebuf.substr(22,3);//
	if( code == "200")
	{
		uuid = pagebuf.substr(50,12);
	}
	else if(code.compare("400"))
	{
		return false;
	}

	return true;
}

bool HttpSocket::GetQRcode()
{
	std::cout << "______________________GET QRcode______________________\n";
	char url[255];
	CURL *curl = curl_easy_init();
	memset(url,0,sizeof(url));
	sprintf(url,"https://login.weixin.qq.com/qrcode/%s",uuid.c_str());
	std::cout << url << std::endl;
	FILE *qrFile = NULL;
	qrFile = fopen("QRcode.jpg","w");
	if(qrFile == NULL)
	{
		fprintf(stderr,"creat qrFile file failde\n");
		return false;
	}

	curl_easy_setopt(curl,CURLOPT_URL, url);//处理url
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,qrFile);//配合CURLOPT_WRIYEFUNCTION 写入数据
	curl_easy_perform(curl);//开始抓取
	fclose(qrFile);
	curl_easy_cleanup(curl);
	return true;
} 

bool HttpSocket::waitForLogin()
{
	std::cout << "______________________Wait For Login______________________\n";
	sleep(1);
	time_t tm;
	time(&tm);
	char url[255];
	std::string loginCode;
	std::string pagebuf;
	memset(url,0,sizeof(url));
	FILE *fp = popen("eog QRcode.jpg","r");
	if(fp == NULL)
	{
		fprintf(stderr,"popen error\n");
		return false;
	}

	sprintf(url,"https://login.weixin.qq.com/cgi-bin/mmwebwx-bin/login?uuid=%s&tip=1&_=%ld",uuid.c_str(),tm);
	pagebuf = urlGet(url);
	if(pagebuf.empty())
	{
		return false;
	}
	loginCode = pagebuf.substr(12,3);
	if(loginCode == "408")
	{
		std::cout << "login time out! \n";	
		pclose(fp);
		exit(0);
	}else if(loginCode == "201"){
		std::cout << "login wait!\n";
		memset(url,0,sizeof(url));
		sprintf(url,"https://login.weixin.qq.com/cgi-bin/mmwebwx-bin/login?uuid=%s&tip=0&r=%ld",uuid.c_str(),tm);
		pagebuf = urlGet(url);
		if(pagebuf.empty())
		{
			return false;
		}
		loginCode = pagebuf.substr(12,3);
		if(loginCode == "408")
		{
			std::cout << "login time out! \n";	
			pclose(fp);
			exit(0);
		}else if(loginCode == "200")
		{
			std::cout << "login success!\n";
			int wx_version = pagebuf.find("wx2");
			if(wx_version > 0)
			{
				WX_V2 = true;
			}
			int firstChar = pagebuf.find('\"');
			int lastChar = pagebuf.find_last_of('\"');
			int count = lastChar - firstChar;
			std::string getContent = pagebuf.substr(firstChar+1,count-1);
			std::cout << "getContent = " << getContent << "\n";
			memset(url,0,sizeof(url));
			strcpy(url,getContent.c_str());
			strcat(url,"&fun=new");
			pagebuf = urlGet(url);
			processHtml(pagebuf);
		}else
		{
			std::cout << "login error!\n";
		}
	}else
	{
		std::cout << "login error!\n";
	}
	pclose(fp);
	return true;
}

void HttpSocket::processHtml(std::string &strHtml)
{
//	char pagebuf[100000];

	HTML::ParserDom parse;
	tree<HTML::Node> dom = parse.parseTree(strHtml);
	tree<HTML::Node>::iterator it = dom.begin();
	tree<HTML::Node>::iterator end = dom.end();
	for(;it != end;it++)
	{
		if(it->text().compare("<skey>") == 0)
		{
			it->parseAttributes();
			std::string str = it.node->first_child->data;
		 	skey = str;
			std::cout << "skey = " << skey << std::endl;
		}
		if(it->text().compare("<wxsid>") == 0)
		{
			it->parseAttributes();
			std::string str = it.node->first_child->data;
			wxsid += str;
			std::cout << "wxsid = " << wxsid << std::endl;
		}
		if(it->text().compare("<wxuin>") == 0)
		{
			it->parseAttributes();
			std::string str = it.node->first_child->data;
			wxuin += str;
			std::cout << "wxuin = " << wxuin << std::endl;
		}
		if(it->text().compare("<pass_ticket>") == 0)
		{
			it->parseAttributes();
			std::string str = it.node->first_child->data;
			pass_ticket += str;
			std::cout << "pass_ticket = " << pass_ticket << std::endl;
		}

	}

}

void HttpSocket::StartHttpSocket()
{
	GetUuid();
	GetQRcode();
	waitForLogin();
}
