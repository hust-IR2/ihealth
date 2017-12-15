//********************************************************************
//**	Created:	2011.11.28  14:59
//**	Author:		luzj
//**	
//**	Purpose:	字符串查询操作相关扩展类
//**	Remark:		
//********************************************************************
#pragma once

#ifdef DCICOMMONEXT_MODULE
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllexport)
#else
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllimport)
#endif

//#include <vector>
#include "List.h"
using namespace Ext::Collection;

namespace Ext
{

	class DCICOMMONEXT_MODULE_EXPIMP CExString
	{
	public:
		/// <summary>
		/// 根据指定分隔符拆分字符串到数组
		/// </summary>
		/// <remarks></remarks>
		/// <param name="aryResult">输出拆分后得到的字符串数组</param>
		/// <param name="strOrign">输入原始字符串</param>
		/// <param name="strSplitSymbol">输入特殊字符串(用于作为拆分标志的字符串)</param>
		/// <returns>返回输出数组的长度</returns>
		/// <author name="Luzj" date="2011.11.28  13:34"/>
		static int SplitStrBySymbol(List<CString> &aryResult,CString strOrign,CString strSplitSymbol=_T(";"));
		//static int SplitStrBySymbol(CStringArray &aryResult,CString strOrign,CString strSplitSymbol=_T(";"));
		//static int SplitStrBySymbol(std::vector<CString> &aryResult,CString strOrign,CString strSplitSymbol=_T(";"));

		/// <summary>
		/// 判断一个字符串数组中是否存在一个字符串(等长的)
		/// </summary>
		/// <remarks></remarks>
		/// <param name="arySource">输入字符串数组</param>
		/// <param name="strFind">输入要查找的字符串</param>
		/// <param name="bMatchCase">输入是否区分大小写,TRUE区分,FALSE不区分</param>
		/// <param name="nFindStart">输入起始查询位置</param>
		/// <returns>如果存在,返回所在位置(0表示第一位),不存在返回-1</returns>
		/// <author name="Luzj" date="2011.11.28  13:47"/>
		static int FindStrInArray(const CStringArray &arySource,CString strFind,BOOL bMatchCase=TRUE,int nFindStart=0);
		static int FindStrInArray(const List<CString> &arySource,CString strFind,BOOL bMatchCase=TRUE,int nFindStart=0);

		/// <summary>
		/// 去除字符串数组中重复的字符串
		/// </summary>
		/// <remarks></remarks>
		/// <param name="arySource">输入输出要处理的字符串</param>
		/// <param name="bCompareCase">输入是否考虑大小写, TRUE区分大小写(缺省), FALSE不区分</param>
		/// <returns>返回处理后的数值长度</returns>
		/// <author name="Luzj" date="2011.11.28  14:30"/>
		//static int RemoveSameStrInArray(CStringArray &arySource,BOOL bCompareCase=TRUE);
		static int RemoveSameStrInArray(List<CString> &arySource,BOOL bCompareCase=TRUE);

		/// <summary>
		/// 将浮点数变成字符串,并去掉浮点数末尾多余的0
		/// </summary>
		/// <remarks></remarks>
		/// <param name="dFormatStr">输入需要转换的double值</param>
		/// <returns>返回对应的字符串</returns>
		/// <author name="Luzj" date="2011.11.28  14:49"/>
		static CString FormatDouble(double dFormatStr);

		static CString FormatString(CString strText,int nFormat=6);		

		/// <summary>
		/// 获取浮点数的整数部分。
		/// </summary>
		/// <remarks></remarks>
		/// <param name="str">[I] 输入含有浮点数的字符串。</param>
		/// <returns>返回表示浮点数整数部分的字符串。</returns>
		/// <author name="peishengh" date="2011.11.30   16:13"/>
		static CString GetInteger(const CString &str);

		/// <summary>
		/// 将 Unicode 字符转换为 ASCII 字符。
		/// </summary>
		/// <remarks>调用者负责释放返回值的资源。</remarks>
		/// <param name="tchStr">[I] 输入要转换的字符系列。</param>
		/// <returns>返回对应的 ASCII 字符系列。</returns>
		/// <author name="hps" date="2012.4.10   15:55"/>
		static char* TCHAR2char(const TCHAR* tchStr);

		/// <summary>
		/// 将 ASCII 字符转换为 Unicode 字符。
		/// </summary>
		/// <remarks>调用者负责释放返回值的资源。</remarks>
		/// <param name="charStr">[I] 输入要转换的字符系列。</param>
		/// <returns>返回对应的 Unicode 字符系列。</returns>
		/// <author name="hps" date="2012.4.11   11:35"/>
		static TCHAR* char2TCHAR(const char* charStr);

		static CString C2W(const char* chr);
		static std::string  W2C(const wchar_t* wchr);

		/*****************************************************************************
		* 将一个字符的Unicode(UCS-2和UCS-4)编码转换成UTF-8编码.
		*
		* 参数:
		*    unic     字符的Unicode编码值
		*    pOutput  指向输出的用于存储UTF8编码值的缓冲区的指针
		*    outsize  pOutput缓冲的大小
		*
		* 返回值:
		*    返回转换后的字符的UTF8编码所占的字节数, 如果出错则返回 0 .
		*
		* 注意:
		*     1. UTF8没有字节序问题, 但是Unicode有字节序要求;
		*        字节序分为大端(Big Endian)和小端(Little Endian)两种;
		*        在Intel处理器中采用小端法表示, 在此采用小端法表示. (低地址存低位)
		*     2. 请保证 pOutput 缓冲区有最少有 6 字节的空间大小!
		****************************************************************************/
		static int	  OneUnicoide2UTF8(unsigned long unic, unsigned char *pOutput);

		/*****************************************************************************
		* 将一个字符的UTF8编码转换成Unicode(UCS-2和UCS-4)编码.
		*
		* 参数:
		*    pInput      指向输入缓冲区, 以UTF-8编码
		*	  utfbytes    utf8字符的位数
		*    Unic        指向输出缓冲区, 其保存的数据即是Unicode编码值,
		*                类型为unsigned long .
		*
		* 返回值:
		*    成功则返回该字符的UTF8编码所占用的字节数; 失败则返回0.
		*
		* 注意:
		*     1. UTF8没有字节序问题, 但是Unicode有字节序要求;
		*        字节序分为大端(Big Endian)和小端(Little Endian)两种;
		*        在Intel处理器中采用小端法表示, 在此采用小端法表示. (低地址存低位)
		****************************************************************************/
		static int	  OneUTF82Unicode(const unsigned char *pInput, int utfbytes, unsigned long *Unic);

		/// <summary>
		/// 将 Unicode 字符转换为 UTF8 字符。
		/// </summary>
		/// <remarks>调用者负责申请内存空间。</remarks>
		/// <param name="pInput">[I] 输入要转换的字符序列。</param>
		/// <param name="inLength">[I] 指示 pInput 的长度。</param>
		/// <param name="pOutPut">[O] 输出转换后的字符数组。</param>
		/// <param name="outLength">[O] 指示 pOutPut 的长度。</param>
		/// <returns>返回实际转换的字符个数</returns>
		/// <author name="quanl" date="2013.6.27   11:35"/>
		static size_t Unicode2Utf8(const wchar_t *pInput, size_t inLength, char *pOutPut, size_t outLength);

		/// <summary>
		/// 将 UTF8 字符转换为 Unicode 字符。
		/// </summary>
		/// <remarks>调用者负责申请内存空间。</remarks>
		/// <param name="pInput">[I] 输入要转换的字符序列。</param>
		/// <param name="inLength">[I] 指示 pInput 的长度。</param>
		/// <param name="pOutPut">[O] 输出转换后的字符数组。</param>
		/// <param name="outLength">[O] 指示 out 的长度。</param>
		/// <returns>返回实际转换的字符个数</returns>
		/// <author name="quanl" date="2013.6.27   11:35"/>
		static size_t Utf82Unicode(char *pInput, size_t insize, wchar_t *pOutput,  size_t outsize);

		//如果一个字符串是由字符与数字两部分组成的，把该字符串拆分成字符和数字两部分
		//参考说明  : 输入参数包含的字符可以为: 字符＋数字 或 数字＋字符串
		static BOOL SplitStrInTwoPart(CString &strStr,	//输出参数:拆分后的字符串部分
			CString &strDigit,							//输出参数:拆分后的数字部分
			CString strOrign);							//输入参数:原始字符串

		//检查一个字符串是否是数字串
		//参考说明	: 该字符串如果是小数，科学技术法的形式都认为是数字
		static BOOL IsDigit(CString strStr);		//输入参数:源字符串				

	private:
		CExString(void){};
		~CExString(void){};
	};

} // End namespace Ext