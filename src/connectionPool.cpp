#include "connectionPool.h"

//线程安全的 懒汉单例模式
ConnectionPool* ConnectionPool::getConnectionPool()
{
	static ConnectionPool pool;//静态局部变量初始化由编译器 lock 和u nlock

	return &pool;
}

//加载配置文件
bool ConnectionPool::loadConfigFile()
{
	ifstream ifs;
	ifs.open("mysql.ini", ios::in);
	if (!ifs.is_open())//打开失败
	{
		LOG("mysql.ini file is not exist!");
		return false;
	}

	string strLine;
	while (getline(ifs, strLine))//一行一行读取
	{
		int idx = strLine.find('=', 0);
		if (-1 == idx)//此行没有=
		{
			continue;
		}
		int endidx = strLine.find('\n', idx);
		string key = strLine.substr(0,idx);
		string value = strLine.substr((size_t)idx+1,(size_t)endidx-idx-1);

		if (key == "ip")
		{
			_ip = value;
		}
		else if (key == "port")
		{
			_port = atoi(value.c_str());
		}
		else if (key == "username")
		{
			_username = value;
		}
		else if (key == "password")
		{
			_password = value;
		}
		else if (key == "dbname")
		{
			_dbname = value;
		}
		else if (key == "initSize")
		{
			_initSize = atoi(value.c_str());
		}
		else if (key == "maxSize")
		{
			_maxSize = atoi(value.c_str());
		}
		else if (key == "maxIdleTime")
		{
			_maxIdleTime = atoi(value.c_str());
		}
		else if (key == "connectionTimeOut")
		{
			_connectionTimeout = atoi(value.c_str());
		}
	}

	return true;
}

//构造函数
ConnectionPool::ConnectionPool()
{
	//1、加载配置项
	if (!loadConfigFile())
	{
		LOG("config file is not filaed");
		return;
	}

	//初始化连接
	for (int i = 0; i < _initSize; i++)
	{
		Connection* p = new Connection();
		p->connect(_ip, _port, _username, _password, _dbname);
		p->refreshAliveTime();//刷新一下起始时间
		_connectionQue.push(p);//将连接好的对象入队
		_connectionCnt++;
	}

	//启动一个新线程，作为生产者
	thread produce(std::bind(&ConnectionPool::produceConnectionTask,this));// 传人一个线程函数，可以用C接口方式,也可以用类中成员函数，但是需要用函数绑定器
	produce.detach();//设置守护线程 主线程结束，线程自动回收

	//启动一个新的定时线程，扫描多余的定时连接，超过_maxIdleTime的连接进行回收
	thread scanner(std::bind(&ConnectionPool::scannerConnectionTask,this));
	scanner.detach();
}

//帮线程函数写成类成员，最大的好处是可以访问类中其他成员
//运行在独立的线程中，专门负责生产新连接（生产者）
void ConnectionPool::produceConnectionTask()
{
	while (1)
	{
		unique_lock<mutex> lock(_queueMutex);
		while (!_connectionQue.empty())//队列不为空
		{
			//等待在此处,等待消费线程消费
			cv.wait(lock);
		}

		//队列为空，生产新连接
		if (_connectionCnt < _maxSize)//先判断当前已有连接数不超过设定的最大连接
		{
			//创建新连接
			Connection* p = new Connection();
			p->connect(_ip, _port, _username, _password, _dbname);
			p->refreshAliveTime();//刷新一下起始时间
			_connectionQue.push(p);//将连接好的对象入队
			_connectionCnt++;
		}

		//通知消费者线程，有新连接可以使用了
		cv.notify_all();
	}
}

void ConnectionPool::scannerConnectionTask()
{
	while (1)
	{
		this_thread::sleep_for(chrono::seconds(_maxIdleTime));//睡眠_maxIdleTime
		//扫描整个队列，释放超时连接
		unique_lock<mutex> lock(_queueMutex);//加锁 出作用域自动解锁
		while (_connectionCnt > _initSize)//超过初始连接量 才释放
		{
			Connection* p = _connectionQue.front();
			if (p->getAliceTime() > (_maxIdleTime * 1000))
			{
				_connectionQue.pop();
				delete p;
			}
			else//队头连接没有超时，其他连接肯定也没超时
			{
				break;
			}
		}
	}
}

//获取一个新连接 （消费）
shared_ptr<Connection> ConnectionPool::getConnection()
{
	unique_lock<mutex> lock(_queueMutex);//加锁 出作用域自动解锁
	while (_connectionQue.empty())//队列为空
	{
		//等待
		if (cv_status::timeout == cv.wait_for(lock, chrono::microseconds(_connectionTimeout)))//等待超时
		{
			if (_connectionQue.empty())//队列还是为空
			{
				LOG("获取空闲连接超时...获得连接失败");
				return nullptr;
			}
		}	
	}

	//队列不为空，可以开始消费   智能指针会自动析构，重写智能指针的释放方式
	shared_ptr<Connection> sp(_connectionQue.front(), [&](Connection* pconn) {
		//这里会在服务端线程调用，要考虑线程安全的操作
		unique_lock<mutex> lock(_queueMutex);
		pconn->refreshAliveTime();//刷新一下起始时间
		_connectionQue.push(pconn);//将连接归还到队列
	});//将队头放到智能指针里
	_connectionQue.pop();//出队
	cv.notify_all();//消费完，通知生产者线程

	return sp;
}