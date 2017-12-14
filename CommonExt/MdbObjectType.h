/********************************************************************
Created:	2012.5.23  10:46
Author:		hps

Purpose:	表示 Setting.mdb 中的 objectType 表，即图层表
Remark:		
*********************************************************************/

#pragma once

#ifdef DCICOMMONEXT_MODULE
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllexport)
#else
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllimport)
#endif

#include "List.h"
using namespace Ext::Collection;

#include "AdoRow.h"
#include "AdoTable.h"
using namespace Ext::Ado;

namespace Dci
{
	namespace SettingMDB
	{

		/// <summary>
		/// 表示 Setting.mdb 中的 projectType 表
		/// </summary>
		class DCICOMMONEXT_MODULE_EXPIMP CMdbObjectType : public CAdoTable
		{
			// 字段名称常量
		public:
			static const CString MdbObjectTypeTableName;
			static const CString ID;
			static const CString LayerName;

		public:
			CMdbObjectType(void);
			virtual ~CMdbObjectType(void);

			static CMdbObjectType& Instance();

			/// <summary>
			/// 获取指定项目的所有图层信息。
			/// </summary>
			/// <remarks>输出的 CAdoRow 对象的记录有：[ID, prjId, layerName, entityType, color, linetype]</remarks>
			/// <param name="rows">[O] 输出对应项目的所有图层。每一行表示 objectType 表中的一个记录。</param>
			/// <param name="projectID">[I] 指定要查询的项目编号。</param>
			/// <returns></returns>
			/// <author name="hps" date="2012.7.13   10:45"/>
			void GetProjectLayers(List<CAdoRow*> &rows, const int& projectID);
			static void SGetProjectLayers(List<CAdoRow*> &rows, const int& projectID);

			/// <summary>
			/// 获取指定项目的所有图层名。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="layerNames">[O] 输出对应项目的所有图层名。</param>
			/// <param name="projectID">[I] 指定要查询的项目编号。</param>
			/// <returns></returns>
			/// <author name="hps" date="2012.7.13   10:46"/>
			void GetProjectLayers(List<CString> &layerNames, const int& projectID);
			static void SGetProjectLayers(List<CString> &layerNames, const int& projectID);

			CAdoRow* GetOneRow(const int& projectID, const CString &layerName);
			static CAdoRow* SGetOneRow(const int& projectID, const CString &layerName);

		private:
			virtual int OnReflectTo(); 
		};
	}
}