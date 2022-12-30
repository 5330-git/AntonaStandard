#ifndef EXCEPTION_H 
#define EXCEPTION_H
#include <stdexcept>
namespace AntonaStandard{
	#define Exception_VERSION "1.1.0"
	#define Exception_ESIT_TIME "2022/12/29"
	#define Exception_AUTHOR "Anton"
}
/*
*   Decoded by utf-8
*   2022/12/19  1.0.0 初步实现
*	2022/12/19  1.1.0 添加NotFound_Error 用于标记未遍历到目标值的错误
*/
namespace AntonaStandard{
	class  Uninitialization_Error;
	class  WrongArgument_Error;
	class  UnlawfulOperation_Error;
	class  Unkown_ErrorUnkown_Error ;
	class  SystemCollapse_Error;
	class  NullPointer_Error;
	class  NotFound_Error;
	
}

// 类的声明，和部分定义
namespace AntonaStandard{
	class Uninitialization_Error :public std::logic_error {
	public:
		Uninitialization_Error(const char* msg) :std::logic_error(msg) {};
	};
	class WrongArgument_Error :public std::logic_error {
	public:
		WrongArgument_Error(const char* msg) :std::logic_error(msg) {};
	};
	class UnlawfulOperation_Error :public std::logic_error {
	public:
		UnlawfulOperation_Error(const char* msg) :std::logic_error(msg) {};
	};
	class Unkown_Error :public std::logic_error {
	public:
		Unkown_Error(const char* msg) :std::logic_error(msg) {};
	};
	class SystemCollapse_Error:public std::runtime_error{
	public:
		SystemCollapse_Error(const char* msg):std::runtime_error(msg){};
	};
	class NullPointer_Error:public std::logic_error{
	public:
		NullPointer_Error(const char* msg):std::logic_error(msg){};
	};
	class NotFound_Error:public std::logic_error{
	public:
		NotFound_Error(const char* msg):std::logic_error(msg){};
	};
}



#endif 



