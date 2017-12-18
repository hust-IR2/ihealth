#include "stdafx.h"
#include "ClassInfo.h"
#include "MetadataManager.h"

using namespace Ext::CPP;

Ext::CPP::ClassInfo::ClassInfo()
{

}

ClassInfo::ClassInfo(ClassInfo *pBase)
{
	this->Initial();
	MetadataManager::AddMetadata(this);
}

ClassInfo::ClassInfo(ClassInfo *pBase, const std::wstring &strName, const FunForCreate pFun)
{
	this->Initial();

	this->m_pBase = pBase;
	this->m_strName = strName;
	this->m_pFun = pFun;

	MetadataManager::AddMetadata(this);
}

ClassInfo::ClassInfo(ClassInfo *pBase, const std::wstring &strCatalog, const std::wstring &strName, const FunForCreate pFun)
{
	this->Initial();

	this->m_pBase = pBase;
	this->m_strCatalog = strCatalog;
	this->m_strName = strName;
	this->m_pFun = pFun;

	MetadataManager::AddMetadata(this);
}

ClassInfo::ClassInfo(const ClassInfo &rSource)
{
	*this = rSource;
}

ClassInfo::~ClassInfo(void)
{
}

ClassInfo& ClassInfo::operator=(const ClassInfo &rSource)
{
	if (this != &rSource)
	{
		this->m_HashCode = rSource.m_HashCode;

		this->m_pBase = rSource.m_pBase;
		this->m_strCatalog = rSource.m_strCatalog;
		this->m_strName = rSource.m_strName;
		this->m_pFun = rSource.m_pFun;
	}
	return *this;
}

void* ClassInfo::CreateInstance()
{
	return (*m_pFun)();
}

const std::wstring& ClassInfo::GetCatalog()
{
	return m_strCatalog;
}

const std::wstring& ClassInfo::GetName()
{
	return m_strName;
}

void ClassInfo::Initial()
{
	this->m_HashCode = MetadataManager::GenerateHashCode();

	this->m_strCatalog = _T("");
	this->m_strName = _T("");
	this->m_pFun = NULL;

	this->m_pBase = NULL;
}

const unsigned long ClassInfo::GetHashCode() const
{
	return this->m_HashCode;
}

bool ClassInfo::IsEqual(const ClassInfo &info) const
{
	if (this->m_HashCode == info.m_HashCode)
		return true;

	ClassInfo *pBase = this->m_pBase;
	while (pBase != NULL)
	{
		if (pBase->m_HashCode == info.m_HashCode)
			return true;

		pBase = pBase->m_pBase;
	}
	return false;
}

const ClassInfo& ClassInfo::GetBaseInfo()
{
	return *(this->m_pBase);
}

bool ClassInfo::operator== (const ClassInfo &info)
{
	return this->IsEqual(info);
}

bool ClassInfo::IsSubClass(const ClassInfo &info) const
{
	if (this->m_HashCode == info.m_HashCode)
		return false;

	ClassInfo *pBase = this->m_pBase;
	while (pBase != NULL)
	{
		if (pBase->m_HashCode == info.m_HashCode)
			return true;

		pBase = pBase->m_pBase;
	}
	return false;
}