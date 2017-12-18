/********************************************************************
Created:	2012.5.23  10:46
Author:		hps

Purpose:	表示 Setting.mdb 中的 LayerCodeDefine 表
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
		/// 表示 Setting.mdb 中的 LayerCodeDefine 表
		/// </summary>
		class DCICOMMONEXT_MODULE_EXPIMP CMdbLayerCodeDefine : public CAdoTable
		{
			// 字段名称常量
		public:
			static const CString MdbLayerCodeDefineTableName;
			static const CString ID;

		public:
			CMdbLayerCodeDefine(void);
			virtual ~CMdbLayerCodeDefine(void);

			static CMdbLayerCodeDefine& Instance();

			/// <summary>
			/// 根据指定的项目编号，获取一组记录行。
			/// </summary>
			/// <remarks>输出的每一行记录中包含 4 个字段：[layerName、propName、propVal、code]</remarks>
			/// <param name="output">[O] 输出符合条件的记录行。</param>
			/// <param name="projectId">[I] 指定要查询的项目编号。</param>
			/// <returns>若执行成功，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.7.26   17:01"/>
			bool GetByProject(List<CAdoRow*> &output, long projectId);
			static bool SGetByProject(List<CAdoRow*> &output, long projectId);

			/// <summary>
			/// 根据指定的项目编号和CAD图层名，获取一组记录行。
			/// </summary>
			/// <remarks>输出的每一行记录中包含 3 个字段：[propName、propVal、code]</remarks>
			/// <param name="output">[O] 输出符合条件的记录行。</param>
			/// <param name="projectId">[I] 指定要查询的项目编号。</param>
			/// <param name="layerName">[I] 指定要查询的CAD图层的名称。</param>
			/// <returns>若执行成功，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.7.24   12:03"/>
			bool GetByProjectAndLayer(List<CAdoRow*> &output, long projectId, const CString &layerName);
			static bool SGetByProjectAndLayer(List<CAdoRow*> &output, long projectId, const CString &layerName);

			/// <summary>
			/// 根据指定的项目编号和属性值，获取第一个匹配记录行的 Code 列的值。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="outCode">[O] 输出 code 列的值。</param>
			/// <param name="projectId">[I] 指定要查询的项目编号。</param>
			/// <param name="propVal">[I] 指定要查询查询的属性值。</param>
			/// <returns>若执行成功，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.7.26   16:44"/>
			bool GetByProjectAndPropVal(CString &outCode, long projectId, const CString &propVal);
			static bool SGetByProjectAndPropVal(CString &outCode, long projectId, const CString &propVal);

			bool GetByCode(CAdoRow *&pOutRow, const CString &code, const CString &layernmae);
			static bool SGetByCode(CAdoRow *&pOutRow, const CString &code, const CString &layernmae);

		private:
			virtual int OnReflectTo(); 
		};

	}
}