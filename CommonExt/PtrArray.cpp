#include "StdAfx.h"
#include "PtrArray.h"

using namespace Ext;

CStdPtrArray::CStdPtrArray(int iPreallocSize) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(iPreallocSize)
{
	ASSERT(iPreallocSize>=0);
	if( iPreallocSize > 0 ) m_ppVoid = static_cast<LPVOID*>(malloc(iPreallocSize * sizeof(LPVOID)));
}

CStdPtrArray::CStdPtrArray(const CStdPtrArray& src) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(0)
{
	for(int i=0; i<src.GetSize(); i++)
		Add(src.GetAt(i));
}

CStdPtrArray::~CStdPtrArray()
{
	if( m_ppVoid != NULL ) free(m_ppVoid);
}

void CStdPtrArray::Empty()
{
	if( m_ppVoid != NULL ) free(m_ppVoid);
	m_ppVoid = NULL;
	m_nCount = m_nAllocated = 0;
}

void CStdPtrArray::Resize(int iSize)
{
	Empty();
	m_ppVoid = static_cast<LPVOID*>(malloc(iSize * sizeof(LPVOID)));
	::ZeroMemory(m_ppVoid, iSize * sizeof(LPVOID));
	m_nAllocated = iSize;
	m_nCount = iSize;
}

bool CStdPtrArray::IsEmpty() const
{
	return m_nCount == 0;
}

bool CStdPtrArray::Add(LPVOID pData)
{
	if( ++m_nCount >= m_nAllocated) {
		int nAllocated = m_nAllocated * 2;
		if( nAllocated == 0 ) nAllocated = 11;
		LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
		if( ppVoid != NULL ) {
			m_nAllocated = nAllocated;
			m_ppVoid = ppVoid;
		}
		else {
			--m_nCount;
			return false;
		}
	}
	m_ppVoid[m_nCount - 1] = pData;
	return true;
}

bool CStdPtrArray::InsertAt(int iIndex, LPVOID pData)
{
	if( iIndex == m_nCount ) return Add(pData);
	if( iIndex < 0 || iIndex > m_nCount ) return false;
	if( ++m_nCount >= m_nAllocated) {
		int nAllocated = m_nAllocated * 2;
		if( nAllocated == 0 ) nAllocated = 11;
		LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
		if( ppVoid != NULL ) {
			m_nAllocated = nAllocated;
			m_ppVoid = ppVoid;
		}
		else {
			--m_nCount;
			return false;
		}
	}
	memmove(&m_ppVoid[iIndex + 1], &m_ppVoid[iIndex], (m_nCount - iIndex - 1) * sizeof(LPVOID));
	m_ppVoid[iIndex] = pData;
	return true;
}

bool CStdPtrArray::SetAt(int iIndex, LPVOID pData)
{
	if( iIndex < 0 || iIndex >= m_nCount ) return false;
	m_ppVoid[iIndex] = pData;
	return true;
}

bool CStdPtrArray::Remove(int iIndex)
{
	if( iIndex < 0 || iIndex >= m_nCount ) return false;
	if( iIndex < --m_nCount ) ::CopyMemory(m_ppVoid + iIndex, m_ppVoid + iIndex + 1, (m_nCount - iIndex) * sizeof(LPVOID));
	return true;
}

int CStdPtrArray::Find(LPVOID pData) const
{
	for( int i = 0; i < m_nCount; i++ ) if( m_ppVoid[i] == pData ) return i;
	return -1;
}

int CStdPtrArray::GetSize() const
{
	return m_nCount;
}

LPVOID* CStdPtrArray::GetData()
{
	return m_ppVoid;
}

LPVOID CStdPtrArray::GetAt(int iIndex) const
{
	if( iIndex < 0 || iIndex >= m_nCount ) return NULL;
	return m_ppVoid[iIndex];
}

LPVOID CStdPtrArray::operator[] (int iIndex) const
{
	ASSERT(iIndex>=0 && iIndex<m_nCount);
	return m_ppVoid[iIndex];
}