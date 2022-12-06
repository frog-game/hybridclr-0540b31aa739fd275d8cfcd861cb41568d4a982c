#pragma once

#include "os/ThreadLocalValue.h"

#include "../CommonDef.h"
#include "MethodBridge.h"
#include "Engine.h"
#include "../metadata/Image.h"

namespace hybridclr
{
namespace interpreter
{

	class InterpreterModule
	{
	public:
		/// @brief 初始化
		static void Initialize();

		/// @brief 获取当前线程局部数据
		/// @return 
		static MachineState& GetCurrentThreadMachineState();

		/// @brief 进行释放
		static void FreeThreadLocalMachineState();

		/// @brief 获取InterpMethodInfo结构体指针
		/// @param methodInfo 
		/// @return 
		static InterpMethodInfo* GetInterpMethodInfo(const MethodInfo* methodInfo);

		/// @brief Il2CppMethodPointer 指向普通执行函数
		/// 下面4个都是
		/// @param method 
		/// @return 
		static Il2CppMethodPointer GetMethodPointer(const Il2CppMethodDefinition* method);
		static Il2CppMethodPointer GetMethodPointer(const MethodInfo* method);
		static Il2CppMethodPointer GetAdjustThunkMethodPointer(const Il2CppMethodDefinition* method);
		static Il2CppMethodPointer GetAdjustThunkMethodPointer(const MethodInfo* method);

		/// @brief 获取桥接函数指针
		/// 下面2个都是
		/// @param method 
		/// @param forceStatic 
		/// @return 
		static Managed2NativeCallMethod GetManaged2NativeMethodPointer(const MethodInfo* method, bool forceStatic);
		static Managed2NativeCallMethod GetManaged2NativeMethodPointer(const metadata::ResolveStandAloneMethodSig& methodSig);

		/// @brief 获取委托指针
		/// @param method 
		/// @return 
		static InvokerMethod GetMethodInvoker(const Il2CppMethodDefinition* method);
		static InvokerMethod GetMethodInvoker(const MethodInfo* method);

		/// @brief 是不是解释器实现的
		/// @param method 
		/// @return 
		static bool IsImplementsByInterpreter(const MethodInfo* method);

	private:

		static il2cpp::os::ThreadLocalValue s_machineState; //线程本地存储的一些数据 你可以理解成每个线程有一个自己的void *value 副本数据,大家互相不干扰影响
	};
}
}
