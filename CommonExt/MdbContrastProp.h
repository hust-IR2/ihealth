/********************************************************************
Created:	2012.5.23  10:46
Author:		hps

Purpose:	表示 Setting.mdb 中的 contrastProp 表
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
		/// 表示 Setting.mdb 中的 contrastProp 表
		/// </summary>
		class DCICOMMONEXT_MODULE_EXPIMP CMdbContrastProp : public CAdoTable
		{
			// 字段名称常量
		public:
			static const CString MdbContrastPropTableName;

		public:
			CMdbContrastProp(void);
			virtual ~CMdbContrastProp(void);

			static CMdbContrastProp& Instance();

			/// <summary>
			/// 根据 contrastLayer 表中指定的 sdeLayer 列的值和 contrastProp 表中 specialVal 列的值，获取 contrastProp 表中 sdeProp 列对应的值。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="output">[O] 输出属性对照表 contrastProp 中 sdeProp 列对应的值。</param>
			/// <param name="strSdeLayer">[I] 指定要查询的 sde 图层名。</param>
			/// <param name="strSpecialValue">[I] 指定 contrastProp 表中 specialVal 列的值。</param>
			/// <returns>若执行成功，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.7.19   16:20"/>
			bool GetSdePropertyBySpecialValue(CString &output, const CString &strSdeLayer, const CString &strSpecialValue);
			static bool SGetSdePropertyBySpecialValue(CString &output, const CString &strSdeLayer, const CString &strSpecialValue);

			/// <summary>
			/// 获取指定的 sde 图层的所有字段信息。
			/// </summary>
			/// <remarks>输出的每一行记录中包含 3 个字段：[sdeProp、dataType、length]</remarks>
			/// <param name="output">[O] 输出记录行。</param>
			/// <param name="projectID">[I] 指定 sde 图层所在的项目编号。</param>
			/// <param name="strSdeLayer">[I] 指定要查询的 sde 图层名称。</param>
			/// <returns>若执行成功，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.7.19   16:25"/>
			bool GetAllSdeProperty(List<CAdoRow*> &output, long projectID, const CString &strSdeLayer);
			static bool SGetAllSdeProperty(List<CAdoRow*> &output, long projectID, const CString &strSdeLayer);

			
			/// <summary>
			/// 获取指定的 sde 图层中的所有特殊字段信息。
			/// </summary>
			/// <remarks>输出的每一行记录中包含 3 个字段：[sdeProp、dataType、length]</remarks>
			/// <param name="output">[O] 输出记录行。</param>
			/// <param name="projectID">[I] 指定 sde 图层所在的项目编号。</param>
			/// <param name="strSdeLayer">[I] 指定要查询的 sde 图层名称。</param>
			/// <returns>若执行成功，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.8.15   10:11"/>
			bool GetAllSpecialSdeProperty(List<CAdoRow*> &output, long projectID, const CString &strSdeLayer);
			static bool SGetAllSpecialSdeProperty(List<CAdoRow*> &output, long projectID, const CString &strSdeLayer);

			/// <summary>
			/// 获取指定的 sde 图层属性对照信息。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="output">输出的每一行记录中包含 3 个字段：[contrastProp.sdeProp、contrastProp.cadProp、contrastProp.specialVal]</param>
			/// <param name="projectID"></param>
			/// <param name="strSdeLayer"></param>
			/// <returns></returns>
			/// <author name="hps" date="2012.7.23   10:43"/>
			bool GetContrastCadInfoForSdeLayer(List<CAdoRow*> &output, long projectID, const CString &strSdeLayer);
			static bool SGetContrastCadInfoForSdeLayer(List<CAdoRow*> &output, long projectID, const CString &strSdeLayer);
			
		private:
			virtual int OnReflectTo(); 
		};

	}
}