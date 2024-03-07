#ifndef GLOBALS_EXCEPTION_H 
#define GLOBALS_EXCEPTION_H
/**
 * @file Exception.h
 * @author Anton (yunye_helloworld@qq.com)
 * @brief 定义一些通用的异常
 * @version 1.0.0
 * @date 2024-03-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <stdexcept>

namespace AntonaStandard{
	/**
	 * @brief 该组件主要包含一些需要经常被其它组件引用的内容，比如异常类
	 * 
	 */
	namespace Globals{
		class  Uninitialization_Error;
		class  WrongArgument_Error;
		class  UnlawfulOperation_Error;
		class  Unkown_ErrorUnkown_Error ;
		class  SystemCollapse_Error;
		class  NullPointer_Error;
		class  NotFound_Error;
		class  Conflict_Error;
		class  FailToLoadDll_Error;
		class  FailToUnloadDll_Error;
		class  FailToGetFunction_Error;
	}
}

// 类的声明，和部分定义
namespace AntonaStandard::Globals{
	/**
	 * @brief 未初始化错误
	 * 
	 */
	class Uninitialization_Error :public std::logic_error {
	public:
		Uninitialization_Error(const char* msg) :std::logic_error(msg) {};
	};
	/**
	 * @brief 错误参数错误
	 * 
	 */
	class WrongArgument_Error :public std::logic_error {
	public:
		WrongArgument_Error(const char* msg) :std::logic_error(msg) {};
	};
	/**
	 * @brief 非法操作错误
	 * 
	 */
	class UnlawfulOperation_Error :public std::logic_error {
	public:
		UnlawfulOperation_Error(const char* msg) :std::logic_error(msg) {};
	};
	/**
	 * @brief 未知错误
	 * 
	 */
	class Unkown_Error :public std::logic_error {
	public:
		Unkown_Error(const char* msg) :std::logic_error(msg) {};
	};
	/**
	 * @brief 系统崩溃错误
	 * 
	 */
	class SystemCollapse_Error:public std::runtime_error{
	public:
		SystemCollapse_Error(const char* msg):std::runtime_error(msg){};
	};
	/**
	 * @brief 空指针访问错误
	 * 
	 */
	class NullPointer_Error:public std::logic_error{
	public:
		NullPointer_Error(const char* msg):std::logic_error(msg){};
	};
	/**
	 * @brief 未找到错误
	 * 
	 */
	class NotFound_Error:public std::logic_error{
	public:
		NotFound_Error(const char* msg):std::logic_error(msg){};
	};
	/**
	 * @brief 冲突错误
	 * 
	 */
	class Conflict_Error:public std::logic_error{
	public:
		Conflict_Error(const char* msg):std::logic_error(msg){};
	};
	/**
	 * @brief 动态库加载失败错误
	 * 
	 */
	class FailToLoadDll_Error:public std::runtime_error{
	public:
		FailToLoadDll_Error(const char* msg):std::runtime_error(msg){};
	};
	/**
	 * @brief 动态库卸载失败
	 * 
	 */
	class FailToUnloadDll_Error:public std::runtime_error{
	public:
		FailToUnloadDll_Error(const char* msg):std::runtime_error(msg){};
	};
	/**
	 * @brief 动态库访问函数错误，一般是函数的符号未找到
	 * 
	 */
	class FailToGetFunction_Error:public std::runtime_error{
	public:
		FailToGetFunction_Error(const char* msg):std::runtime_error(msg){};
	};
}



#endif 

