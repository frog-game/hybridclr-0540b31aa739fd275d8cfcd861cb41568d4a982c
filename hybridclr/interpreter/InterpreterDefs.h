#pragma once
#include "../CommonDef.h"
#include "../metadata/MetadataDef.h"

namespace hybridclr
{
	namespace interpreter
	{

		// from obj or arg
		enum class LocationDataType
		{
			I1,
			U1,
			I2,
			U2,
			U8,
			SR, // structure reference
			S_16, // struct size <= 16
			S_24, // struct size <= 24
			S_32, // struct size <= 32
			S_N,  // struct size = 3，5，6，7， > 8, size is described by stackObjectSize
		};

		/// 数据栈上的对象
		/// @brief 因为是联合体所以这个空间是按最大变量的字节决定,uint64_t决定了是8个字节
		union StackObject
		{
			uint64_t __u64;
			void* ptr; // can't adjust position. will raise native_invoke init args bugs.
			bool b;
			int8_t i8;
			uint8_t u8;
			int16_t i16;
			uint16_t u16;
			int32_t i32;
			uint32_t u32;
			int64_t i64;
			uint64_t u64;
			float f4;
			double f8;
			Il2CppObject* obj;
			Il2CppString* str;
			Il2CppObject** ptrObj;
		};

		static_assert(sizeof(StackObject) == 8, "requrie 8 bytes");


		enum class ExceptionFlowType
		{
			Exception,//异常
			Catch,
			Leave,
		};

		struct InterpMethodInfo;

		struct ExceptionFlowInfo
		{
			ExceptionFlowType exFlowType;
			int32_t throwOffset;
			Il2CppException* ex;
			int32_t nextExClauseIndex;
			int32_t leaveTarget;
		};

		/// @brief 解释器栈帧
		struct InterpFrame
		{
			const InterpMethodInfo* method;//函数信息
			StackObject* stackBasePtr;//函数帧帧地址
			int32_t oldStackTop;//旧的栈顶索引
			void* ret;//结果
			byte* ip;//指令

			ExceptionFlowInfo* exFlowBase;//异常流
			int32_t exFlowCount;//异常流大小
			int32_t exFlowCapaticy;//异常流容量
		};

		struct InterpExceptionClause
		{
			metadata::CorILExceptionClauseType flags;//异常类型
			int32_t tryBeginOffset;
			int32_t tryEndOffset;
			int32_t handlerBeginOffset;
			int32_t handlerEndOffset;
			int32_t filterBeginOffset;
			Il2CppClass* exKlass;
		};

		struct ArgDesc
		{
			LocationDataType type;
			uint32_t stackObjectSize; //
		};

		/// @brief 和指令,逻辑地址,函数相关的操作
		struct InterpMethodInfo
		{
			const MethodInfo* method;//函数信息
			ArgDesc* args;//函数参数
			uint32_t argCount;//参数长度
			uint32_t argStackObjectSize;//参数上面StackObject的总大小
			byte* codes;//代码数据
			uint32_t codeLength;//代码长度
			uint32_t maxStackSize; // args + locals + evalstack size
			uint32_t localVarBaseOffset;//局部变量基准偏移
			uint32_t evalStackBaseOffset;//表达式,栈顶数据基准偏移
			uint32_t localStackSize; // args + locals StackObject size
			std::vector<uint64_t> resolveDatas;//解析数据---为了节省性能开销
			std::vector<InterpExceptionClause*> exClauses;//异常方面处理
			uint32_t isTrivialCopyArgs : 1;//是不是简单类型
		};
	}
}