#ifndef _HANDLE_BASE_H_
#define _HANDLE_BASE_H_

#include <typeinfo.h>
#pragma warning(disable:4715)

namespace Ext
{

	namespace CPP
	{

		template<typename Result, typename Arg>
		class FunBase
		{
		public:
			FunBase() 
			{
			};

			virtual ~FunBase() 
			{
			};

			virtual Result Invoke(Arg &e) = 0;

			virtual bool operator== (const FunBase &fun) = 0;

			virtual FunBase* Clone() = 0;
		};

		template <typename PointerToOwner, typename PointerToFun, typename Result, typename Arg>
		class MemberFun : public FunBase<Result, Arg>
		{
		private:
			typedef MemberFun<PointerToOwner, PointerToFun, Result, Arg> this_type;

		public:

			MemberFun(const PointerToOwner &pObj, PointerToFun pMemFn)
			{
				m_pObj = pObj;
				m_pFun = pMemFn;
			}

			virtual ~MemberFun()
			{
			}

			Result Invoke(Arg &e)
			{
				return (m_pObj->*m_pFun)(e);
			}

			FunBase<Result, Arg>* Clone()
			{
				return new this_type(m_pObj, m_pFun);
			}

			bool operator== (const FunBase<Result, Arg> &fun)
			{
				if (typeid(fun) == typeid(this_type))
				{
					const this_type& rFun = dynamic_cast<const this_type& >(fun);
					return (m_pObj == rFun.m_pObj && m_pFun == rFun.m_pFun);
				}

				return false;
			}

		protected:
			PointerToOwner m_pObj;
			PointerToFun m_pFun;

		};

		template <typename Fun, typename Result, typename Arg>
		class StaticFun : public FunBase<Result, Arg>
		{
		private:
			typedef StaticFun<Fun, Result, Arg> this_type;

		public:

			StaticFun(const Fun &fun)
			{ 
				m_pFun = fun;
			};

			virtual ~StaticFun()
			{
			}

			Result Invoke(Arg &e)
			{
				return (*m_pFun)(e);
			}

			FunBase<Result, Arg>* Clone()
			{
				return new this_type(m_pFun);
			}

			bool operator== (const FunBase<Result, Arg> &fun)
			{
				if (typeid(fun) == typeid(this_type))
				{
					const this_type& rFun = dynamic_cast<const this_type&>(fun);
					return m_pFun == rFun.m_pFun;
				}

				return false;
			}

		protected:
			Fun m_pFun;
		};

#if 0
		template <typename Fun, typename Arg>
		class StaticFun<Fun, void, Arg> : public FunBase<void, Arg>
		{
		private:
			typedef StaticFun<Fun, void, Arg> this_type;

		public:

			StaticFun(const Fun &fun)
			{ 
				m_pFun = fun;
			};

			virtual ~StaticFun()
			{
			}

			void Invoke(Arg &e)
			{
				(*m_pFun)(e);
			}

			FunBase* Clone()
			{
				return new this_type(m_pFun);
			}

			bool operator== (const FunBase &fun)
			{
				if (typeid(fun) == typeid(this_type))
				{
					const this_type& rFun = dynamic_cast<const this_type&>(fun);
					return m_pFun == rFun.m_pFun;
				}

				return false;
			}

		protected:
			Fun m_pFun;
		};
#endif

		template<typename Result, typename Arg>
		class HandleBase
		{
		public:
			HandleBase()
			{
				m_pFun = NULL;
				m_IsValid = false;
			}

			// 拷贝构造函数，STL 容器需要该函数
			HandleBase(const HandleBase &fun)
			{
				m_pFun = NULL;
				m_IsValid = fun.m_IsValid;
				m_IsInstanceHandle = fun.m_IsInstanceHandle;

				*this = fun;
			}

			// 全局函数或静态函数
			template <typename Fun>
			HandleBase(const Fun &fun)
			{
				m_IsValid = true;
				this->m_IsInstanceHandle = false;
				m_pFun = new StaticFun<Fun, Result, Arg>(fun);
			}

			// 成员函数
			template <typename PointerToObj, typename PointerToMemFun>
			HandleBase(const PointerToObj &pObj, const PointerToMemFun &pFun)
			{
				m_IsValid = true;
				this->m_IsInstanceHandle = true;
				m_pFun = new MemberFun<PointerToObj, PointerToMemFun, Result, Arg>(pObj, pFun);
			}

			virtual ~HandleBase()
			{
				Clear();
			}

			HandleBase& operator= (const HandleBase &fun)
			{
				Clear();

				if (fun.m_pFun)
				{
					m_pFun = fun.m_pFun->Clone();
				}

				m_IsValid = fun.m_IsValid;
				m_IsInstanceHandle = fun.m_IsInstanceHandle;

				return *this;
			}

			Result Invoke(Arg &e)
			{
				if (m_pFun != NULL)
				{
					return (*m_pFun).Invoke(e);
				}
			}

			Result Invoke(Arg &e) const
			{
				if (m_pFun != NULL)
				{
					return (*m_pFun).Invoke(e);
				}
			}

			bool operator== (const HandleBase &handler)
			{
				if (m_pFun == NULL || handler.m_pFun == NULL)
				{
					return false;
				}

				if (typeid(m_pFun) == typeid(handler.m_pFun))
				{
					return (*m_pFun) == (*(handler.m_pFun));
				}

				return false;
			}

			void Clear()
			{
				if (m_pFun != NULL)
				{
					delete m_pFun;
					m_pFun = NULL;
				}
				this->m_IsValid = false;
			}

			bool IsValid() const
			{
				return m_IsValid;
			}

			bool IsInstanceHandle() const
			{
				return m_IsInstanceHandle;
			}

		protected:
			FunBase<Result, Arg> *m_pFun;

			bool m_IsValid;

			bool m_IsInstanceHandle;
		};


	} // End namespace CPP

} // End namesapce Ext

#endif // _HANDLE_BASE_H_