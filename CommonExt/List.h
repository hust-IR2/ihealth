#pragma once

#ifndef COMMONEXT_LIST
#define COMMONEXT_LIST

#include "Collection.h"
#include "MemoryStrategy.h"
#include "Sort.h"
#include "ExtCPP.h" // C++ 语法扩展

namespace Ext
{

	namespace Collection
	{
		/// <summary>
		/// 表示线性列表容器类。
		/// </summary>
		template <class T> 
		class __declspec(dllexport) List
		{
		public:

			/// <summary>
			/// 用于比较两个 T 元素的大小的函数指针。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="a">[I] 比较的第一个 T 元素的指针。</param>
			/// <param name="b">[I] 比较的第二个 T 元素的指针。</param>
			/// <returns>
			/// 升序比较时的返回值：
			/// -1 if a < b is true
			///  1 if b < a is true
			///  0 if niether a < b nor b < a is true 
			/// 
			/// 降序比较时的返回值：
			/// -1 if a > b is true
			///  1 if b > a is true
			///  0 if niether a < b nor b < a is true 
			/// </returns>
			/// <author name="hps" date="2012.6.8   9:42"/>
			typedef  int(*CompareFunc)(const T &a, const T &b);

		public:

			/// <summary>
			/// 初始化一个空的列表容器。
			/// </summary>
			/// <remarks></remarks>
			/// <author name="hps" date="2012.6.5   11:59"/>
			List();

			/// <summary>
			/// 初始化预保留大小的列表容器。
			/// </summary>
			/// <remarks></remarks>
			/// <param name=""></param>
			/// <author name="hps" date="2012.6.5   12:00"/>
			List(int capacity);

			/// <summary>
			/// 拷贝构造函数。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="rSource">[I] 指定要拷贝的源列表。</param>
			/// <author name="hps" date="2012.6.5   12:02"/>
			List(const List<T>& rSource);

			/// <summary>
			/// 析构函数。
			/// </summary>
			/// <remarks></remarks>
			/// <author name="hps" date="2012.6.5   12:03"/>
			virtual ~List();

			/// <summary>
			/// 赋值运算符。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="&">[I] 指定表达式的右值。</param>
			/// <returns>返回表达式左值的引用。</returns>
			/// <author name="hps" date="2012.6.5   12:03"/>
			List<T>& operator=(const List<T>& rSource);

			/// <summary>
			/// 紧急销毁列表。
			/// </summary>
			/// <remarks>仅当列表的内存无效或无法控制时，才能调用此方法。此方法将列表内部的所有信息都清空，严重时可能导致析构时崩溃。</remarks>
			/// <param name=""></param>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   12:07"/>
			void EmergencyDestroy(void);

			/// <summary>
			/// 获取列表中实际元素的数目
			/// </summary>
			/// <remarks></remarks>
			/// <returns>返回实际存在在列表中元素的数目。</returns>
			/// <author name="hps" date="2012.6.5   12:09"/>
			int Count() const;

			/// <summary>
			/// 获取列表的容量的大小。
			/// </summary>
			/// <remarks></remarks>
			/// <returns>返回列表容量的大小。</returns>
			/// <author name="hps" date="2012.6.5   12:10"/>
			int Capacity() const;

			/// <summary>
			/// 获取列表实际使用内存的大小(以字节为单位)。
			/// </summary>
			/// <remarks></remarks>
			/// <returns>返回列表实际使用内存的大小。</returns>
			/// <author name="hps" date="2012.6.5   12:14"/>
			unsigned int TotalMemory() const;

			/// <summary>
			/// 索引器，调用方必需确保索引的值在 [0, capacity) 之间。
			/// </summary>
			/// <remarks></remarks>
			/// <param name=""></param>
			/// <returns>返回索引处的元素的引用</returns>
			/// <author name="hps" date="2012.6.5   12:12"/>
			T& operator[](int index);
			T& operator[](unsigned int index);
			const T& operator[](int index) const;
			const T& operator[](unsigned int index) const;

			/// <summary>
			/// 类型转换符。
			/// </summary>
			/// <remarks></remarks>
			/// <returns>返回列表起始元素的指针。</returns>
			/// <author name="hps" date="2012.6.5   12:15"/>
			operator T*();
			operator const T*() const;

			/// <summary>
			/// 获取列表中的第一个元素。
			/// </summary>
			/// <remarks></remarks>
			/// <returns>返回第一个元素的指针。若列表为空，则返回 NULL。</returns>
			/// <author name="hps" date="2012.6.5   12:16"/>
			T& First();
			const T& First() const;

			/// <summary>
			/// 获取列表中的最后一个元素。
			/// </summary>
			/// <remarks></remarks>
			/// <returns>返回最后一个元素的指针。若列表为空，则返回 NULL。</returns>
			/// <author name="hps" date="2012.6.5   12:20"/>
			T& Last();
			const T& Last() const;

			/// <summary>
			/// 获取指定索引处的元素。
			/// </summary>
			/// <remarks>注：该方法将检测索引范围的有效性，若输入的范围越界，则返回 NULL。</remarks>
			/// <param name="">[I] 指定要检索的索引。</param>
			/// <returns>返回指定索引处元素的指针。</returns>
			/// <author name="hps" date="2012.6.5   12:17"/>
			T& At(int index);
			T& At(unsigned int index);
			const T& At(int index) const;
			const T& At(unsigned int index) const;

			/// <summary>
			/// 获取一个值，只是当前的 List 容器是否为空。
			/// </summary>
			/// <remarks></remarks>
			/// <returns>若当期容器不包含任何元素，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.6.21   15:27"/>
			bool IsEmpty() const;

			/// <summary>
			/// 在列表中追加一个元素类型的一个具有默认值的元素。
			/// </summary>
			/// <remarks></remarks>
			/// <returns>返回添加仅列表中元素的引用。</returns>
			/// <author name="hps" date="2012.6.5   12:22"/>
			T& AppendNew();

			/// <summary>
			/// 将指定元素的副本追加到列表的末尾。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="rElement">[I] 指定要追加的元素。</param>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   12:26"/>
			void Append(const T& rElement);

			/// <summary>
			/// 将指定数组的元素的副本追加到列表的末尾。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="count">[I] 指定需要从数组 pArray 中追加到列表的元素的数量。</param>
			/// <param name="pArray">[I] 指定数组。</param>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   12:50"/>
			void Append(int count, const T* pArray);

			/// <summary>
			/// 将指定列表 list 中的所有元素追加到当前列表中。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="list">[I] 指定要追加元素的列表。</param>
			/// <returns></returns>
			/// <author name="hps" date="2012.7.2   16:20"/>
			void Append(const List<T> &list);

			/// <summary>
			/// 在列表的的指定索引处插入元素的副本。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="index">[I] 指定新元素插入的位置。</param>
			/// <param name="rSource">[I] 指定元素。</param>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   13:19"/>
			void Insert(int index, const T& rSource);

			/// <summary>
			/// 移除列表中的最后一个元素。
			/// </summary>
			/// <remarks></remarks>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   13:21"/>
			void Remove();

			/// <summary>
			/// 移除指定索引处的元素。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="index">[I] 指定要移除元素的索引。</param>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   13:22"/>
			void Remove(int index);

			/// <summary>
			/// 清空列表
			/// </summary>
			/// <remarks></remarks>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   13:23"/>
			void Clear();

			/// <summary>
			/// 反转列表。
			/// </summary>
			/// <remarks></remarks>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   13:23"/>
			void Reverse();

			/// <summary>
			/// 交换指定的两个索引的元素。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="index1">[I] 指定索引1.</param>
			/// <param name="index2">[I] 指定索引2.</param>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   13:23"/>
			void Swap(int index1, int index2);

			/// <summary>
			/// 顺序查找列表。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="element">[I] 指定要查找的元素。</param>
			/// <param name="comparer">[I] 指定用于比较的两个元素大小的函数指针。</param>
			/// <returns>返回与指定元素相等的第一个索引值。若没有找到与之匹配的元素，则返回 -1.</returns>
			/// <author name="hps" date="2012.6.5   13:27"/>
			int Search(const T &key, CompareFunc pCompare) const;

			/// <summary>
			/// 使用二分查找法在已排序的列表中查找指定的元素。
			/// </summary>
			/// <remarks>在使用该方法之前，必须确保列表中的元素已经排序。否则返回的结果可能不正确。</remarks>
			/// <param name="element">[I] 指定要查找的元素。</param>
			/// <param name="pCompare">[I] 指定用于比较的两个元素大小的函数指针。</param>
			/// <returns>返回与指定元素相等的第一个索引值。若没有找到与之匹配的元素，则返回 -1.</returns>
			/// <author name="hps" date="2012.6.5   13:31"/>
			int BinarySearch(const T &key, CompareFunc pCompare) const;

			/// <summary>
			/// 使用堆排序算法对当前列表进行排序。
			/// </summary>
			/// <remarks>算法有误，目前勿用！！！</remarks>
			/// <param name="pCompare">[I] 指定用于比较的两个元素大小的函数指针。</param>
			/// <returns>若排序成功，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.6.5   13:34"/>
			virtual bool HeapSort(CompareFunc pCompare);

			/// <summary>
			/// 使用快速排序算法对当前列表进行排序。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="pCompare">[I] 指定用于比较的两个元素大小的函数指针。</param>
			/// <returns>若排序成功，则返回 true；否则返回 false。</returns>
			/// <author name="hps" date="2012.6.5   13:36"/>
			virtual bool QuickSort(CompareFunc pCompare);

			/// <summary>
			/// Permutes the array so that output[i] = input[index[i]].
			/// The index[] array should be a permutation of (0,...,Count()-1).
			/// </summary>
			/// <remarks></remarks>
			/// <param name="inputArray"></param>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   14:03"/>
			bool Permute(const int* inputArray);

			/// <summary>
			/// 销毁列表中的所有元素并使用默认构造函数得到的对象重新添加列表。
			/// </summary>
			/// <remarks>该方法不会改变当前列表的元素数量和容量。</remarks>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   14:04"/>
			void Zero();

			/// <summary>
			/// 预保留指定数量的容量。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="capacity"></param>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   14:06"/>
			void Reserve(int capacity);

			/// <summary>
			/// 清除未使用的容量。
			/// </summary>
			/// <remarks></remarks>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   14:07"/>
			void Shrink();

			/// <summary>
			/// 销毁列表。
			/// </summary>
			/// <remarks>该方法执行完后，列表的元素数量和容量都被设置为 0。</remarks>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   14:07"/>
			void Destroy();

			/// <summary>
			/// 为元素分配内存。
			/// </summary>
			/// <remarks>
			/// 默认情况下，List 在内部使用 OnRealloc() 来为元素分配内存。若要使用新的内存分配策略，请重写 Realloc(...) 方法。
			/// 在重写 Realloc(...) 方法时，确保该方法满足下列条件：
			/// 1). 如果 ptr = NULL, 且 capacity = 0，函数返回 NULL。
			/// 2). 如果 ptr = NULL，且 capacity > 0, 将分配 capacity*sizeof(T) 大小的内存块并返回该内存块的指针；若分配内存失败，则返回 NULL。
			/// 3). 如果 ptr != NULL, 且 capacity = 0，将释放 ptr 执行的内存块并返回 NULL。
			/// 4). 如果 ptr != NULL, 且 capacity > 0, 将重新分配内存块并返回内存块的指针；若分配内存失败，则返回 NULL。
			/// </remarks>
			/// <param name="ptr">[I] 指定元素的指针。</param>
			/// <param name="capacity">[I] 指定分配内存的容量。</param>
			/// <returns>返回指向分配的内存的指针。</returns>
			/// <author name="hps" date="2012.6.5   14:11"/>
			virtual T* Realloc(T* ptr, int capacity);

			/// <summary>
			/// 返回列表内部使用的数组指针。
			/// </summary>
			/// <remarks></remarks>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   14:23"/>
			T* Array();
			const T* Array() const;

			/// <summary>
			/// 设置列表中元素的数量。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="count">[I] 指定元素的数量。</param>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   14:24"/>
			void SetCount(int count);

			/// <summary>
			/// 设置列表的容量。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="capacity">[I] 指定新的容量大小。</param>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   14:24"/>
			void SetCapacity(int capacity);

			/// <summary>
			/// 当列表的容量不能满足需求时，使用该方法计算新的容量。
			/// </summary>
			/// <remarks></remarks>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   14:25"/>
			int NewCapacity() const;

			/// <summary>
			/// 获取列表容量的增长步调。
			/// </summary>
			/// <remarks></remarks>
			/// <returns>返回内存增长的步调。</returns>
			/// <author name="hps" date="2012.7.11   12:20"/>
			int GetMemoryStep() const;

			/// <summary>
			/// 设置列表容量的增长步调。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="step">[I] 指定增长的步调。</param>
			/// <returns></returns>
			/// <author name="hps" date="2012.7.11   12:21"/>
			void SetMemoryStep(int step);

			/// <summary>
			/// 返回列表内部使用的数组指针并销毁当前列表。
			/// </summary>
			/// <remarks>调用方需负责释放(调用：OnFree(...))该方法返回的指针的内存。</remarks>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   14:27"/>
			T* KeepArray();

			/// <summary>
			/// 请勿使用该方法。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="*"></param>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   14:31"/>
			void SetArray(T*);

			/// <summary>
			/// 使用指定的数组信息重置当前列表。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="inputArray">[I] 指定数组。</param>
			/// <param name="count">[I] 指定大小。</param>
			/// <param name="capacity">[I] 指定容量。</param>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   14:31"/>
			void SetArray(T* inputArray, int count, int capacity);

		public:
			Ext::CPP::Event OnAppending;
			Ext::CPP::Event OnAppended;
			Ext::CPP::Event OnRemoving;
			Ext::CPP::Event OnRemoved;

		protected:

			int BinarySearch(T a[], int low, int heigh, const T &key, CompareFunc pCompare) const;

			/// <summary>
			/// 移动元素
			/// </summary>
			/// <remarks></remarks>
			/// <param name="destIndex">[I] 目标索引。</param>
			/// <param name="srcIndex">[I] 原索引。</param>
			/// <param name="elementCount">[I] 元素数量。</param>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   14:34"/>
			void Move(int  destIndex, int  srcIndex, int  elementCount);

			/// <summary>
			/// 构造默认元素值。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="t"></param>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   14:35"/>
			void ConstructDefaultElement(T* t);

			/// <summary>
			/// 销毁元素
			/// </summary>
			/// <remarks></remarks>
			/// <param name="t"></param>
			/// <returns></returns>
			/// <author name="hps" date="2012.6.5   14:35"/>
			void DestroyElement(T& t);

			/// <summary>
			/// 内部用于存储元素的数组
			/// </summary>
			T*   m_a;        // pointer to array memory

			/// <summary>
			/// 记录列表中实际元素的数量，满足：0 <= m_count <= m_capacity
			/// </summary>
			int  m_count;

			/// <summary>
			/// 记录 m_a 数组的实际大小，即列表的容量。
			/// </summary>
			int  m_capacity;

			/// <summary>
			/// 内存增长步调。默认为 8
			/// </summary>
			int  m_memoryStep;
		};

		// List 模板的实现
		#include "ListImplement.h"

	} // End namespace Collection
} // End namespace Ext

#endif // COMMONEXT_LIST