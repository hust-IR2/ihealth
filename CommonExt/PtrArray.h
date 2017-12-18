
#pragma once

#ifdef DCICOMMONEXT_MODULE
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllexport)
#else
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllimport)
#endif

namespace Ext
{
	class DCICOMMONEXT_MODULE_EXPIMP CStdPtrArray
	{
	public:
		CStdPtrArray(int iPreallocSize = 0);
		CStdPtrArray(const CStdPtrArray& src);
		~CStdPtrArray();

		void Empty();
		void Resize(int iSize);
		bool IsEmpty() const;
		int Find(LPVOID iIndex) const;
		bool Add(LPVOID pData);
		bool SetAt(int iIndex, LPVOID pData);
		bool InsertAt(int iIndex, LPVOID pData);
		bool Remove(int iIndex);
		int GetSize() const;
		LPVOID* GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPVOID* m_ppVoid;
		int m_nCount;
		int m_nAllocated;
	};
} // End namespace Ext