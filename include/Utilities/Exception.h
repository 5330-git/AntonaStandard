#ifndef EXCEPTION_H 
#define EXCEPTION_H
#include <stdexcept>

#define AntonaStandard_Exception_VERSION "1.3.0"
#define AntonaStandard_Exception_EDIT_TIME "2023/8/8"
#define AntonaStandard_Exception_AUTHOR "Anton"


/*
*   Decoded by utf-8
*   2022/12/19 v-1.0.0 初步实现
*	2022/12/19 v-1.1.0 添加NotFound_Error 用于标记未遍历到目标值的错误
*	2023/1/2   v-1.1.1 更改了项目的宏信息
*   2023/2/24  v-1.2.0 添加Conflict_Error 用于处理系统中出现的冲突错误
*   2023/8/8   v-1.3.0  修改命名空间从 AntonaStandard 到 AntonaStandard::Utilities
*/


namespace AntonaStandard{
	namespace Utilities{
		class  Uninitialization_Error;
		class  WrongArgument_Error;
		class  UnlawfulOperation_Error;
		class  Unkown_ErrorUnkown_Error ;
		class  SystemCollapse_Error;
		class  NullPointer_Error;
		class  NotFound_Error;
		class  Conflict_Error;
	}
}

// 类的声明，和部分定义
namespace AntonaStandard::Utilities{
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
	class Conflict_Error:public std::logic_error{
	public:
		Conflict_Error(const char* msg):std::logic_error(msg){};
	};
}



#endif 

