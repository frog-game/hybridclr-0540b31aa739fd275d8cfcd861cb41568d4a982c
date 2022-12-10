#pragma once
#include "Image.h"

namespace hybridclr
{
namespace metadata
{
	struct AOTFieldData
	{
		uint32_t typeDefIndex; // rowIndex - 1
		const Il2CppFieldDefinition* fieldDef;
	};

	/// @brief 元数据模式
	enum class HomologousImageMode
	{
		CONSISTENT,//一致模式,即补充的dll与打包时裁剪后的dll精确一致
		SUPERSET,//超集模式,即补充的dll是打包时裁剪后的dll的超集，包含了裁剪dll的所有元数据
	};

	/// @brief AOT元数据镜像基类
	class AOTHomologousImage : public Image
	{
	public:

		static AOTHomologousImage* FindImageByAssembly(const Il2CppAssembly* ass);
		static AOTHomologousImage* FindImageByAssemblyLocked(const Il2CppAssembly* ass, il2cpp::os::FastAutoLock& lock);
		static void RegisterLocked(AOTHomologousImage* image, il2cpp::os::FastAutoLock& lock);

		AOTHomologousImage() : _aotAssembly(nullptr) { }

		/// @brief 获取程序集
		/// @return 
		const Il2CppAssembly* GetAOTAssembly() const
		{
			return _aotAssembly;
		}

		LoadImageErrorCode Load(const byte* imageData, size_t length);

		bool GetModuleIl2CppType(Il2CppType& type, uint32_t moduleRowIndex, uint32_t typeNamespace, uint32_t typeName, bool raiseExceptionIfNotFound) override;
	protected:
		const Il2CppAssembly* _aotAssembly;//程序集
	};
}
}