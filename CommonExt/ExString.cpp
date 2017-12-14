#include "StdAfx.h"
#include "ExString.h"

using namespace Ext;

int CExString::SplitStrBySymbol(List<CString> &aryResult,CString strOrign,CString strSplitSymbol)
{
	//aryResult.RemoveAll();
	aryResult.Clear();

	//消除输入字符串参数首尾的空格
	CString strSymbol = strSplitSymbol;
	CString strOrigin = strOrign;
	strOrigin.TrimLeft();
	strOrigin.TrimRight();
	int nLenSymbol = strSymbol.GetLength();
	int nLenOrigin = strOrigin.GetLength();

	//检查输入参数是否有效
	if (nLenSymbol == 0  || nLenOrigin == 0)
		return aryResult.Count();
		//return aryResult.GetSize();

	//拆分字符串
	while(TRUE)
	{
		CString strGetSeg = _T("");
		int nIndex = strOrigin.Find(strSymbol);
		if(nIndex == -1)
		{
			strGetSeg = strOrigin;
			strOrigin = _T("");
		}
		else
		{
			strGetSeg = strOrigin.Mid(0, nIndex);
			strOrigin = strOrigin.Mid(nIndex+nLenSymbol);
		}

		strGetSeg.TrimLeft();
		strGetSeg.TrimRight();

		//aryResult.Add(strGetSeg);
		aryResult.Append(strGetSeg);

		if(nIndex == -1)
			break;

		if(strOrigin.IsEmpty())
			break;
	}

	//return aryResult.GetSize();
	return aryResult.Count();
}

//int CExString::SplitStrBySymbol(std::vector<CString> &aryResult,CString strOrign,CString strSplitSymbol/*=_T(";")*/)
//{
//	CStringArray temp;
//	int result = SplitStrBySymbol(temp, strOrign, strSplitSymbol);
//
//	int size = temp.GetCount();
//	for (int i = 0; i < size; ++i)
//		aryResult.push_back(temp[i]);
//
//	return result;
//}

//原函数,暂时备份在此,待确认无错可以删除
//bool CCommonUtils::SplitString(CString& str, CStringArray &StrArr, TCHAR ch)
//{
//	if(str.IsEmpty())
//		return false;
//
//	CString str1;
//	int start=0;
//	for(int n = str.Find(ch,start); n != -1; n = str.Find(ch,start))
//	{
//		str1 = str.Mid(start,n-start);
//		StrArr.Add(str1);
//		start = n+1;
//	}
//	str1 = str.Mid(start);
//	StrArr.Add(str1);
//	return true;
//}

int CExString::FindStrInArray(const CStringArray &arySource,CString strFind, BOOL bMatchCase, int nFindStart)	
{
	CString sFind = strFind;

	//检查参数有效性
	if(arySource.GetSize() == 0 )
		return -1;

	int i = 0;

	//查找字符串
	if(bMatchCase)//区分大小写
	{
		for(i=nFindStart; i<arySource.GetSize(); i++)
		{
			if(arySource[i] == sFind)
				return i;
		}
		return -1;
	}
	else//不区分大小写
	{
		sFind.MakeUpper();
		for(i=nFindStart ; i<arySource.GetSize(); i++)
		{
			CString str;
			str = arySource[i];
			str.MakeUpper();
			if(str == sFind)
				return i;
		}
		return -1;
	}

	return -1;
}


int CExString::FindStrInArray(const List<CString> &arySource,CString strFind, BOOL bMatchCase, int nFindStart)	
{
	CString sFind = strFind;

	//检查参数有效性
	if(arySource.Count() == 0 )
		return -1;

	int i = 0;

	//查找字符串
	if(bMatchCase)//区分大小写
	{
		for(i=nFindStart; i<arySource.Count(); i++)
		{
			if(arySource[i] == sFind)
				return i;
		}
		return -1;
	}
	else//不区分大小写
	{
		sFind.MakeUpper();
		for(i=nFindStart ; i<arySource.Count(); i++)
		{
			CString str;
			str = arySource[i];
			str.MakeUpper();
			if(str == sFind)
				return i;
		}
		return -1;
	}

	return -1;
}

int CExString::RemoveSameStrInArray(List<CString> &arySource, BOOL bCompareCase)
{
	//CStringArray aryNew;
	List<CString> aryNew;

	for(int i=0; i<arySource.Count(); i++)
	{
		CString strOld = arySource[i];

		int nIndex = FindStrInArray(aryNew,strOld,bCompareCase);

		if(nIndex >= 0)
			continue;

		aryNew.Append(arySource[i]);
	}

	//arySource.RemoveAll();
	arySource.Clear();
	arySource.Append(aryNew);
	return arySource.Count();
}

CString CExString::FormatDouble(double dFormatStr)			
{
	CString strFormat = _T("");
	strFormat.Format(_T("%f"),dFormatStr);
	if (strFormat.Find(_T("."), 0) > 0)
	{
		strFormat.TrimRight(_T("0"));
		strFormat.TrimRight(_T("."));
	}

	return strFormat;
}

CString CExString::FormatString(CString strText,int nFormat)			
{
	CString str = strText;

	int nPos = str.Find(_T("."));
	if(nPos > 0)
	{
		str.TrimRight(_T("0"));
		str.TrimRight(_T("."));

		if(nPos + nFormat < str.GetLength()-1)
			str = str.Left(nPos+nFormat+1);
	}
	return str;
}


CString CExString::GetInteger(const CString &str)
{
	CString result = str;

	if (result.IsEmpty())
		return result;

	int len = result.GetLength();
	if (len == 1)
		return result;

	if (result.Find(_T(".")) == -1)
		return result;//找不到小数点

	for (int i = len - 1; i >= 0; i--)
	{
		if (result[i] != '0' && result[i] != '.')
			break;

		if (result[i] == '.')
		{
			result = result.Left(i);
			break;
		}
		else
			result = result.Left(i);
	}

	return result;
}

char* CExString::TCHAR2char(const TCHAR* tchStr)
{
	char* pDest = NULL;

#ifdef _UNICODE 
	int iSize = WideCharToMultiByte(CP_ACP, 0, tchStr, -1, NULL, 0, NULL, NULL);
	pDest = (char*)malloc(sizeof(char)*(iSize + 1));
	WideCharToMultiByte(CP_ACP, 0, tchStr, -1, pDest, iSize, NULL, NULL);
#else
	pDest = (char*)malloc(strlen(tchStr) + 1); 
	strcpy(pDest, tchStr);
#endif

	return pDest;
}

TCHAR* CExString::char2TCHAR(const char* charStr)
{
	TCHAR *pDest = NULL;

#ifdef _UNICODE 
	int iSize = MultiByteToWideChar(CP_ACP, 0, charStr, -1, NULL, 0);
	pDest = (TCHAR*)malloc(sizeof(TCHAR)*(iSize + 1));
	MultiByteToWideChar(CP_ACP, 0, charStr, -1, pDest, iSize);
#else
	pDest = (char*)malloc(strlen(charStr) + 1); 
	strcpy(pDest, charStr);
#endif
	return pDest;
}

CString CExString::C2W(const char* chr)
{
	TCHAR *Dst = CExString::char2TCHAR(chr);
	CString Rst = Dst;
	free(Dst);

	return Rst;
}

std::string CExString::W2C(const wchar_t* wchr)
{
	char *Dst = CExString::TCHAR2char((LPCTSTR)wchr);
	std::string Rst = Dst;
	free(Dst);

	return Rst;
}

int CExString::OneUnicoide2UTF8(unsigned long unic, unsigned char *pOutput)
{
	if(pOutput == NULL) {return 0;}

	if ( unic <= 0x0000007F )
	{
		// * U-00000000 - U-0000007F:  0xxxxxxx
		*pOutput     = (unic & 0x7F);
		return 1;
	}
	else if ( unic >= 0x00000080 && unic <= 0x000007FF )
	{
		// * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx
		*(pOutput+1) = (unic & 0x3F) | 0x80;
		*pOutput     = ((unic >> 6) & 0x1F) | 0xC0;
		return 2;
	}
	else if ( unic >= 0x00000800 && unic <= 0x0000FFFF )
	{
		// * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx
		*(pOutput+2) = (unic & 0x3F) | 0x80;
		*(pOutput+1) = ((unic >>  6) & 0x3F) | 0x80;
		*pOutput     = ((unic >> 12) & 0x0F) | 0xE0;
		return 3;
	}
	else if ( unic >= 0x00010000 && unic <= 0x001FFFFF )
	{
		// * U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		*(pOutput+3) = (unic & 0x3F) | 0x80;
		*(pOutput+2) = ((unic >>  6) & 0x3F) | 0x80;
		*(pOutput+1) = ((unic >> 12) & 0x3F) | 0x80;
		*pOutput     = ((unic >> 18) & 0x07) | 0xF0;
		return 4;
	}
	else if ( unic >= 0x00200000 && unic <= 0x03FFFFFF )
	{
		// * U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		*(pOutput+4) = (unic & 0x3F) | 0x80;
		*(pOutput+3) = ((unic >>  6) & 0x3F) | 0x80;
		*(pOutput+2) = ((unic >> 12) & 0x3F) | 0x80;
		*(pOutput+1) = ((unic >> 18) & 0x3F) | 0x80;
		*pOutput     = ((unic >> 24) & 0x03) | 0xF8;
		return 5;
	}
	else if ( unic >= 0x04000000 && unic <= 0x7FFFFFFF )
	{
		// * U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		*(pOutput+5) = (unic & 0x3F) | 0x80;
		*(pOutput+4) = ((unic >>  6) & 0x3F) | 0x80;
		*(pOutput+3) = ((unic >> 12) & 0x3F) | 0x80;
		*(pOutput+2) = ((unic >> 18) & 0x3F) | 0x80;
		*(pOutput+1) = ((unic >> 24) & 0x3F) | 0x80;
		*pOutput     = ((unic >> 30) & 0x01) | 0xFC;
		return 6;
	}

	return 0;
}

int CExString::OneUTF82Unicode(const unsigned char *pInput, int utfbytes, unsigned long *Unic)
{
	assert(pInput != NULL && Unic != NULL);

	// b1 表示UTF-8编码的pInput中的高字节, b2 表示次高字节, ...
	char b1, b2, b3, b4, b5, b6;

	*Unic = 0x0; // 把 *Unic 初始化为全零
	//int utfbytes = enc_get_utf8_size(*pInput);
	unsigned char *pOutput = (unsigned char *) Unic;

	switch ( utfbytes )
	{
	case 0:
		*pOutput     = *pInput;
		utfbytes    += 1;
		break;
	case 2:
		b1 = *pInput;
		b2 = *(pInput + 1);
		if ( (b2 & 0xE0) != 0x80 )
			return 0;
		*pOutput     = (b1 << 6) + (b2 & 0x3F);
		*(pOutput+1) = (b1 >> 2) & 0x07;
		break;
	case 3:
		b1 = *pInput;
		b2 = *(pInput + 1);
		b3 = *(pInput + 2);
		if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) )
			return 0;
		*pOutput     = (b2 << 6) + (b3 & 0x3F);
		*(pOutput+1) = (b1 << 4) + ((b2 >> 2) & 0x0F);
		break;
	case 4:
		b1 = *pInput;
		b2 = *(pInput + 1);
		b3 = *(pInput + 2);
		b4 = *(pInput + 3);
		if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)
			|| ((b4 & 0xC0) != 0x80) )
			return 0;
		*pOutput     = (b3 << 6) + (b4 & 0x3F);
		*(pOutput+1) = (b2 << 4) + ((b3 >> 2) & 0x0F);
		*(pOutput+2) = ((b1 << 2) & 0x1C)  + ((b2 >> 4) & 0x03);
		break;
	case 5:
		b1 = *pInput;
		b2 = *(pInput + 1);
		b3 = *(pInput + 2);
		b4 = *(pInput + 3);
		b5 = *(pInput + 4);
		if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)
			|| ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80) )
			return 0;
		*pOutput     = (b4 << 6) + (b5 & 0x3F);
		*(pOutput+1) = (b3 << 4) + ((b4 >> 2) & 0x0F);
		*(pOutput+2) = (b2 << 2) + ((b3 >> 4) & 0x03);
		*(pOutput+3) = (b1 << 6);
		break;
	case 6:
		b1 = *pInput;
		b2 = *(pInput + 1);
		b3 = *(pInput + 2);
		b4 = *(pInput + 3);
		b5 = *(pInput + 4);
		b6 = *(pInput + 5);
		if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)
			|| ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80)
			|| ((b6 & 0xC0) != 0x80) )
			return 0;
		*pOutput     = (b5 << 6) + (b6 & 0x3F);
		*(pOutput+1) = (b5 << 4) + ((b6 >> 2) & 0x0F);
		*(pOutput+2) = (b3 << 2) + ((b4 >> 4) & 0x03);
		*(pOutput+3) = ((b1 << 6) & 0x40) + (b2 & 0x3F);
		break;
	default:
		return 0;
		break;
	}

	return utfbytes;
}

size_t CExString::Unicode2Utf8(const wchar_t *pInput, size_t inLength, char *pOutPut, size_t outLength )
{
	if(inLength == 0)
		inLength = wcslen(pInput);
	return WideCharToMultiByte(CP_UTF8, 0, pInput, static_cast<int>(inLength),
		(char*)pOutPut, static_cast<int>(outLength), NULL, NULL);
}

size_t CExString::Utf82Unicode(char* pInput, size_t insize, wchar_t * pOutput, size_t outsize)
{
	if(insize == 0)
		insize = strlen((const char*)pInput);
	return MultiByteToWideChar(CP_UTF8, 0, (const char*)pInput, static_cast<int>(insize),
		pOutput, static_cast<int>(outsize));

}

BOOL CExString::SplitStrInTwoPart(CString &strStr,							//输出参数:拆分后的字符串部分
								CString &strDigit,						//输出参数:拆分后的数字部分
								CString strOrign)						//输入参数:原始字符串

{
	//检查原始字符串参数
	CString sTmpStr = strOrign;
	sTmpStr.TrimLeft();
	sTmpStr.TrimRight();
	int nLenTmpStr = sTmpStr.GetLength();
	if(nLenTmpStr == 0)
		return FALSE;

	//计算拆分字符串
	int nDigitLeft = -1;
	int nDigitRight= -1;
	for(int nIndex = 0; nIndex < nLenTmpStr; nIndex++)
	{
		CString sLeft = sTmpStr.Mid(0, nIndex);
		CString sRight= sTmpStr.Mid(nIndex);
		BOOL bIsDigitLeft = bIsDigitLeft = CExString::IsDigit(sLeft);
		BOOL bIsDigitRight= FALSE;

		if(nDigitRight == -1)
			bIsDigitRight= CExString::IsDigit(sRight);
		if(bIsDigitLeft)
			nDigitLeft = nIndex;
		if(bIsDigitRight)
			nDigitRight = nIndex;
		//
		if(nDigitRight >= 0)
			break;
	}

	// 提取数字/字符串
	if(nDigitLeft > 0)
	{
		strDigit = sTmpStr.Mid(0, nDigitLeft);
		strStr = sTmpStr.Mid(nDigitLeft);
		return TRUE;
	}
	else if(nDigitRight >=0)
	{
		strDigit = sTmpStr.Mid(nDigitRight);
		strStr = sTmpStr.Mid(0, nDigitRight);
		return TRUE;
	}
	else
	{
		strDigit = _T("");
		strStr = sTmpStr;
		return FALSE;
	}
	return FALSE;
}

BOOL CExString::IsDigit(CString strStr)
{
	int nCount = strStr.GetLength();
	if (nCount == 1)
	{
		if (strStr >= _T("0") && strStr <= _T("9"))
		{
			return TRUE;
		}
		return FALSE;
	}


	CString sMun = _T("");
	for (int i = 0; i< nCount; i++)
	{
		CString s(strStr.GetAt(i));
		if ((s >= _T("0") && s <= _T("9")) || s == _T("."))
		{
			sMun = sMun + s;
		}
		else
		{
			break;
		}
	}

	if (sMun.GetLength() < strStr.GetLength())
	{
		return FALSE;
	}

	return TRUE;
}
