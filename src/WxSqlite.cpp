#include "WxSqlite.h"
#include <iostream>
#include "sqlite3.h"
#include <stdio.h>
#include <string.h>

static int userinfoId = 0;
WxSqlite *WxSqlite::wxInit = NULL;
WxSqlite *WxSqlite::getInstance()
{
	if(wxInit == NULL)
		wxInit = new WxSqlite();
	return wxInit;
}

WxSqlite::WxSqlite()
{
    deteleSql = "delete from userinfo";
	result = sqlite3_open("./userinfo.db",&wxUserInfodb);
	if(result != SQLITE_OK)
	{
		fprintf(stderr,"Can't open UserInfoDB!\n");
	}
#if 1
	result = sqlite3_exec(wxUserInfodb,deteleSql,NULL,NULL,&errmsg);
	if(result != SQLITE_OK)
	{
		fprintf(stderr,"SQL error : %s\n" , errmsg);
	}else
	{
		printf("delete SQL OK\n");
	}
#endif	
}

WxSqlite::~WxSqlite()
{
}

bool WxSqlite::insertUserInfo(std::string UserName,std::string NickName,std::string RemarkName)
{
	errmsg = 0;
	char Buf[1024];
	memset(Buf,0,sizeof(Buf));
	sprintf(Buf,"insert into userinfo values('%d','%s','%s','%s');",userinfoId,UserName.c_str(),NickName.c_str(),RemarkName.c_str());
	result = sqlite3_exec(wxUserInfodb,Buf,NULL,NULL,&errmsg);
	if(result != SQLITE_OK)
	{
		fprintf(stderr,"SQL Error : %s\n",errmsg);
		sqlite3_free(errmsg);
		return false;
	}else{
		fprintf(stdout,"Records insert success!\n");
	}
	userinfoId++;

	return true;
}

int callback( void * para, int n_column, char ** column_value, char ** column_name )
{
	WxSqlite::getInstance()->selectUserName = column_value[0];
	//std::cout << "UserName :" <<column_value[0] << "\n";
	return 0;
}

bool WxSqlite::selectUserInfo(std::string Name)
{
	errmsg = 0;
	char Buf[1024];
	WxSqlite::getInstance()->selectUserName.clear();
	memset(Buf,0,sizeof(Buf));
	sprintf(Buf,"select UserName from userinfo where NickName = \"%s\" or RemarkName = \"%s\";",Name.c_str(),Name.c_str());
#if 0
	char **dbResult; 
	int nRow, nColumn;

	result = sqlite3_get_table(wxUserInfodb, Buf, &dbResult, &nRow, &nColumn, &errmsg);
	if(result != SQLITE_OK)
	{
		fprintf(stderr,"SQL Error : %s\n",errmsg);
		sqlite3_free(errmsg);
		return false;
	}else{
		if(nRow == 0)
		{
			std::cout << "没有这个联系人\n";
		}else{
			WxSqlite::getInstance()->selectUserName = dbResult[nColumn];
			std::cout << "UserName :" <<WxSqlite::getInstance()->selectUserName<< "\n";
			std::cout << "UserName :" <<dbResult[nColumn]<< "\n";
		}
	}
#endif
	
	result = sqlite3_exec(wxUserInfodb,Buf,callback,NULL,&errmsg);
	if(result != SQLITE_OK)
	{
		fprintf(stderr,"SQL Error : %s\n",errmsg);
		sqlite3_free(errmsg);
		return false;
	}else{
	//	fprintf(stdout,"Select success!\n");
	}
	
	return true ;

}
