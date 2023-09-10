#define _CRT_SECURE_NO_WARNINGS 
#include <iostream>
#include <ctime>
#include "connection.h"
#include "connectionPool.h"

int main()
{
	Connection conn;
	conn.connect("127.0.0.1", 3306, "root", "w123456", "chattest");
	/*
	Connection conn;
	char sql[1024] = { 0 };
	sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");

	conn.connect("127.0.0.1", 3306, "root", "w123456", "chattest");
	conn.update(sql);

	ConnectionPool* pool = ConnectionPool::getConnectionPool();
	for (int i = 0; i < 10000; i++)
	{
		shared_ptr<Connection> sp = pool->getConnection();
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
		sp->update(sql);
	}
	*/

	

	clock_t startTime = clock();
	
	thread t1([]() {
		for (int i = 0; i < 1250; ++i)
		{
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
			conn.connect("127.0.0.1", 3306, "root", "w123456", "chattest");
			conn.update(sql);
		}
		});
	thread t2([]() {
		for (int i = 0; i < 1250; ++i)
		{
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
			conn.connect("127.0.0.1", 3306, "root", "w123456", "chattest");
			conn.update(sql);
		}
		});
	thread t3([]() {
		for (int i = 0; i < 1250; ++i)
		{
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
			conn.connect("127.0.0.1", 3306, "root", "w123456", "chattest");
			conn.update(sql);
		}
		});
	thread t4([]() {
		for (int i = 0; i < 1250; ++i)
		{
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
			conn.connect("127.0.0.1", 3306, "root", "w123456", "chattest");
			conn.update(sql);
		}
		});

	t1.join();
	t2.join();
	t3.join();
	t4.join();

	clock_t endTime = clock();
	std::cout << endTime - startTime << "ms" << std::endl;

	return 0;
}