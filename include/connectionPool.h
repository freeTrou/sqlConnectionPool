#ifndef _CONNECTIONPOOL_H 
#define _CONNECTIONPOOL_H

#include <fstream>
#include <iostream>
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <memory>
#include <condition_variable>//条件变量头文件
#include <functional>
#include "connection.h"
using namespace std;

//连接池功能模块
class ConnectionPool
{
public:
	//获取该类唯一实例
	static ConnectionPool* getConnectionPool();
	//获取一个新连接
	shared_ptr<Connection> getConnection();
private:
	//构造函数私有化
	ConnectionPool();
	//加载配置文件
	bool loadConfigFile();
	//运行在独立的线程中，专门负责产生新连接
	void produceConnectionTask();
	//扫描超时的空闲连接，进行回收
	void scannerConnectionTask();

	//连接池成员变量定义
	string _ip;
	unsigned short _port;
	string _username;
	string _password;
	string _dbname; //数据库名
	int _initSize; //连接池的初始连接量
	int _maxSize; //连接池的最大连接量
	int _maxIdleTime; //连接池最大空闲时间 新增加的连接，在空闲时间没被使用，回收释放
	int _connectionTimeout; //连接池获取连接的超时时间

	queue<Connection*> _connectionQue; //存储mysql连接的队列
	mutex _queueMutex; //维护连接队列的线程安全互斥锁
	atomic_int _connectionCnt; //记录连接的connection连接数量,原子类型
	condition_variable cv; //设置条件变量，用于连接生产线程和消费线程的通信
};

#endif 