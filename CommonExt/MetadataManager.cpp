#include "stdafx.h"
#include "MetadataManager.h"
#include "ClassInfo.h"

using namespace Ext::CPP;

unsigned long MetadataManager::sm_HashCodeManager = 0;
std::map<unsigned long, ClassInfo*> MetadataManager::sm_HashCodeToMetadatas;
std::map<std::wstring, ClassInfo*> MetadataManager::sm_KeyToMetadatas;

long MetadataManager::GenerateHashCode()
{
	sm_HashCodeManager += 1;
	return sm_HashCodeManager;
}

void MetadataManager::AddMetadata(ClassInfo *pClassInfo)
{
	if (sm_HashCodeToMetadatas.count(pClassInfo->GetHashCode()) > 0)
		return;


	sm_HashCodeToMetadatas[pClassInfo->GetHashCode()] = pClassInfo;
	sm_KeyToMetadatas[pClassInfo->GetName()] = pClassInfo;
}

void* MetadataManager::CreateInstance(const std::wstring &strName)
{
	if (sm_KeyToMetadatas.count(strName) <= 0)
		return NULL;

	ClassInfo *pInfo = sm_KeyToMetadatas[strName];
	return pInfo->CreateInstance();
}

void Ext::CPP::MetadataManager::GetAllClassInfo(Ext::CStdPtrArray &result)
{
	std::map<unsigned long, ClassInfo*>::iterator it = sm_HashCodeToMetadatas.begin();
	for (; it != sm_HashCodeToMetadatas.end(); ++it)
	{
		ClassInfo *pTemp = it->second;
		result.Add(pTemp);
	}
}
