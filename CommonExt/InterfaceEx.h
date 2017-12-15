#pragma once

namespace Ext
{
	namespace CPP
	{
		struct Element;
		class __declspec(dllexport) InterfaceEx
		{
		public:
			enum ValueType 
			{
				NullType = 0,
				BoolType = 1,
				IntType = 2,
				LongType = 4,
				DoubleType = 8,
				StringType = 16,
				ObjectType = 32,
				ArrayType = 64,
				Uint64Type = 128
			};

		public:
			InterfaceEx();
			InterfaceEx(const InterfaceEx& val);
			InterfaceEx(const bool& val);
			InterfaceEx(const int& val);
			InterfaceEx(const UINT64& val);
			InterfaceEx(const long& val);
			InterfaceEx(const double& val);
			InterfaceEx(const TCHAR* val);
			InterfaceEx(const CString& val);

			~InterfaceEx();

		public:
			InterfaceEx& operator=(const InterfaceEx& val);	
			InterfaceEx& operator=(const bool& val);
			InterfaceEx& operator=(const int& val);
			InterfaceEx& operator=(const UINT64& val);
			InterfaceEx& operator=(const long& val);
			InterfaceEx& operator=(const double& val);
			InterfaceEx& operator=(const TCHAR* val);
			InterfaceEx& operator=(const CString& val);

		public:
			operator bool&() const;
			operator int&() const;
			operator UINT64&() const;
			operator long&() const;
			operator double&() const;
			operator CString&() const;

		public:
			bool&		GetBoolVal() const;
			int&		GetIntVal() const;
			UINT64&		GetUint64Val() const;
			long&		GetLongVal() const;
			double&		GetDoubleVal() const;
			CString&	GetStringVal() const;

			void SetBoolVal(const bool& val) const;
			void SetIntVal(const int& val) const;
			void SetUint64Val(const UINT64& val) const;
			void SetLongVal(const long& val) const;
			void SetDoubleVal(const double& val) const;
			void SetStringVal(const CString& val) const;

		public:
			void			Insert(const CString& k, const InterfaceEx& v) const;
			void			Delete(const CString& k) const;
			void			Update(const CString& k, const InterfaceEx& v) const;
			InterfaceEx&	Query(const CString& k) const;
			InterfaceEx&	operator[](const CString& k) const;
			InterfaceEx&    operator[](const wchar_t* k) const;

			Element			First() const;
			Element			Next() const;
			bool			End() const;

			void			PushBack(const InterfaceEx& v) const;
			void			PopBack() const;
			void			PushFront(const InterfaceEx& v) const;
			void			PopFront() const;
			void			Delete(const int& i) const;
			void			Insert(const int& i, const InterfaceEx& v) const;
			void			Update(const int& i, const InterfaceEx& v) const;
			InterfaceEx&	At(const int& i) const;
			InterfaceEx&	operator[](const int& i) const;
			
			bool			Empty() const;
			void			RemoveAll() const;
			int				Count() const;

		public:
			ValueType GetType() const;
			bool IsNullType();
			bool IsNumeric();

		private:
			void ClearBool() const;
			void ClearInt() const;
			void ClearUint64() const;
			void ClearLong() const;
			void ClearDouble() const;
			void ClearString() const;
			void ClearObject() const;
			void ClearArray() const;

			void Clear() const;
			
		public:
			mutable void*	   m_pData;
			mutable ValueType  m_iType;
			mutable void*	   m_pIter;
		};

		struct Element
		{
			CString k;
			InterfaceEx v;
		};

	} // End namespace CPP

} // End namesapce Ext