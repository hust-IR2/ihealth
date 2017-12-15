
#pragma once

#ifdef DCICOMMONEXT_MODULE
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllexport)
#else
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllimport)
#endif

#pragma warning(disable:4996)
#include <string>
namespace Ext
{
	namespace CPP
	{
		class DCICOMMONEXT_MODULE_EXPIMP Panic
		{
		public:
			size_t Id;
			std::wstring Info;
			void* Tag;

		public:
			Panic()
			{
				this->Reset();
			}

			Panic(void *pTag)
			{
				this->Reset();
				Tag = pTag;
			}

			Panic(size_t id, const std::wstring &info)
			{
				this->Reset();
				Id = id;
				Info = info;
			}

			Panic(size_t id, const std::wstring &info, void *pTag)
			{
				Id = id;
				Info = info;
				Tag = pTag;
			}

			~Panic()
			{
				this->Reset();
			}

			Panic(const Panic &src)
			{
				*this = src;
			}

			Panic& operator= (const Panic &src)
			{
				this->Id = src.Id;
				this->Info = src.Info;
				this->Tag = src.Tag;	
				return *this;
			}

			void SetId(size_t id)
			{
				this->Id = id;
			}

			void SetInfo(const TCHAR *strMessageFormat, ...)
			{
				TCHAR szBuffer[1024] = _T("");

				va_list argList;
				va_start(argList, strMessageFormat);
				_vsntprintf(szBuffer, 1024, strMessageFormat, argList);
				va_end(argList);

				this->Info = szBuffer;
			}

			void Set(size_t id, const TCHAR *strMessageFormat, ...)
			{
				this->Id = id;

				TCHAR szBuffer[1024] = _T("");

				va_list argList;
				va_start(argList, strMessageFormat);
				_vsntprintf(szBuffer, 1024, strMessageFormat, argList);   
				va_end(argList);

				this->Info = szBuffer;
			}

			void SetTag(void* data)
			{
				this->Tag = data;
			}

			void Reset()
			{
				this->Id = 0;
				this->Info = _T("");
				this->Tag = NULL;
			}

			// 指针类型转换。调用：CAdoRow *pRow = panic.GetTag<CAdoRow*>();	// panic 是Panic类型的实例, Tag的实际类型为CAdoRow*
			template<typename T>
			T GetTag()
			{
				T pointer = static_cast<T>(Tag);
				return pointer;
			}

			// 指针的指针转换为指针类型。调用：CAdoRow *pRow = panic.GetPPTag<CAdoRow*>();	// panic 是Panic类型的实例, Tag的实际类型为CAdoRow**
			template<typename T>
			T GetPPTag()
			{
				void** pointer1 = (void**)Tag;
				void*  pointer2 = (void*)(*pointer1);
				T attach = static_cast<T>(pointer2);
				return attach;
			}

			// 值类型转换。调用：int data = panic.GetTagAsValue<int>();	// panic 是Panic类型的实例, Tag的实际类型为int
			template<typename T>
			T GetTagAsValue()
			{
				T *pointer = static_cast<T*>(Tag);
				T &value = *pointer;
				return value;
			}

			bool OK(int okid = 0)
			{
				if (this->Id == okid)
					return true;
				return false;
			}
		};
	} // End namespace CPP
} // End namespace Ext