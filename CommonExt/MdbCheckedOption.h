/********************************************************************
Created:	2012.5.23  10:46
Author:		hps

Purpose:	表示 Setting.mdb 中的 CheckedOption 表
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
		/// 表示 Setting.mdb 中的 CheckedOption 表
		/// </summary>
		class DCICOMMONEXT_MODULE_EXPIMP CMdbCheckedOption : public CAdoTable
		{
			// 字段名称常量
		public:
			static const CString MdbCheckedOptionTableName;

		public:
			CMdbCheckedOption(void);
			virtual ~CMdbCheckedOption(void);

			static CMdbCheckedOption& Instance();

			/// <summary>
			/// 获取“空值检测”配置
			/// </summary>
			/// <remarks>输出的每一行记录中包含全部字段：[ID、ProjectID、LayerName、PropertyName、CheckType、Expected、ErrorMessage、Summary]</remarks>
			/// <param name="outOptions">[O] 输出记录行。</param>
			/// <param name="projectID">[I] 指定要查询的项目编号。</param>
			/// <returns>若执行成功，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.7.30   15:02"/>
			bool GetCheckNullOptions(List<CAdoRow*> &outOptions, long projectID);
			static bool SGetCheckNullOptions(List<CAdoRow*> &outOptions, long projectID);

			/// <summary>
			/// 获取“重复性”配置
			/// </summary>
			/// <remarks>输出的每一行记录中包含全部字段：[ID、ProjectID、LayerName、PropertyName、CheckType、Expected、ErrorMessage、Summary]</remarks>
			/// <param name="outOptions">[O] 输出记录行。</param>
			/// <param name="projectID">[I] 指定要查询的项目编号。</param>
			/// <returns>若执行成功，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.7.30   15:05"/>
			bool GetCheckRepeatOptions(List<CAdoRow*> &outOptions, long projectID);
			static bool SGetCheckRepeatOptions(List<CAdoRow*> &outOptions, long projectID);

			/// <summary>
			/// 获取“闭合性”配置
			/// </summary>
			/// <remarks>输出的每一行记录中包含全部字段：[ID、ProjectID、LayerName、PropertyName、CheckType、Expected、ErrorMessage、Summary]</remarks>
			/// <param name="outOptions">[O] 输出记录行。</param>
			/// <param name="projectID">[I] 指定要查询的项目编号。</param>
			/// <returns>若执行成功，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.7.30   15:06"/>
			bool GetCheckClosedOptions(List<CAdoRow*> &outOptions, long projectID);
			static bool SGetCheckClosedOptions(List<CAdoRow*> &outOptions, long projectID);

			/// <summary>
			/// 获取“许可证证号、收件编号、许可证核发时间关联性”配置
			/// </summary>
			/// <remarks>输出的每一行记录中包含全部字段：[ID、ProjectID、LayerName、PropertyName、CheckType、Expected、ErrorMessage、Summary]</remarks>
			/// <param name="outOptions">[O] 输出记录行。</param>
			/// <param name="projectID">[I] 指定要查询的项目编号。</param>
			/// <returns>若执行成功，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.7.30   15:07"/>
			bool GetCheckZhuhaiRelationOptions(List<CAdoRow*> &outOptions, long projectID);
			static bool SGetCheckZhuhaiRelationOptions(List<CAdoRow*> &outOptions, long projectID);

			/// <summary>
			/// 获取“属性值规范性检测”配置
			/// </summary>
			/// <remarks>输出的每一行记录中包含全部字段：[ID、ProjectID、LayerName、PropertyName、CheckType、Expected、ErrorMessage、Summary]</remarks>
			/// <param name="outOptions">[O] 输出记录行。</param>
			/// <param name="projectID">[I] 指定要查询的项目编号。</param>
			/// <returns>若执行成功，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.7.30   15:07"/>
			bool GetCheckRegexpOptions(List<CAdoRow*> &outOptions, long projectID);
			static bool SGetCheckRegexpOptions(List<CAdoRow*> &outOptions, long projectID);

			/// <summary>
			/// 获取“附件检测”配置
			/// </summary>
			/// <remarks>输出的每一行记录中包含全部字段：[ID、ProjectID、LayerName、PropertyName、CheckType、Expected、ErrorMessage、Summary]</remarks>
			/// <param name="outOptions">[O] 输出记录行。</param>
			/// <param name="projectID">[I] 指定要查询的项目编号。</param>
			/// <returns>若执行成功，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.7.30   15:08"/>
			bool GetCheckAttachmentOptions(List<CAdoRow*> &outOptions, long projectID);
			static bool SGetCheckAttachmentOptions(List<CAdoRow*> &outOptions, long projectID);

			/// <summary>
			/// 获取“固定长度”配置
			/// </summary>
			/// <remarks>输出的每一行记录中包含全部字段：[ID、ProjectID、LayerName、PropertyName、CheckType、Expected、ErrorMessage、Summary]</remarks>
			/// <param name="outOptions">[O] 输出记录行。</param>
			/// <param name="projectID">[I] 指定要查询的项目编号。</param>
			/// <returns>若执行成功，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.7.30   15:09"/>
			bool GetCheckFixLengthOptions(List<CAdoRow*> &outOptions, long projectID);
			static bool SGetCheckFixLengthOptions(List<CAdoRow*> &outOptions, long projectID);

			/// <summary>
			/// 获取“用地代码与用地性质关联性”配置
			/// </summary>
			/// <remarks>输出的每一行记录中包含全部字段：[ID、ProjectID、LayerName、PropertyName、CheckType、Expected、ErrorMessage、Summary]</remarks>
			/// <param name="outOptions">[O] 输出记录行。</param>
			/// <param name="projectID">[I] 指定要查询的项目编号。</param>
			/// <returns>若执行成功，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.7.30   15:10"/>
			bool GetCheckYDDaimaAndXingzhiOptions(List<CAdoRow*> &outOptions, long projectID);
			static bool SGetCheckYDDaimaAndXingzhiOptions(List<CAdoRow*> &outOptions, long projectID);

			/// <summary>
			/// 获取“删除完全重叠多段线”配置
			/// </summary>
			/// <remarks>输出的每一行记录中包含全部字段：[ID、ProjectID、LayerName、PropertyName、CheckType、Expected、ErrorMessage、Summary]</remarks>
			/// <param name="outOptions">[O] 输出记录行。</param>
			/// <param name="projectID">[I] 指定要查询的项目编号。</param>
			/// <returns>若执行成功，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.10.8   14:34"/>
			bool GetRemoveEmbedLineOptions(List<CAdoRow*> &outOptions, long projectID);
			static bool SGetRemoveEmbedLineOptions(List<CAdoRow*> &outOptions, long projectID);

			/// <summary>
			/// 获取“管线管点关联性检测”配置
			/// </summary>
			/// <remarks>输出的每一行记录中包含全部字段：[ID、ProjectID、LayerName、PropertyName、CheckType、Expected、ErrorMessage、Summary]</remarks>
			/// <param name="outOptions">[O] 输出记录行。</param>
			/// <param name="projectID">[I] 指定要查询的项目编号。</param>
			/// <returns>若执行成功，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.10.12   9:12"/>
			bool GetCheckPipeLineMatchOptions(List<CAdoRow*> &outOptions, long projectID);
			static bool SGetCheckPipeLineMatchOptions(List<CAdoRow*> &outOptions, long projectID);
			
		private:
			virtual int OnReflectTo(); 
		};

	}
}