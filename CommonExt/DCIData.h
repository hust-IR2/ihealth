#pragma once
#include "StdAfx.h"
//#include <comdef.h>
//#include <comutil.h>


#ifdef DCICOMMONEXT_MODULE
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllexport)
#else
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllimport)
#endif

namespace Ext
{
	namespace Ado
	{
		class DCICOMMONEXT_MODULE_EXPIMP CDCIData : public _variant_t
		{
		public:
			CDCIData(void);
			~CDCIData(void);

			/// <summary>
			/// 构造函数重载
			/// </summary>
			/// <remarks></remarks>
			/// <param name="bSrc"></param>
			/// <returns>CDCIData</returns>
			/// <author name="jiangl" date="2011.12.1   16:47"></author>	
			CDCIData(BYTE bSrc);
			CDCIData(bool bSrc);
			CDCIData(short sSrc);
			CDCIData(long lSrc);
			CDCIData(float fltSrc);
			CDCIData(double dblSrc);
			CDCIData(LPCTSTR pSrc);
			//CDCIData(const CString& strVal) ;
			CDCIData(int nVal);
			CDCIData(const COleDateTime& tVal);
			CDCIData(const CTime& tVal);
			CDCIData(const _variant_t& vVal);
			CDCIData(const CDCIData& vVal);

			/// <summary>
			/// 等号重载
			/// </summary>
			/// <remarks></remarks>
			/// <param name="bSrc"></param>
			/// <returns>CDCIData</returns>
			/// <author name="jiangl" date="2011.12.1   16:59"></author>

			CDCIData& operator=(BYTE bSrc);
			CDCIData& operator=(bool bSrc);
			CDCIData& operator=(short sSrc);
			CDCIData& operator=(long lSrc);
			CDCIData& operator=(float fltSrc);
			CDCIData& operator=(double dSrc);
			CDCIData& operator=(LPCTSTR pSrc);
			CDCIData& operator=(const _variant_t& vSrc);
			CDCIData& operator=(const CDCIData& vSrc);
			CDCIData& operator=(int nSrc);
			//CDCIData& operator=(const CString& strSrc);
			CDCIData& operator=(const COleDateTime& tSrc);
			CDCIData& operator=(const CTime& tSrc);

			/// <summary>
			/// 类型转化重载
			/// </summary>
			/// <remarks></remarks>
			/// <returns></returns>
			/// <author name="jiangl" date="2011.12.1   17:00"></author>
			operator BYTE()
			{ 
				return Byte_S(); 
			}

			operator bool() 
			{
				return Boolean_S();
			}

			operator short() 
			{
				return Short_S();
			}

			operator long() 
			{
				return Long_S();
			}

			operator float() 
			{
				return Float_S();
			}

			operator double() 
			{
				return Double_S();
			}

			operator LPCTSTR()
			{
				return (LPCTSTR)String_S();
			}

			operator int()
			{
				return Int_S();
			}

			operator CString() 
			{
				return String_S();
			}

			operator COleDateTime()
			{
				return OleTime_S();
			}

			operator CTime()
			{
				return Time_S();
			}

		public:

			/// <summary>
			/// 类型转换函数重载
			/// </summary>
			/// <remarks></remarks>
			/// <param name="pResult"></param>
			/// <param name="lDefault"></param>
			/// <returns></returns>
			/// <author name="jiangl" date="2011.12.1   17:13"></author>
			long Long_S(bool bResult = false, long lDefault = -1) const;
			int Int_S(bool bResult = false, int nDefault = -1) const;
			double Double_S(bool bResult = false, double dDefault = 0.0) const;
			CString String_S(bool bResult = false, LPCTSTR lpszDefault = NULL) const;
			short Short_S(bool bResult = false, short nDefault = -1) const;
			BYTE Byte_S(bool bResult = false, BYTE bDefault = 0) const;
			bool Boolean_S(bool bResult = false, bool bDefault = false) const;
			float Float_S(bool bResult = false, float fDefault = 0.0f) const;
			COleDateTime OleTime_S(bool bResult = false) const;
			CTime Time_S(bool bResult = false) const;
		};
	} // End namespace Ado
} // End namespace Ext