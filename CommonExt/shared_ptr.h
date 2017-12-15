#pragma once

#include <algorithm>		// std::swap
#include <functional>		// std::less

#define SHARED_PTR_ZERO 0

namespace Ext
{
	namespace CPP
	{
		template<typename T>
		inline void check_delete(T* p)
		{
			if (p != NULL)
			{
				delete p;
				p = NULL;
			}
		}

		template<typename T>
		inline void check_array_delete(T* p)
		{
			if (p != NULL)
			{
				delete[] p;
				p = NULL;
			}
		}

		template<typename T>
		class __declspec(dllexport) shared_ptr
		{
		public:
			explicit shared_ptr(T* p = NULL) : px(p) 
			{
				try 
				{
					pn = new long(SHARED_PTR_ZERO + 1);
				}
				catch(...)
				{
					check_delete(p);
					throw;
				}
			}

			shared_ptr(const shared_ptr& r) : px(r.px)
			{
				++(*(pn = r.pn));
			}

			~shared_ptr(void)
			{
				dispose();
			}

		public:
			shared_ptr& operator=(const shared_ptr& r) 
			{
				share(r.px, r.pn);
				return *this;
			}

			T& operator*() const 
			{
				return *px;
			}

			T* operator->() const 
			{
				return px;
			}

		public:
			void reset(T* p = 0) 
			{
				if (px == p)
				{
					return;
				}

				if(--(*pn) == SHARED_PTR_ZERO)
				{
					check_delete(px);
				}
				else 
				{
					try
					{
						pn = new long();
					}
					catch (...)
					{
						++(*pn);
						check_delete(p);
						throw;
					}
				}

				*pn = SHARED_PTR_ZERO + 1;
				px = p;
			}

			T* get() const
			{
				return px;
			}

			long use_count() const
			{
				return *pn;
			}

			bool unique() const{
				return *pn == (SHARED_PTR_ZERO + 1);
			}

			void swap(shared_ptr<T>& other)
			{
				std::swap(px, other.px);
				std::swap(pn, other.pn);
			}

			shared_ptr& clone(const shared_ptr<T>& other)
			{
				if (--(*pn) == SHARED_PTR_ZERO)
				{
					check_delete(px);
				}
				else
				{
					try
					{
						pn = new long();
					}
					catch (...)
					{
						++(*pn);
						throw;
					}
				}

				*pn = SHARED_PTR_ZERO + 1;
				px = new T;
				*px = *other.get();
			}

		private:
			void dispose() 
			{ 
				try
				{
					if(--(*pn) == SHARED_PTR_ZERO) 
					{
						check_delete(px);
						delete pn;
					}
				}
				catch(...)
				{
					
				}

			}

			void share(T* rpx, long* rpn) 
			{
				if (pn != rpn)
				{
					dispose();
					px = rpx;
					++(*(pn = rpn));
				}
			}

		private:
			T*		px;
			long*	pn;
		};

		template<typename T, typename U>
		inline bool operator==(const shared_ptr<T>& a, const shared_ptr<U>& b)
		{
			return a.get() == b.get();
		}

		template<typename T, typename U>
		inline bool operator!=(const shared_ptr<T>& a, const shared_ptr<U>& b)
		{
			return a.get() != b.get();
		}

		template<typename T, typename U>
		inline bool operator<(const shared_ptr<T>& a, const shared_ptr<U>& b)
		{
			return std::less<T*>(a.get(), b.get());
		}

		template<typename T>
		inline void swap(shared_ptr<T>& a, shared_ptr<T>& b)
		{
			a.swap(b);
		}

	}	// end of namespace cpp
} // end of namespace ext
