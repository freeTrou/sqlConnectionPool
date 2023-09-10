#ifndef _PUBLIC_H 
#define _PUBLIC_H

#include <iostream>

//公共的文件

#define LOG(str) \
		std::cout << __FILE__ << ":" << __LINE__ << ":" << \
		__TIMESTAMP__ << ":" << str << std::endl;

#endif 
