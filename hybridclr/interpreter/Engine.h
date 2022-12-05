#pragma once

#include <stack>

#include "../CommonDef.h"

#include "gc/GarbageCollector.h"
#include "vm/Exception.h"
#include "vm/StackTrace.h"

#include "../metadata/MetadataUtil.h"
#include "../Config.h"

#include "InterpreterDefs.h"
#include "MemoryUtil.h"
#include "MethodBridge.h"


//#if DEBUG
//#define PUSH_STACK_FRAME(method) do { \
//	Il2CppStackFrameInfo stackFrameInfo = { method, (uintptr_t)method->methodPointer }; \
//	il2cpp::vm::StackTrace::PushFrame(stackFrameInfo); \
//} while(0)
//
//#define POP_STACK_FRAME() do { il2cpp::vm::StackTrace::PopFrame(); } while(0)
//
//#else 
#define PUSH_STACK_FRAME(method)
#define POP_STACK_FRAME() 
//#endif

namespace hybridclr
{
namespace interpreter
{

	/// @brief 机器状态
	class MachineState
	{
	public:
		MachineState()
		{
			Config& hc = Config::GetIns();//获取配置
			_stackSize = (int32_t)hc.GetInterpreterThreadObjectStackSize();//从配置表获取不会被GC回收的栈最大大小
			_stackBase = (StackObject*)il2cpp::gc::GarbageCollector::AllocateFixed(hc.GetInterpreterThreadObjectStackSize() * sizeof(StackObject), nullptr);//向unity底层利用BoehmGC算法申请一块固定大小的堆栈空间,这块内存不会被GC标记和回收。
			std::memset(_stackBase, 0, _stackSize * sizeof(StackObject));//进行清空操作
			_stackTopIdx = 0;//栈顶索引设置为0

			_frameBase = (InterpFrame*)IL2CPP_CALLOC(hc.GetInterpreterThreadFrameStackSize(), sizeof(InterpFrame));//看底层最后是调用了calloc直接向系统申请了配置表中想要的栈帧大小
			_frameCount = (int32_t)hc.GetInterpreterThreadFrameStackSize();//设置栈帧最大大小
			_frameTopIdx = 0;//栈帧顶索引

			_exceptionFlowBase = (ExceptionFlowInfo*)IL2CPP_CALLOC(hc.GetInterpreterThreadExceptionFlowSize(), sizeof(ExceptionFlowInfo));//看底层最后是调用了calloc直接向系统申请了配置表中想要异常流大小
			_exceptionFlowCount = (int32_t)hc.GetInterpreterThreadExceptionFlowSize();//设置异常流最大大小
			_exceptionFlowTopIdx = 0;//异常流栈顶索引
		}

		~MachineState()
		{
			il2cpp::gc::GarbageCollector::FreeFixed(_stackBase);//进行释放
			IL2CPP_FREE(_frameBase);//进行释放
			IL2CPP_FREE(_exceptionFlowBase);//进行释放
		}

		/// @brief 根据函数参数个数分配空间
		/// @param argCount 
		/// @return 
		StackObject* AllocArgments(int32_t argCount)
		{
			return AllocStackSlot(argCount);
		}

		/// @brief 得到基础空间指针首地址
		/// @return 
		StackObject* GetStackBasePtr() const
		{
			return _stackBase;
		}

		/// @brief 获取栈顶索引
		/// @return 
		int32_t GetStackTop() const
		{
			return _stackTopIdx;
		}

		/// @brief 分配栈槽
		/// @param slotNum 
		/// @return image.png
		StackObject* AllocStackSlot(int32_t slotNum)
		{
			if (_stackTopIdx + slotNum > _stackSize)//如果当前已申请的+要申请的空间大小大于初始设置的最大大小,那么就报栈溢出异常
			{
				il2cpp::vm::Exception::Raise(il2cpp::vm::Exception::GetStackOverflowException("AllocStackSlot"));//告诉上层我这引发AllocStackSlot异常
			}
			StackObject* dataPtr = _stackBase + _stackTopIdx;//得到栈顶指针其实也就是指针执行你刚请求的空间的首地址
			_stackTopIdx += slotNum;//栈顶索引增加slotNum大小
#if DEBUG
			std::memset(dataPtr, 0xEA, slotNum * sizeof(StackObject));//用来包内存0XEA错误 ??
#endif
			return dataPtr;//返回刚才申请的slotNum大小空间的首地址
		}

		/// @brief 设置栈顶索引
		/// @param oldTop 
		void SetStackTop(int32_t oldTop)
		{
			_stackTopIdx = oldTop;
		}

		/// @brief 获取栈帧栈顶索引
		/// @return 
		uint32_t GetFrameTopIdx() const
		{
			return _frameTopIdx;
		}

		/// @brief 塞入一个栈帧数据
		/// @return 
		InterpFrame* PushFrame()
		{
			if (_frameTopIdx >= _frameCount)//如果当前已申请的+要申请的空间大小大于初始设置的最大大小,那么就报栈溢出异常
			{
				il2cpp::vm::Exception::Raise(il2cpp::vm::Exception::GetStackOverflowException("AllocFrame"));//告诉上层我这引发AllocFrame异常
			}
			return _frameBase + _frameTopIdx++;//返回现在申请的一个栈帧数据的首地址,并且栈顶索引移动一位
		}

		/// @brief 将栈帧大小减少
		void PopFrame()
		{
			IL2CPP_ASSERT(_frameTopIdx > 0);
			--_frameTopIdx;
		}

		/// @brief 将栈帧大小减少count
		/// @param count 
		void PopFrameN(int32_t count)
		{
			IL2CPP_ASSERT(count > 0 && _frameTopIdx >= count);
			_frameTopIdx -= count;
		}

		/// @brief 获取栈帧栈顶指针
		/// @return 
		InterpFrame* GetTopFrame() const
		{
			if (_frameTopIdx > 0)
			{
				return _frameBase + _frameTopIdx - 1;
			}
			else
			{
				return nullptr;
			}
		}

		/// @brief 分配异常流
		/// @param count 
		/// @return 
		ExceptionFlowInfo* AllocExceptionFlow(int32_t count)
		{
			if (_exceptionFlowTopIdx + count < _exceptionFlowCount)  //如果申请的空间在合适范围
			{
				ExceptionFlowInfo* efi = _exceptionFlowBase + _exceptionFlowTopIdx;//得到申请空间的首地址
				_exceptionFlowTopIdx += count;//栈顶索引加count大小
				return efi;//返回申请首地址
			}
			il2cpp::vm::Exception::Raise(il2cpp::vm::Exception::GetExecutionEngineException("AllocExceptionFlowZero")); //如果当前已申请的+要申请的空间大小大于初始设置的最大大小,那么就报栈溢出异常
			return nullptr;//返回null
		}

		/// @brief 获取异常流栈顶大小
		/// @return 
		uint32_t GetExceptionFlowTopIdx() const
		{
			return _exceptionFlowTopIdx;
		}

		/// @brief 设置异常流栈顶索引
		/// @param exTopIdx 
		void SetExceptionFlowTopIdx(uint32_t exTopIdx)
		{
			_exceptionFlowTopIdx = exTopIdx;
		}

		/// @brief /检测下异常流栈顶数据索引是否符合规范
		/// @param top 
		void SetExceptionFlowTop(ExceptionFlowInfo* top)
		{
			_exceptionFlowTopIdx = (int32_t)(top - _exceptionFlowBase);
			IL2CPP_ASSERT(_exceptionFlowTopIdx >= 0 && _exceptionFlowTopIdx <= _exceptionFlowCount);
		}

		/// @brief 塞入一个程序集的镜像
		/// @param image 
		void PushExecutingImage(const Il2CppImage* image)
		{
			_executingImageStack.push(image);
		}

		/// @brief 弹出一个镜像
		void PopExecutingImage()
		{
			_executingImageStack.pop();
		}

		/// @brief 获取顶部镜像
		/// @return 
		const Il2CppImage* GetTopExecutingImage() const
		{
			if (_executingImageStack.empty())
			{
				return nullptr;
			}
			else
			{
				return _executingImageStack.top();
			}
		}

		/// @brief 收集栈帧
		/// @param stackFrames 
		void CollectFrames(il2cpp::vm::StackFrames* stackFrames)
		{
			for (int32_t i = 0; i < _frameTopIdx; i++)
			{
				InterpFrame* frame = _frameBase + i;
				const MethodInfo* method = frame->method->method;
				Il2CppStackFrameInfo stackFrameInfo = {
					method
#if HYBRIDCLR_UNITY_2020_OR_NEW
					, (uintptr_t)method->methodPointer
#endif
				};
				stackFrames->push_back(stackFrameInfo);
			}
		}

	private:

		StackObject* _stackBase;//申请的栈空间首地址
		int32_t _stackSize;//最大栈大小
		int32_t _stackTopIdx;//栈顶部索引

		InterpFrame* _frameBase;//栈帧首地址
		int32_t _frameTopIdx;//最大栈帧大小
		int32_t _frameCount;//栈帧部索引

		ExceptionFlowInfo* _exceptionFlowBase;//异常流首地址
		int32_t _exceptionFlowTopIdx;//最大异常流大小
		int32_t _exceptionFlowCount;//异常流索引


		std::stack<const Il2CppImage*> _executingImageStack;//执行中的镜像集合
	};

	/// @brief 执行中的镜像
	class ExecutingInterpImageScope
	{
	public:
		ExecutingInterpImageScope(MachineState& state, const Il2CppImage* image) : _state(state)
		{
			_state.PushExecutingImage(image);
		}

		~ExecutingInterpImageScope()
		{
			_state.PopExecutingImage();
		}
		
	private:
		MachineState& _state;
	};

	/// @brief 栈帧操作,看着像操作的是_frameBaseIdx到GetFrameTopIdx()的数据
	class InterpFrameGroup
	{
	public:
		InterpFrameGroup(MachineState& ms) : _machineState(ms), _stackBaseIdx(ms.GetStackTop()), _frameBaseIdx(ms.GetFrameTopIdx())
		{

		}

		/// @brief 清理
		void CleanUpFrames()
		{
			IL2CPP_ASSERT(_machineState.GetFrameTopIdx() >= _frameBaseIdx);//保证在_frameBaseIdx到到GetFrameTopIdx()范围
			uint32_t n = _machineState.GetFrameTopIdx() - _frameBaseIdx;//多少数据
			if (n > 0)//如果有
			{
				for (uint32_t i = 0; i < n; i++)
				{
					LeaveFrame();//释放他
				}
			}
		}

		/// @brief 从解释获取栈帧数据
		/// @param imi 
		/// @param argBase 
		/// @return 
		InterpFrame* EnterFrameFromInterpreter(const InterpMethodInfo* imi, StackObject* argBase)
		{
#if IL2CPP_ENABLE_PROFILER
			il2cpp_codegen_profiler_method_enter(imi->method);
#endif
			int32_t oldStackTop = _machineState.GetStackTop();
			StackObject* stackBasePtr = _machineState.AllocStackSlot(imi->maxStackSize - imi->argStackObjectSize);
			InterpFrame* newFrame = _machineState.PushFrame();
			*newFrame = { imi, argBase, oldStackTop, nullptr, nullptr, nullptr, 0, 0 };
			PUSH_STACK_FRAME(imi->method);
			return newFrame;
		}


		InterpFrame* EnterFrameFromNative(const InterpMethodInfo* imi, StackObject* argBase)
		{
#if IL2CPP_ENABLE_PROFILER
			il2cpp_codegen_profiler_method_enter(imi->method);
#endif
			int32_t oldStackTop = _machineState.GetStackTop();
			StackObject* stackBasePtr = _machineState.AllocStackSlot(imi->maxStackSize);
			InterpFrame* newFrame = _machineState.PushFrame();
			*newFrame = { imi, stackBasePtr, oldStackTop, nullptr, nullptr, nullptr, 0, 0 };

			// if not prepare arg stack. copy from args
			if (imi->args)
			{
				IL2CPP_ASSERT(imi->argCount == metadata::GetActualArgumentNum(imi->method));
				if (imi->isTrivialCopyArgs)
				{
					CopyStackObject(stackBasePtr, argBase, imi->argStackObjectSize);
				}
				else
				{
					CopyArgs(stackBasePtr, argBase, imi->args, imi->argCount, imi->argStackObjectSize);
				}
			}
			PUSH_STACK_FRAME(imi->method);
			return newFrame;
		}

		InterpFrame* LeaveFrame()
		{
			IL2CPP_ASSERT(_machineState.GetFrameTopIdx() > _frameBaseIdx);
			POP_STACK_FRAME();
			InterpFrame* frame = _machineState.GetTopFrame();
#if IL2CPP_ENABLE_PROFILER
			il2cpp_codegen_profiler_method_exit(frame->method->method);
#endif
			if (frame->exFlowBase)
			{
				_machineState.SetExceptionFlowTop(frame->exFlowBase);
			}
			_machineState.PopFrame();
			_machineState.SetStackTop(frame->oldStackTop);
			return _machineState.GetFrameTopIdx() > _frameBaseIdx ? _machineState.GetTopFrame() : nullptr;
		}

		void* AllocLoc(size_t size)
		{
			uint32_t soNum = (uint32_t)((size + sizeof(StackObject) - 1) / sizeof(StackObject));
			//void* data = _machineState.AllocStackSlot(soNum);
			//std::memset(data, 0, soNum * 8);
			void* data = IL2CPP_MALLOC_ZERO(size);
			return data;
 		}

		size_t GetFrameCount() const { return _machineState.GetFrameTopIdx() - _frameBaseIdx; }
	private:
		MachineState& _machineState;
		int32_t _stackBaseIdx;
		uint32_t _frameBaseIdx;
	};

	class StackObjectAllocScope
	{
	private:
		MachineState& _state;
		const int32_t _originStackTop;
		const int32_t _count;
		StackObject* _data;
	public:
		StackObjectAllocScope(MachineState& state, int32_t count) : _state(state), _count(count), _originStackTop(_state.GetStackTop())
		{
			_data = state.AllocStackSlot(count);
		}

		~StackObjectAllocScope()
		{
			IL2CPP_ASSERT(_state.GetStackTop() > _originStackTop);
			_state.SetStackTop(_originStackTop);
		}

		StackObject* GetData() const { return _data; }
	};
}
}