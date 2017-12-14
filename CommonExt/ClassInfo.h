#pragma once

#ifdef DCICOMMONEXT_MODULE
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllexport)
#else
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllimport)
#endif
#include <string>
namespace Ext
{

	namespace CPP
	{

		class DCICOMMONEXT_MODULE_EXPIMP ClassInfo
		{
		public:

			typedef void* (*FunForCreate)();

		public:
			ClassInfo();

			ClassInfo(ClassInfo *pBase);

			ClassInfo(ClassInfo *pBase, const std::wstring &strName, const FunForCreate pFun);

			ClassInfo(ClassInfo *pBase, const std::wstring &strCatalog, const std::wstring &strName, const FunForCreate pFun);

			ClassInfo(const ClassInfo &rSource);

			ClassInfo& operator = (const ClassInfo &rSource);

			~ClassInfo();

			void* CreateInstance();

			const std::wstring& GetCatalog();

			const std::wstring& GetName();

			const ClassInfo& GetBaseInfo();

			const unsigned long GetHashCode() const;

			bool operator== (const ClassInfo &info);

			bool IsEqual(const ClassInfo &info) const;

			bool IsSubClass(const ClassInfo &info) const;

		private:
			void Initial();

			unsigned long m_HashCode;

			std::wstring m_strCatalog;
			std::wstring m_strName;
			FunForCreate m_pFun;

			ClassInfo *m_pBase;
		};

#define RUNTIME_TYPE(class_name) \
	(&class_name::m_ClassInfo)

#define DECLARE_METADATA \
	static ClassInfo m_ClassInfo; \
	static const ClassInfo& GetTypeInfo(); \
	virtual const ClassInfo& GetRunTimeType(); \
	virtual bool IsKindOf(const ClassInfo &info) ; \
	virtual bool IsSubClass(const ClassInfo &info);

#define INITIAL_METADATA0(class_name) \
	const ClassInfo& class_name::GetTypeInfo() \
		{ return class_name::m_ClassInfo; } \
		const ClassInfo& class_name::GetRunTimeType()  \
		{ return class_name::m_ClassInfo; } \
		bool class_name::IsKindOf(const ClassInfo &info)  \
		{ return this->GetRunTimeType().IsEqual(info); } \
		bool class_name::IsSubClass(const ClassInfo &info) \
		{ return this->GetRunTimeType().IsSubClass(info); }

#define INITIAL_METADATA(class_name) \
	ClassInfo class_name::m_ClassInfo(NULL); \
	INITIAL_METADATA0(class_name)

#define INITIAL_METADATA1(class_name, base_name, key_name, fun_for_create) \
	ClassInfo class_name::m_ClassInfo(RUNTIME_TYPE(base_name), key_name, fun_for_create); \
	INITIAL_METADATA0(class_name)

#define INITIAL_METADATA2(class_name, base_name, catalog_name, key_name, fun_for_create) \
	ClassInfo class_name::m_ClassInfo(RUNTIME_TYPE(base_name), catalog_name, key_name, fun_for_create); \
	INITIAL_METADATA0(class_name)

	}

}