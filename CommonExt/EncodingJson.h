#pragma once

#include "Interface.h"

namespace Ext
{
	namespace CPP
	{
		class __declspec(dllexport) CEncodingJson
		{
			enum JsonValueType {
				Int64Type = 0,
				DoubleType,
				StringType,
				ArrayType,
				ObjectType,
				BoolType
			};

		public:
			CEncodingJson(void);
			~CEncodingJson(void);

		public:
			static CString		Marshal(const Interface& value);
			static Interface	Unmarshal(const CString& jsonValue);

		protected:
			CString		encode(const Interface& value);
			void		decode(const TCHAR* pszJsonValue, Interface& o);

		private:
			int			getKVCount(const TCHAR* pszJsonValue);
			CString		getKAt(const TCHAR* pszJsonValue, int iIndex);
			CString		getVAt(const TCHAR* pszJsonValue, int iIndex);

			int			getArrayCount(const TCHAR* pszJsonValue);
			CString		getArrayAt(const TCHAR* pszJsonValue, int iIndex);

			int			decodeVType(const TCHAR* pszValue);

			Interface	decodeBool(const TCHAR* pszJsonValue);
			Interface	decodeInt64(const TCHAR* pszJsonValue);
			Interface	decodeDouble(const TCHAR* pszJsonValue);
			Interface	decodeCString(const TCHAR* pszJsonValue);
			void	decodeArray(const TCHAR* pszJsonValue, Interface& v);
			void    decodeObject(const TCHAR* pszJsonValue, Interface& v);
			
		private:
			CString		encodeBool(const Interface& value);
			CString		encodeInt64(const Interface& value);
			CString		encodeDouble(const Interface& value);
			CString		encodeCString(const Interface& value);
			CString		encodeArray(const Interface& value);
			CString		encodeObject(const Interface& value);

		};

	} // End namespace CPP

} // End namesapce Ext