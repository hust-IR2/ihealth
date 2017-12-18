/********************************************************************
Created:	2012.5.23  10:46
Author:		hps

Purpose:	表示 Setting.mdb 中的 params 表
Remark:		
*********************************************************************/

#pragma once

#ifdef DCICOMMONEXT_MODULE
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllexport)
#else
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllimport)
#endif

#include "AdoTable.h"
using namespace Ext::Ado;

namespace Dci
{
	namespace SettingMDB
	{

		/// <summary>
		/// 表示 Setting.mdb 中的 params 表
		/// </summary>
		class DCICOMMONEXT_MODULE_EXPIMP CMdbParams : public CAdoTable
		{
			// 字段名称常量
		public:
			static const CString MdbParamsTableName;
			static const CString ID;

		public:
			CMdbParams(void);
			virtual ~CMdbParams(void);

			static CMdbParams& Instance();

			/// <summary>
			/// 获取指定参数的参数值。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="strParamName">[I] 指定要检索的参数名。</param>
			/// <returns>返回对应的参数值。若指定的参数名不存在，则返回空字符串。</returns>
			/// <author name="hps" date="2012.7.13   10:39"/>
			CString GetValue(const CString &strParamName);
			static CString SGetValue(const CString &strParamName);

			/// <summary>
			/// 获取指定参数的参数值。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="output">[O] 输出对应的参数值。若指定的参数名不存在，output 保持不变。</param>
			/// <param name="strParamName">[I] 指定要检索的参数名。</param>
			/// <returns>若参数名存在并成功获取参数值，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.7.13   10:40"/>
			bool GetValue(CString &output, const CString &strParamName);
			static bool SGetValue(CString &output, const CString &strParamName);

			/// <summary>
			/// 设置指定参数名的参数值。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="strParamName">[I] 指定要修改的参数名。</param>
			/// <param name="strParamValue">[I] 指定参数的新值。</param>
			/// <returns>若参数名存在并修改成功，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.7.13   10:42"/>
			bool SetValue(const CString &strParamName, const CString &strParamValue);
			static bool SSetValue(const CString &strParamName, const CString &strParamValue);

		private:
			virtual int OnReflectTo(); 
		};

	}
}