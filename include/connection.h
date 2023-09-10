#ifndef _CONNECTION_H 
#define _CONNECTION_H

#include <mysql/mysql.h>
#include <string>
#include <ctime>
#include "public.h"
using namespace std;

//MySQL增删改查操作
class Connection
{
public:
	// 初始化数据库连接
	Connection();
	
	// 释放数据库连接资源
	~Connection();
	
	// 连接数据库
	bool connect(string ip, unsigned short port, string user, string password, string dbname);
	// 更新操作 insert、delete、update
	bool update(string sql);
	// 查询操作 select
	MYSQL_RES* query(string sql);
	
	//刷新一下连接的时间
	void refreshAliveTime() { _alivetime = clock(); }//等于当前时间
	// 返回存活的时间
	clock_t getAliceTime() { return clock() - _alivetime; }

private:
	MYSQL* _conn;//表示和MySQL-server的一条连接
	clock_t _alivetime; //记录进入空闲状态的起始存活时间
};

#endif 