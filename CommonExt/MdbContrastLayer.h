/********************************************************************
Created:	2012.5.23  10:46
Author:		hps

Purpose:	表示 Setting.mdb 中的 contrastLayer 表
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
		/// 表示 Setting.mdb 中的 contrastLayer 表
		/// </summary>
		class DCICOMMONEXT_MODULE_EXPIMP CMdbContrastLayer : public CAdoTable
		{
			// 字段名称常量
		public:
			static const CString MdbContrastLayerTableName;

		public:
			CMdbContrastLayer(void);
			virtual ~CMdbContrastLayer(void);

			static CMdbContrastLayer& Instance();


			/// <summary>
			/// 获取指定项目中的入库时所有对照的 SDE 图层信息。
			/// </summary>
			/// <remarks>输出的每一行记录中包含两个字段：[sdeLayer、entityType]</remarks>
			/// <param name="output">[O] 输出记录行。</param>
			/// <param name="projectId">[I] 指定项目编号（即 projectType 中的 ID 字段的值）</param>
			/// <returns>若执行成功，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.7.19   15:35"/>
			bool GetAllSdeLayers(List<CAdoRow*> &output, long projectId);
			static bool SGetAllSdeLayers(List<CAdoRow*> &output, long projectId);

			/// <summary>
			/// 根据项目编号和指定的sde图层名，获取cad的对照信息。
			/// </summary>
			/// <remarks>输出的每一行记录中包含 3 个字段：[layerName、propName、propVal]</remarks>
			/// <param name="output">[O] 输出记录行。</param>
			/// <param name="projectId">[I] 指定项目编号（即 projectType 中的 ID 字段的值）</param>
			/// <param name="strSdeLayer">[I] 指定要查询的SDE图层的名称。</param>
			/// <returns>若执行成功，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.7.20   10:37"/>
			bool GetContrastCadInfoForSdeLayer(List<CAdoRow*> &output, long projectId, const CString &strSdeLayer);
			static bool SGetContrastCadInfoForSdeLayer(List<CAdoRow*> &output, long projectId, const CString &strSdeLayer);

			
			/// <summary>
			/// 根据 SDE 图层名称获取项目类型编号
			/// </summary>
			/// <remarks>当且仅当 contrastLayer 表中 sdeLayer 字段值唯一的时候可用！！！</remarks>
			/// <param name="outputProjectId">[O] 输出项目编号（即 projectType 中的 ID 字段的值）</param>
			/// <param name="strSdeLayer">[I] 指定要查询的SDE图层的名称。</param>
			/// <returns>若执行成功，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2013.2.20   13:47"/>
			bool GetProjectIdBySdeLayer(long &outputProjectId, const CString &strSdeLayer);
			static bool SGetGetProjectIdBySdeLayer(long &outputProjectId, const CString &strSdeLayer);
			
		private:
			virtual int OnReflectTo(); 
		};

	}
}