#ifndef _EVENT_H_
#define _EVENT_H_

#include "HandleBase.h"
#include "PtrArray.h"

#include <typeinfo.h>

namespace Ext
{

	namespace CPP
	{
		class EventArg
		{
		public:
			void* Sender; // 事件的触发者
			void* Attach; // 事件的附加数据

		public:
			EventArg()
			{
				Sender = NULL;
				Attach = NULL;
			}

			EventArg(void* sender, void* data)
			{
				Sender = sender;
				Attach = data;
			}

			// 指针类型转换。调用：CSender *pSender = pArg->GetAttach<CSender*>();	// pArg 是EventArg类型的指针
			template<typename T>
			T GetSender()
			{
				T pointer = static_cast<T>(Sender);
				return pointer;
			}

			// 指针类型转换。调用：CAdoRow *pRow = pArg->GetAttach<CAdoRow*>();	// pArg 是EventArg类型的指针，Attach 指向指针
			template<typename T>
			T GetAttach()
			{
				T pointer = static_cast<T>(Attach);
				return pointer;
			}

			// 指针的指针转换为指针类型。调用：CAdoRow *pRow = pArg->GetPPAttach<CAdoRow*>();	// pArg 是EventArg类型的指针，Attach 指向指针的指针
			template<typename T>
			T GetPPAttach()
			{
				void** pointer1 = (void**)Attach;
				void*  pointer2 = (void*)(*pointer1);
				T attach = static_cast<T>(pointer2);
				return attach;
			}

			// 值类型转换。调用：int data = pArg->GetAttachValue<int>();	// pArg 是EventArg类型的指针，Attach 指向指针值的指针
			template<typename T>
			T GetAttachValue()
			{
				T *pointer = static_cast<T*>(Attach);
				T &value = *pointer;
				return value;
			}
		};

		class Event;
		class EventHandle : public HandleBase<int, EventArg*>
		{
		public:
			EventHandle() : HandleBase<int, EventArg*>()
			{
				m_pFun = NULL;
			}

			// 拷贝构造函数，STL 容器需要该函数w
			EventHandle(const EventHandle &fun)
			{
				m_pFun = NULL;
				*this = fun;
			}

			// 全局函数或静态函数
			template <typename Fun>
			EventHandle(const Fun &fun) : HandleBase<int, EventArg*>(fun)
			{
			}

			// 成员函数
			template <typename PointerToObj, typename PointerToMemFun>
			EventHandle(const PointerToObj &pObj, const PointerToMemFun &pFun) : HandleBase<int, EventArg*>(pObj, pFun)
			{
			}

			virtual ~EventHandle()
			{
			}

			EventHandle& operator= (const EventHandle &fun)
			{
				HandleBase<int, EventArg*>::operator= (fun);			
				return *this;
			}

			bool operator== (const EventHandle &handler)
			{
				return HandleBase<int, EventArg*>::operator ==(handler);
			}
		};

		class OnAddRemoveHandle : public HandleBase<int, EventHandle>
		{
		public:
			OnAddRemoveHandle() : HandleBase<int, EventHandle>()
			{
				m_pFun = NULL;
			}

			// 拷贝构造函数，STL 容器需要该函数w
			OnAddRemoveHandle(const OnAddRemoveHandle &fun)
			{
				m_pFun = NULL;
				*this = fun;
			}

			// 全局函数或静态函数
			template <typename Fun>
			OnAddRemoveHandle(const Fun &fun) : HandleBase<int, EventHandle>(fun)
			{
			}

			// 成员函数
			template <typename PointerToObj, typename PointerToMemFun>
			OnAddRemoveHandle(const PointerToObj &pObj, const PointerToMemFun &pFun) : HandleBase<int, EventHandle>(pObj, pFun)
			{
			}

			virtual ~OnAddRemoveHandle()
			{
			}

			OnAddRemoveHandle& operator= (const OnAddRemoveHandle &fun)
			{
				HandleBase<int, EventHandle>::operator= (fun);
				return *this;
			}

			bool operator== (const OnAddRemoveHandle &handler)
			{
				return HandleBase<int, EventHandle>::operator ==(handler);
			}
		};

		class OnAddRemoveEvent
		{
		public:

			OnAddRemoveEvent()
			{
				m_Handlers.Empty();
			}

			virtual ~OnAddRemoveEvent()
			{
				this->ClearHanlders();
			}

			void Raise(EventHandle &handler)
			{
				int count = m_Handlers.GetSize();
				for (int i = 0; i < count; ++i)
				{
					LPVOID pointer = m_Handlers.GetAt(i);
					OnAddRemoveHandle *pHandle = static_cast<OnAddRemoveHandle*>(pointer);
					pHandle->Invoke(handler);
				}
			}

			OnAddRemoveEvent& operator+= (OnAddRemoveHandle &handler)
			{
				AddHandle(handler);
				return *this;
			}

			OnAddRemoveEvent& operator-= (OnAddRemoveHandle &handler)
			{
				RemoveHandle(handler);
				return *this;
			}

			void AddHandle(OnAddRemoveHandle &handler)
			{
				OnAddRemoveHandle *pHandle = new OnAddRemoveHandle(handler);
				m_Handlers.Add(pHandle);
			}

			void RemoveHandle(OnAddRemoveHandle &handler)
			{
				int count = m_Handlers.GetSize();
				for (int i = 0; i < count; ++i)
				{
					LPVOID pointer = m_Handlers.GetAt(i);
					OnAddRemoveHandle *pHandle = static_cast<OnAddRemoveHandle*>(pointer);
					if (*pHandle == handler)
					{
						m_Handlers.Remove(i);
						return;
					}
				}
			}

			void ClearHanlders()
			{
				int count = m_Handlers.GetSize();
				for (int i = 0; i < count; ++i)
				{
					LPVOID pointer = m_Handlers.GetAt(i);
					OnAddRemoveHandle *pHandle = static_cast<OnAddRemoveHandle*>(pointer);
					delete pHandle;
				}
				m_Handlers.Empty();
			}

		protected:

			// 将侦听的所有函数或者仿函数的集合起来
			Ext::CStdPtrArray m_Handlers;
		};

		class Event
		{
		public:

			Event()
			{
				m_Handlers.Empty();
			}

			Event(const Event &rSource)
			{
				*this = rSource;
			}

			Event& operator= (const Event &rSource)
			{
				if (this == &rSource)
					return *this;

				m_Handlers.Empty();
				OnAddHandler.ClearHanlders();
				OnRemoveHandler.ClearHanlders();

				return *this;
			}

			virtual ~Event()
			{
				this->ClearHanlders();
			}

			void Raise(EventArg *pArg)
			{
				int count = m_Handlers.GetSize();
				for (int i = 0; i < count; ++i)
				{
					LPVOID pointer = m_Handlers.GetAt(i);
					EventHandle *pHandle = static_cast<EventHandle*>(pointer);
					pHandle->Invoke(pArg);
				}
			}

			Event& operator+= (EventHandle &handler)
			{
				AddHandle(handler);
				return *this;
			}

			Event& operator-= (EventHandle &handler)
			{
				RemoveHandle(handler);
				return *this;
			}

			void AddHandle(EventHandle &handler)
			{
				if (handler.IsValid() == false)
					return;

				OnAddHandler.Raise(handler);
				EventHandle *pHandle = new EventHandle(handler);
				m_Handlers.Add(pHandle);
			}

			void RemoveHandle(EventHandle &handler)
			{
				if (handler.IsValid() == false)
					return;

				OnRemoveHandler.Raise(handler);

				int count = m_Handlers.GetSize();
				for (int i = 0; i < count; ++i)
				{
					LPVOID pointer = m_Handlers.GetAt(i);
					EventHandle *pHandle = static_cast<EventHandle*>(pointer);
					if (*pHandle == handler)
					{
						m_Handlers.Remove(i);
						return;
					}
				}
			}

			const CStdPtrArray& GetHandlers()
			{
				return this->m_Handlers;
			}

			void ClearHanlders()
			{
				int count = m_Handlers.GetSize();
				for (int i = 0; i < count; ++i)
				{
					LPVOID pointer = m_Handlers.GetAt(i);
					EventHandle *pHandle = static_cast<EventHandle*>(pointer);
					delete pHandle;
				}
				m_Handlers.Empty();
			}

		public:
			OnAddRemoveEvent OnAddHandler;
			OnAddRemoveEvent OnRemoveHandler;

		protected:

			// 将侦听的所有函数或者仿函数的集合起来
			Ext::CStdPtrArray m_Handlers;
		};
	} // End namespace CPP
} // End namesapce Ext

#endif // _EVENT_H_