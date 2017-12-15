#pragma once

namespace Ext
{
	namespace CPP
	{
		struct Element;
		class __declspec(dllexport) Interface
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
			Interface();
			Interface(const Interface& val);
			Interface(const bool& val);
			Interface(const int& val);
			Interface(const UINT64& val);
			Interface(const long& val);
			Interface(const double& val);
			Interface(const TCHAR* val);
			Interface(const CString& val);

			~Interface();

		public:
			Interface& operator=(const Interface& val);	
			Interface& operator=(const bool& val);
			Interface& operator=(const int& val);
			Interface& operator=(const UINT64& val);
			Interface& operator=(const long& val);
			Interface& operator=(const double& val);
			Interface& operator=(const TCHAR* val);
			Interface& operator=(const CString& val);

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
			void			Insert(const CString& k, const Interface& v) const;
			void			Delete(const CString& k) const;
			Interface&		Find(const CString& k) const;
			Interface&		operator[](const CString& k) const;
			Interface&		operator[](const wchar_t* k) const;
			bool			Exists(const CString& k) const;

			void			Start() const;
			bool			Step() const;
			bool			KeepUp() const;
			Element			Current() const;

			void			Append(const Interface& v) const;
			void			Delete(const int& i) const;
			Interface&		At(const int& i) const;
			Interface&		operator[](const int& i) const;
			
			bool			IsEmpty() const;
			void			RemoveAll() const;
			int				Count() const;

		public:
			ValueType GetType() const;
			bool IsNullType() const;
			bool IsNumeric() const;

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
			Interface v;
		};
	} // End namespace CPP

} // End namesapce Ext