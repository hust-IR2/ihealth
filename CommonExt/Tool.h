#pragma once

#ifdef DCICOMMONEXT_MODULE
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllexport)
#else
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllimport)
#endif

#include "List.h"
using namespace Ext::Collection;

#include <vector>

namespace Ext
{

	/// <summary>
	/// 提供一组静态方法，协助完成一些通用的逻辑操作。
	/// </summary>
	class DCICOMMONEXT_MODULE_EXPIMP CTool
	{
	public:
		CTool(void);
		~CTool(void);

		static int CompareStringWithNoCase(const CString &a, const CString &b);

		/// <summary>
		/// 释放 std::vector<T*> 容器的资源。
		/// </summary>
		/// <remarks></remarks>
		/// <param name="vect">[I] 指定要释放资源的容器。</param>
		/// <returns></returns>
		/// <author name="hps" date="2012.5.22   9:27"/>
		template<typename T> static void ReleaseVector(std::vector<T*> &vect)
		{
			for (std::vector<T*>::const_iterator begin = vect.begin(); begin != vect.end(); ++begin)
			{
				delete (*begin);
			}
			vect.clear();
		}

		template<typename T> static void ReleaseList(List<T*> &list)
		{
			int count = list.Count();
			for (int i = 0; i < count; ++i)
			{
				T *p = list[i];
				if (p == NULL)
					continue;

				delete p;
			}
			list.Clear();
			list.Shrink();
		}
	};

}