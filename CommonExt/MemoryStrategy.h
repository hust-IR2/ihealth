
#ifndef COMMONEXT_MEMORYSTRATEGY
#define COMMONEXT_MEMORYSTRATEGY

/// <summary>
/// 负责内存的分配和释放
/// </summary>
class __declspec(dllexport) CMemoryStrategy
{
public:

	/// <summary>
	/// 向系统申请分配指定size个字节的内存空间，返回一个指向分配起始地址的指针。
	/// </summary>
	/// <remarks>分配的内存未执行任何初始化，可能包含垃圾数据。</remarks>
	/// <param name="size">[I] 指定要分配的内存的大小。</param>
	/// <returns>若内存分配成功，则返回一个指向分配起始地址的指针；否则返回 NULL。</returns>
	/// <author name="hps" date="2012.6.27   16:28"/>
	static void* OnMalloc(size_t size);

	/// <summary>
	/// 在内存的动态存储区中分配n个长度为size的连续空间，返回一个指向分配起始地址的指针。
	/// </summary>
	/// <remarks>在动态分配完内存后，自动初始化该内存空间为零。</remarks>
	/// <param name="n">[I] 指定要分配的内存的数量。</param>
	/// <param name="size">[I] 指定内存分配单元的大小。</param>
	/// <returns>若内存分配成功，则返回一个指向分配起始地址的指针；否则返回 NULL。</returns>
	/// <author name="hps" date="2012.6.27   16:30"/>
	static void* OnCalloc(size_t n, size_t size);

	/// <summary>
	/// 释放 memblock 指向的内存空间的数据。
	/// </summary>
	/// <remarks></remarks>
	/// <param name="memblock">[I] 指定要释放资源的指针。</param>
	/// <returns></returns>
	/// <author name="hps" date="2012.6.27   16:34"/>
	static void OnFree(void* memblock);

	/// <summary>
	/// 释放 memblock 指向的内存空间的资源，并重新分配大小为 size 的内存空间。
	/// </summary>
	/// <remarks></remarks>
	/// <param name="memblock">[I] 指定要重新分配资源的指针。</param>
	/// <param name="size">[I] 指定重新分配资源的大小。</param>
	/// <returns>若内存分配成功，则返回一个指向分配起始地址的指针；否则返回 NULL。</returns>
	/// <author name="hps" date="2012.6.27   16:36"/>
	static void* OnRealloc(void *memblock, size_t size);
};

#endif // COMMONEXT_MEMORYSTRATEGY
