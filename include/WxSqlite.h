#ifndef _WXSQLITE_H_
#define _WXSQLITE_H_
#include <string>
#include "sqlite3.h"

class WxSqlite{
	public :
		std::string selectUserName;
	public :
		WxSqlite();
		~WxSqlite();
		static WxSqlite * getInstance();
		bool insertUserInfo(std::string UserName,std::string NickName,std::string RemarkName);
		bool selectUserInfo(std::string Name);

	private:
		sqlite3 * wxUserInfodb;
		static WxSqlite * wxInit;
		int result;
		const char *deteleSql;
		const char *selectSql;
		char *errmsg ;
		friend int callback( void * para, int n_column, char ** column_value, char ** column_name );
};

#endif
