#ifndef _PROPERTY_H_
#define _PROPERTY_H_

#include "HandleBase.h"
#include <list>
//#pragma warning(disable:4355)

namespace Ext
{

	namespace CPP
	{

		template<typename Arg>
		class GetHandle : public HandleBase<int, Arg>
		{
		public:
			static const GetHandle<Arg> Null;

		public:
			GetHandle() : HandleBase<int, Arg>()
			{
			}

			// 拷贝构造函数，STL 容器需要该函数
			GetHandle(const GetHandle &fun)
			{
				m_pFun = NULL;
				*this = fun;
			}

			// 全局函数或静态函数
			template <typename Fun>
			GetHandle(const Fun &fun) : HandleBase<int, Arg>(fun)
			{
			}

			// 成员函数
			template <typename PointerToObj, typename PointerToMemFun>
			GetHandle(const PointerToObj &pObj, const PointerToMemFun &pFun) : HandleBase<int, Arg>(pObj, pFun)
			{
			}

			virtual ~GetHandle()
			{
			}

			GetHandle& operator= (const GetHandle &fun)
			{
				HandleBase<int, Arg>::operator= (fun);
				return *this;
			}

			bool operator== (const GetHandle &handler)
			{
				return HandleBase<int, Arg>::operator ==(handler);
			}
		};

		template<typename Arg>
		const GetHandle<Arg> GetHandle<Arg>::Null;

		template<typename Arg>
		class SetHandle : public HandleBase<int, Arg>
		{
		public:
			static const SetHandle<Arg> Null;

		public:

			SetHandle() : HandleBase<int, Arg>()
			{
			}

			// 拷贝构造函数，STL 容器需要该函数
			SetHandle(const SetHandle &fun)
			{
				m_pFun = NULL;
				*this = fun;
			}

			// 全局函数或静态函数
			template <typename Fun>
			SetHandle(const Fun &fun) : HandleBase<int, Arg>(fun)
			{
			}

			// 成员函数
			template <typename PointerToObj, typename PointerToMemFun>
			SetHandle(const PointerToObj &pObj, const PointerToMemFun &pFun) : HandleBase<int, Arg>(pObj, pFun)
			{
			}

			virtual ~SetHandle()
			{
			}

			SetHandle& operator= (const SetHandle &fun)
			{
				HandleBase<int, Arg>::operator= (fun);
				return *this;
			}

			bool operator== (const SetHandle &handler)
			{
				return HandleBase<int, Arg>::operator ==(handler);
			}
		};

		template<typename Arg>
		const SetHandle<Arg> SetHandle<Arg>::Null;

		/// <summary>
		/// 普通的读写属性模板
		/// </summary>
		/// <remarks></remarks>
		/// <param name="Validate">[I] 指定用于验证属性回调方法所属的类型。</param>
		/// <param name="T">[I] 指定属性值的类型。</param>
		template <typename T>
		class DciProperty
		{
		public:

			/// <summary>
			/// 绑定验证方法。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="onStaticGet">[I] 指定用于在获取值时的验证方法。</param>
			/// <param name="onStaticSet">[I] 指定用于在设置值时的验证方法。</param>
			/// <returns></returns>
			/// <author name="peishengh" date="2011.12.13   15:08"/>
			void Binding(const GetHandle<T> &onGet)
			{
				if (onGet.IsValid() == false)
					return;

				if (onGet.IsInstanceHandle())
				{
					this->m_OnGet = onGet;
				}
				else
				{
					this->m_OnStaticGet = onGet;
				}
			}

			void Binding(const SetHandle<T> &onSet)
			{
				if (onSet.IsValid() == false)
					return;

				if (onSet.IsInstanceHandle())
				{
					this->m_OnSet = onSet;
				}
				else
				{
					this->m_OnStaticSet = onSet;
				}
			}

			/// <summary>
			/// 绑定验证方法。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="ownerInstance">[I] 实例验证方法的对象。</param>
			/// <param name="onGet">[I] 指定用于在获取值时的验证方法。</param>
			/// <param name="onSet">[I] 指定用于在设置值时的验证方法。</param>
			/// <returns></returns>
			/// <author name="peishengh" date="2011.12.13   15:07"/>
			void Binding(const GetHandle<T> &onGet, const SetHandle<T> &onSet)
			{
				this->Binding(onGet);
				this->Binding(onSet);
			}

			DciProperty& operator+= (const GetHandle<T> &handler)
			{
				this->Binding(handler);
				return *this;
			}

			DciProperty& operator+= (const SetHandle<T> &handler)
			{
				this->Binding(handler);
				return *this;
			}

			/// <summary>
			/// 默认构造函数。
			/// </summary>
			/// <remarks></remarks>
			/// <author name="peishengh" date="2011.11.14   14:40"/>
			DciProperty()
			{
				this->Reset();
			}

			/// <summary>
			/// 使用默认值初始化属性。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="defaultValue">[I] 指定默认值。</param>
			/// <returns></returns>
			/// <author name="peishengh" date="2011.12.13   15:21"/>
			DciProperty(T defaultValue)
			{
				this->Reset();
				this->m_Value = defaultValue;
			}

			DciProperty(T defaultValue, const GetHandle<T> &onGet)
			{
				this->Reset();
				this->Binding(onGet);

				this->m_Value = defaultValue;
			}

			DciProperty(T defaultValue, const SetHandle<T> &onSet)
			{
				this->Reset();
				this->Binding(onSet);

				m_Value = defaultValue;
			}

			/// <summary>
			/// 使用默认值和静态回调信息初始化属性。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="defaultValue">[I] 指定默认值。</param>
			/// <param name="onStaticGet">[I] 指定用于在获取值时的验证函数。</param>
			/// <param name="onStaticSet">[I] 指定用于在设置值时的验证函数。</param>
			/// <returns></returns>
			/// <author name="peishengh" date="2011.12.13   15:23"/>
			DciProperty(T defaultValue, const GetHandle<T> &onGet, const SetHandle<T> &onSet)
			{
				this->Reset();
				this->Binding(onGet);
				this->Binding(onSet);

				m_Value = defaultValue;
			}

			/// <summary>
			/// 拷贝构造函数。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="rSource">[I] 指定拷贝的源对象。</param>
			/// <author name="peishengh" date="2011.11.14   14:44"/>
			DciProperty(const DciProperty &rSource)
			{
				*this = rSource;
			}

			/// <summary>
			/// 赋值运算符。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="rSource">[I] 指定赋值的源对象。</param>
			/// <returns>当前对象。</returns>
			/// <author name="peishengh" date="2011.11.14   14:45"/>
			DciProperty& operator = (const DciProperty &rSource)
			{
				if (this != &rSource)
				{
					this->m_Value = rSource.m_Value;
				}
				return *this;
			}

			~DciProperty()
			{
				this->m_OnGet.Clear();
				this->m_OnSet.Clear();

				this->m_OnStaticGet.Clear();
				this->m_OnStaticSet.Clear();

				this->Reset();
			}

			/// <summary>
			/// 重写：类型转换操作符。
			/// </summary>
			/// <remarks>使得当前对象的值可以转换为属性值类型的值。</remarks>
			/// <returns>返回属性值类型的值。</returns>
			/// <author name="peishengh" date="2011.11.14   14:46"/>
			operator T(void)
			{
				return this->GetValue(m_Value);
			}

			/// <summary>
			/// 重写：类型转换操作符。
			/// </summary>
			/// <remarks>使得当前对象的值可以转换为属性值类型的值。</remarks>
			/// <param name="">返回属性值类型的值。</param>
			/// <returns></returns>
			/// <author name="peishengh" date="2011.12.13   15:26"/>
			operator T(void) const
			{
				T temp = m_Value;
				return this->GetValue(temp);
			}

			/// <summary>
			/// 显示地将当前对象转换为属性值类型对应的值。
			/// </summary>
			/// <remarks></remarks>
			/// <returns>返回属性值类型的值。</returns>
			/// <author name="peishengh" date="2011.11.14   14:47"/>
			T& Raw(void)
			{
				return (this->GetValue(m_Value));
			}

			T& Raw(void) const
			{
				T temp = m_Value;
				return (this->GetValue(temp));
			}

			/// <summary>
			/// 赋值运算符。
			/// </summary>
			/// <remarks>使得属性值类型的值可以相互转换为当前对象的值。</remarks>
			/// <param name="value">[I] 指定属性值。</param>
			/// <returns>返回属性值类型的值。</returns>
			/// <author name="peishengh" date="2011.11.14   14:48"/>
			T& operator = (T value)
			{
				if (m_IsOnSetting == true)
					return m_Value;

				m_IsOnSetting = true;

				// 开始设置属性的值
				//if (m_OnStaticSet.size() != 0)
				if (m_OnStaticSet.IsValid())
				{
					int result = m_OnStaticSet.Invoke(value);
					if (result == 0)
						m_Value = value;
				}

				//if (m_OnSet.size() != 0)
				if (m_OnSet.IsValid())
				{
					int result = m_OnSet.Invoke(value);
					if (result == 0)
						m_Value = value;
				}

				if (m_OnStaticSet.IsValid() == false && m_OnSet.IsValid() == false)
					m_Value = value;

				// 设置属性的值结束
				m_IsOnSetting = false;

				return m_Value;
			}

		private:

			void Reset()
			{
				m_OnGet.Clear();
				m_OnSet.Clear();
				m_OnStaticGet.Clear();
				m_OnStaticSet.Clear();

				m_IsOnSetting = false;
			}

			/// <summary>
			/// 获取当前的属性值。
			/// </summary>
			/// <remarks></remarks>
			/// <returns>返回当前的属性值。</returns>
			/// <author name="peishengh" date="2011.11.14   14:51"/>
			T& GetValue(T& v) const
			{		
				if (this->m_OnStaticGet.IsValid() == true)
					m_OnStaticGet.Invoke(v);

				if (this->m_OnGet.IsValid() == true)
					m_OnGet.Invoke(v);

				return v;
			}

		protected:
			/// <summary>
			/// 保存属性的值。
			/// </summary>
			T m_Value;

			/// <summary>
			/// 实例函数指针：在获取值时进行验证。
			/// </summary>
			GetHandle<T> m_OnGet;

			/// <summary>
			/// 实例函数指针：在设置值时进行验证。
			/// </summary>
			SetHandle<T> m_OnSet;

			/// <summary>
			/// 静态函数指针：在获取值时进行验证。
			/// </summary>
			GetHandle<T> m_OnStaticGet;

			/// <summary>
			/// 静态函数指针：在设置值时进行验证。
			/// </summary>
			SetHandle<T> m_OnStaticSet;

			/// <summary>
			/// 在设置属性的值提供状态，避免进入递归或嵌套设置属性值。
			/// </summary>
			bool m_IsOnSetting;
		};

		/// <summary>
		/// 普通的读写属性模板
		/// </summary>
		/// <remarks></remarks>
		/// <param name="Validate">[I] 指定用于验证属性回调方法所属的类型。</param>
		/// <param name="T">[I] 指定属性值的类型。</param>
		template <typename T>
		class ReadOnly
		{
		public:

			/// <summary>
			/// 绑定验证方法。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="onStaticGet">[I] 指定用于在获取值时的验证方法。</param>
			/// <param name="onStaticSet">[I] 指定用于在设置值时的验证方法。</param>
			/// <returns></returns>
			/// <author name="peishengh" date="2011.12.13   15:08"/>
			void Binding(const GetHandle<T> &onGet)
			{
				if (onGet.IsValid() == false)
					return;

				if (onGet.IsInstanceHandle())
				{
					this->m_OnGet = onGet;
				}
				else
				{
					this->m_OnStaticGet = onGet;
				}
			}

			ReadOnly& operator+= (const GetHandle<T> &handler)
			{
				this->Binding(handler);
				return *this;
			}

			/// <summary>
			/// 默认构造函数。
			/// </summary>
			/// <remarks></remarks>
			/// <author name="peishengh" date="2011.11.14   14:40"/>
			ReadOnly()
			{
				this->Reset();
			}

			ReadOnly(T defaultValue, const GetHandle<T> &onGet)
			{
				this->Reset();
				this->Binding(onGet);

				this->m_Value = defaultValue;
			}

			/// <summary>
			/// 拷贝构造函数。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="rSource">[I] 指定拷贝的源对象。</param>
			/// <author name="peishengh" date="2011.11.14   14:44"/>
			ReadOnly(const ReadOnly &rSource)
			{
				*this = rSource;
			}

			/// <summary>
			/// 赋值运算符。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="rSource">[I] 指定赋值的源对象。</param>
			/// <returns>当前对象。</returns>
			/// <author name="peishengh" date="2011.11.14   14:45"/>
			ReadOnly& operator = (const ReadOnly &rSource)
			{
				if (this != &rSource)
				{
					this->m_Value = rSource.m_Value;
				}
				return *this;
			}

			~ReadOnly()
			{
				this->m_OnGet.Clear();
				this->m_OnStaticGet.Clear();

				this->Reset();
			}

			/// <summary>
			/// 重写：类型转换操作符。
			/// </summary>
			/// <remarks>使得当前对象的值可以转换为属性值类型的值。</remarks>
			/// <returns>返回属性值类型的值。</returns>
			/// <author name="peishengh" date="2011.11.14   14:46"/>
			operator T(void)
			{
				return this->GetValue(m_Value);
			}

			/// <summary>
			/// 重写：类型转换操作符。
			/// </summary>
			/// <remarks>使得当前对象的值可以转换为属性值类型的值。</remarks>
			/// <param name="">返回属性值类型的值。</param>
			/// <returns></returns>
			/// <author name="peishengh" date="2011.12.13   15:26"/>
			operator T(void) const
			{
				T temp = m_Value;
				return this->GetValue(temp);
			}

			/// <summary>
			/// 显示地将当前对象转换为属性值类型对应的值。
			/// </summary>
			/// <remarks></remarks>
			/// <returns>返回属性值类型的值。</returns>
			/// <author name="peishengh" date="2011.11.14   14:47"/>
			T To(void)
			{
				return this->GetValue(m_Value);
			}

			T To(void) const
			{
				T temp = m_Value;
				return this->GetValue(temp);
			}

		private:

			void Reset()
			{
				m_OnGet.Clear();
				m_OnStaticGet.Clear();
			}

			/// <summary>
			/// 获取当前的属性值。
			/// </summary>
			/// <remarks></remarks>
			/// <returns>返回当前的属性值。</returns>
			/// <author name="peishengh" date="2011.11.14   14:51"/>
			T GetValue(T& v) const
			{
				if (this->m_OnStaticGet.IsValid() ==  true)
					m_OnStaticGet.Invoke(v);

				if (this->m_OnGet.IsValid() == true)
					m_OnGet.Invoke(v);

				return v;
			}

		private:
			/// <summary>
			/// 保存属性的值。
			/// </summary>
			T m_Value;

			/// <summary>
			/// 实例函数指针：在获取值时进行验证。
			/// </summary>
			GetHandle<T> m_OnGet;

			/// <summary>
			/// 静态函数指针：在获取值时进行验证。
			/// </summary>
			GetHandle<T> m_OnStaticGet;
		};

	}// End namespace CPP

} // End namespace Ext
#endif // _PROPERTY_H_