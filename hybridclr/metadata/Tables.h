#pragma once
#include <stdint.h>

namespace hybridclr
{
namespace metadata
{
    /// @brief table类型索引
    enum class TableType
    {
        MODULE,//定义模块
        TYPEREF,//描述引用类型
        TYPEDEF,//定义类型或者接口
        FIELD_POINTER,//在非优化的元数据表中，定义累中的字段时的中间查找表
        FIELD,//定义字段
        METHOD_POINTER,//在非优化的元数据表中，在定义类的方法时的中间查找表
        METHOD,//定义方法
        PARAM_POINTER,//非优化的元数据表中，在定位方法的参数时的中间查找表
        PARAM,//定义方法参数
        INTERFACEIMPL,//定义类与所实现的接口
        MEMBERREF, /* 0xa *///定义引用的方法或字段
        CONSTANT,//定义#Blob流中存储的常数值与相应的字段, 参数或属性的关系
        CUSTOMATTRIBUTE,//定义自定义属性
        FIELDMARSHAL,//定义托管与非托管的互操作时的参数或字段的Marshal操作
        DECLSECURITY,//定义安全码
        CLASSLAYOUT,//描述与类被加载时应如何布局的信息
        FIELDLAYOUT, /* 0x10 *///描述了单个字段的偏移或是序号
        STANDALONESIG,//用于定义本地变量或间接调用(calli)的签名
        EVENTMAP,//类到事件的映射表，不是中间查找表(区别于Ptr结尾的表)
        EVENT_POINTER,//非优化的元数据表中，定位事件的中间查找表
        EVENT,//定义事件
        PROPERTYMAP,//类到属性的映射表
        PROPERTY_POINTER,//非优化的元数据表中，定位属性时的中间查找表
        PROPERTY,//定义属性
        METHODSEMANTICS,//描述方法与事件或属性的关联，以及如何关联
        METHODIMPL,//描述方法的重载
        MODULEREF, /* 0x1a *///描述引用模块
        TYPESPEC,//定义type specification(构造函数，指针，数组等)
        IMPLMAP,//托管与非托管的互操作时的P/Invoke操作
        FIELDRVA,//定义字段到数据的映射关系
        UNUSED6,//占位
        UNUSED7,//占位
        ASSEMBLY, /* 0x20 *///定义当前程序集
        ASSEMBLYPROCESSOR,//程序集处理器相关
        ASSEMBLYOS,//定义和平台Id,版本号相关的东西
        ASSEMBLYREF,//引用程序集
        ASSEMBLYREFPROCESSOR,//引用程序集处理器相关
        ASSEMBLYREFOS,//定义和引用程序集平台Id,版本号相关的东西
        FILE,//描述当前程序集中的其他文件
        EXPORTEDTYPE,//仅存在于程序集主模块中，描述程序集中其他模块定义的导出类
        MANIFESTRESOURCE,//描述托管资源
        NESTEDCLASS,//描述嵌套定义类及相应的包含类
        GENERICPARAM, /* 0x2a *///描述泛型类或泛型方法的类型参数
        METHODSPEC,//描述泛型方法的实例化
        GENERICPARAMCONSTRAINT,//描述泛型类或泛型方法的类型参数的相应限制
        UNUSED8,//占位
        UNUSED9,//占位
        UNUSED10,//占位
        /* Portable PDB tables *///跨平台 PDB 库
        DOCUMENT, /* 0x30 *///文档
        METHODBODY,//方法主体
        LOCALSCOPE,//局部作用域
        LOCALVARIABLE,//局部变量
        LOCALCONSTANT,//局部常量
        IMPORTSCOPE,//导入
        STATEMACHINEMETHOD,//状态机方法.这个东西你可以认为是对方法利用状态机在管理,切换
        CUSTOMDEBUGINFORMATION//调试信息
    };


    // 0
    struct TbModule
    {
        uint16_t generation;
        uint32_t name;
        uint32_t mvid;
        uint32_t encid;
        uint32_t encBaseId;
    };

    // 1
    struct TbTypeRef
    {
        uint32_t resolutionScope;
        uint32_t typeName;
        uint32_t typeNamespace;
    };

    // 2
    struct TbTypeDef
    {
        uint32_t flags;
        uint32_t typeName;
        uint32_t typeNamespace;
        uint32_t extends;
        uint32_t fieldList;
        uint32_t methodList;
    };

    // 3 FIELD_POINTER

    // 4
    struct TbField
    {
        uint32_t flags;
        uint32_t name;
        uint32_t signature;
    };

    // 5 METHOD_POINTER
        
    // 6
    struct TbMethod
    {
        uint32_t rva;
        uint16_t implFlags;
        uint16_t flags;
        uint32_t name;
        uint32_t signature;
        uint32_t paramList;
    };

    // 7 PARAM_POINTER

    // 8
    struct TbParam
    {
        uint16_t flags;
        uint16_t sequence;
        uint32_t name;
    };

    // 9
    struct TbInterfaceImpl
    {
        uint32_t classIdx;
        uint32_t interfaceIdx;
    };

    // 0xa
    struct TbMemberRef
    {
        uint32_t classIdx;
        uint32_t name;
        uint32_t signature;
    };

    struct TbConstant
    {
        uint8_t type; // 实际上占2字节
        uint32_t parent;
        uint32_t value;
    };

    struct TbCustomAttribute
    {
        uint32_t parent;
        uint32_t type;
        uint32_t value;
    };

    struct TbFieldMarshal
    {
        uint32_t parent;
        uint32_t nativeType;
    };

    struct TbDeclSecurity
    {
        uint16_t action;
        uint32_t parent;
        uint32_t permissionSet;
    };

    struct TbClassLayout
    {
        uint16_t packingSize;
        uint32_t classSize;
        uint32_t parent;
    };

    // 0x10
    struct TbFieldLayout
    {
        uint32_t offset;
        uint32_t field;
    };

    struct TbStandAloneSig
    {
        uint32_t signature; // 指向 blob heap的位置
    };

    struct TbEventMap
    {
        uint32_t parent;
        uint32_t eventList;
    };

    // 0x13 EVENT_POINTER

    // 0x14
    struct TbEvent
    {
        uint16_t eventFlags;
        uint32_t name;
        uint32_t eventType;
    };

    struct TbPropertyMap
    {
        uint32_t parent;
        uint32_t propertyList;
    };

    // PROPERTY_POINTER

    struct TbProperty
    {
        uint16_t flags;
        uint32_t name;
        uint32_t type;
    };

    struct TbMethodSemantics
    {
        uint16_t semantics;
        uint32_t method;
        uint32_t association;
    };

    struct TbMethodImpl
    {
        uint32_t classIdx;
        uint32_t methodBody;
        uint32_t methodDeclaration;
    };

    struct TbModuleRef
    {
        uint32_t name;
    };

    struct TbTypeSpec
    {
        uint32_t signature;
    };

    struct TbImplMap
    {
        uint16_t mappingFlags;
        uint32_t memberForwarded;
        uint32_t importName;
        uint32_t importScope;
    };

    struct TbFieldRVA
    {
        uint32_t rva;
        uint32_t field;
    };

    // UNUSED 6
    // UNUSED 7

    struct TbAssembly
    {
        uint32_t hashAlgId;
        uint16_t majorVersion;
        uint16_t minorVersion;
        uint16_t buildNumber;
        uint16_t revisionNumber;
        uint32_t flags;
        uint32_t publicKey;
        uint32_t name;
        uint32_t culture;
    };

    struct TbAssemblyProcessor
    {
        uint32_t processor;
    };

    struct TbAssemblyOS
    {
        uint32_t osPlatformID;
        uint32_t osMajorVersion;
        uint32_t osMinorVersion;
    };

    struct TbAssemblyRef
    {
        uint16_t majorVersion;
        uint16_t minorVersion;
        uint16_t buildNumber;
        uint16_t revisionNumber;
        uint32_t flags;
        uint32_t publicKeyOrToken;
        uint32_t name;
        uint32_t culture;
        uint32_t hashValue;
    };

    struct TbAssemblyRefProcessor
    {
        uint32_t processor;
        uint32_t assemblyRef;
    };

    struct TbAssemblyRefOS
    {
        uint32_t osPlatformID;
        uint32_t osMajorVersion;
        uint32_t osMinorVersion;
        uint32_t assemblyRef;
    };

    struct TbFile
    {
        uint32_t flags;
        uint32_t name;
        uint32_t hashValue;
    };

    struct TbExportedType
    {
        uint32_t flags;
        uint32_t typeDefId;
        uint32_t typeName;
        uint32_t typeNamespace;
        uint32_t implementation;
    };

    struct TbManifestResource
    {
        uint32_t offset;
        uint32_t flags;
        uint32_t name;
        uint32_t implementation;
    };

    struct TbNestedClass
    {
        uint32_t nestedClass;
        uint32_t enclosingClass;
    };

    struct TbGenericParam
    {
        uint16_t number;
        uint16_t flags;
        uint32_t owner;
        uint32_t name;
    };

    struct TbMethodSpec
    {
        uint32_t method;
        uint32_t instantiation;
    };

    struct TbGenericParamConstraint
    {
        uint32_t owner;
        uint32_t constraint;
    };

    // 以下这些都不是tables的类型
    // 但mono特殊处理一下，额外也加到这个表中

    // size 84
    struct TbSymbolDocument
    {

    };

    // size 52
    struct TbSymbolMethodBody
    {

    };

    // size 20
    struct TbSymbolLocalScope
    {

    };

    // size 56
    struct TbSymbolLocalVariable
    {

    };

    // size 24
    struct TbSymbolConstant
    {

    };

    // SymUsing. size 8
    struct TbSymbolImportScope
    {

    };


    struct TbSymbolMisc
    {

    };

    struct TbSymbolString
    {

    };

}

}