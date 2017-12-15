#pragma once

#ifndef _DYNAMIC_METADATA_H_
#define _DYNAMIC_METADATA_H_

#include "ClassInfo.h"
#include "PtrArray.h"

#include <map>

#ifdef DCICOMMONEXT_MODULE
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllexport)
#else
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllimport)
#endif

namespace Ext
{

	namespace CPP
	{

		class DCICOMMONEXT_MODULE_EXPIMP MetadataManager
		{
		public:
			static void AddMetadata(ClassInfo *pClassInfo);

			static void* CreateInstance(const std::wstring &strName);

			static void GetAllClassInfo(Ext::CStdPtrArray &result);

			static long GenerateHashCode();

		private:

			static std::map<unsigned long, ClassInfo*> sm_HashCodeToMetadatas;

			static std::map<std::wstring, ClassInfo*> sm_KeyToMetadatas;

			static unsigned long sm_HashCodeManager;
		};

		#define CREATE_INSTANCE(key_name) \
			MetadataManager::CreateInstance(key_name);
	} // End namespace CPP

}// End namespace Ext
#endif // _DYNAMIC_METADATA_H_