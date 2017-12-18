#pragma once

#ifdef COMMONLIB_MODULE
#define COMMONLIBDLLIMPEXP __declspec(dllexport)
#else
#define COMMONLIBDLLIMPEXP __declspec(dllimport)
#endif

#include "TreeNode.h"
namespace Ext
{

	namespace Collection
	{
		class __declspec(dllexport) EIterOrder
		{
		public:
			enum Enums
			{ 
				/// <summary>
				/// 先序遍历
				/// </summary>
				PreOrder,

				/// <summary>
				/// 中序遍历，即升序遍历
				/// </summary>
				InOrder,

				/// <summary>
				/// 后序遍历
				/// </summary>
				PostOrder,

				/// <summary>
				/// 升序遍历
				/// </summary>
				ASC,

				/// <summary>
				/// 降序遍历
				/// </summary>
				DSC
			};
		};

		template<typename K, typename V>
		class __declspec(dllexport) CTreeIter
		{
		private:
			CTreeNode<K, V> *m_pRoot;
			CTreeNode<K, V> *m_pCurrent;
			CTreeNode<K, V> *m_pNext;
			EIterOrder::Enums m_Order;

		public:
			CTreeIter(const CTreeIter& src)
			{
				m_pRoot = src.m_pRoot;
				m_pCurrent = src.m_pCurrent;
				m_pNext = src.m_pNext;
				m_Order = src.m_Order;
			}

			CTreeIter(CTreeNode<K, V> *pStartNode, EIterOrder::Enums order)
			{
				m_pRoot = pStartNode;
				m_pCurrent = NULL;
				m_pNext = NULL;
				m_Order = order;
			}

			const K& GetKey() { return m_pCurrent->GetKey(); }
			V& GetValue() { return m_pCurrent->GetValue(); }
			CTreeNode<K, V>* GetCurrentNode() { return m_pCurrent; }

			void Start()
			{
				if (m_Order == EIterOrder::ASC || m_Order == EIterOrder::InOrder)
					this->StartForASC();
				else if (m_Order == EIterOrder::DSC)
					this->StartForDSC();
				else if (m_Order == EIterOrder::PostOrder)
					this->StartForPostOrder();
				else if (m_Order == EIterOrder::PreOrder)
					this->StartForPreOrder();
			}

			bool Step()
			{
				if (m_Order == EIterOrder::ASC || m_Order == EIterOrder::InOrder)
					return this->StepForASC();
				else if (m_Order == EIterOrder::DSC)
					return this->StepForDSC();
				else if (m_Order == EIterOrder::PostOrder)
					return this->StepForPostOrder();
				else if (m_Order == EIterOrder::PreOrder)
					return this->StepForPreOrder();
			}

			bool Keepup()
			{
				m_pCurrent = m_pNext;
				if (m_pCurrent == NULL)
					return false;

				return true;
			}

		private:

			void StartForASC()
			{
				CTreeNode<K, V> *pMin = NULL;
				CTreeNode<K, V> *pNode = m_pRoot;
				while (pNode != NULL && pNode->IsNull() == false)
				{
					pMin = pNode;
					pNode = pNode->GetLeft();
				}

				m_pNext = pMin;
			}

			bool StepForASC()
			{
				m_pNext = NULL;
				if (m_pCurrent->GetRight()->IsNull() == false)
				{
					// 右子树存在，找到右子树中的最小节点
					CTreeNode<K, V> *pMin = NULL;
					CTreeNode<K, V> *pNode = m_pCurrent->GetRight();
					while (pNode->IsNull() == false)
					{
						pMin = pNode;
						pNode = pNode->GetLeft();
					}
					m_pNext = pMin;
					return true;
				}

				if (m_pCurrent->GetRight()->IsNull() == true && m_pCurrent->IsLeftChild())
				{
					m_pNext = m_pCurrent->GetParent();
					return true;
				}

				if (m_pCurrent->GetRight()->IsNull() == true && m_pCurrent->IsRightChild())
				{
					CTreeNode<K, V> *pUp = m_pCurrent->GetParent();
					while (pUp->IsNull() == false)
					{
						if (pUp->IsLeftChild() == true)
						{
							m_pNext = pUp->GetParent();
							return true;
						}
						pUp = pUp->GetParent();
					}
				}

				return true;
			}

			void StartForDSC()
			{
				CTreeNode<K, V> *pMax = NULL;
				CTreeNode<K, V> *pNode = m_pRoot;
				while (pNode != NULL && pNode->IsNull() == false)
				{
					pMax = pNode;
					pNode = pNode->GetRight();
				}

				m_pNext = pMax;
			}

			bool StepForDSC()
			{
				m_pNext = NULL;
				if (m_pCurrent->GetLeft()->IsNull() == false)
				{
					// 左子树存在，找到左子树中的最大节点
					CTreeNode<K, V> *pMax = NULL;
					CTreeNode<K, V> *pNode = m_pCurrent->GetLeft();
					while (pNode->IsNull() == false)
					{
						pMax = pNode;
						pNode = pNode->GetRight();
					}
					m_pNext = pMax;
					return true;
				}

				if (m_pCurrent->GetLeft()->IsNull() == true && m_pCurrent->IsRightChild())
				{
					m_pNext = m_pCurrent->GetParent();
					return true;
				}

				if (m_pCurrent->GetLeft()->IsNull() == true && m_pCurrent->IsLeftChild())
				{
					CTreeNode<K, V> *pUp = m_pCurrent->GetParent();
					while (pUp->IsNull() == false)
					{
						if (pUp->IsRightChild() == true)
						{
							m_pNext = pUp->GetParent();
							return true;
						}
						pUp = pUp->GetParent();
					}
				}

				return true;
			}

			void StartForPreOrder()
			{
				m_pNext = m_pRoot;
			}

			bool StepForPreOrder()
			{
				m_pNext = NULL;
				if (m_pCurrent->GetLeft()->IsNull() == false)
				{
					m_pNext = m_pCurrent->GetLeft();
					return true;
				}

				if (m_pCurrent->GetRight()->IsNull() == false)
				{
					m_pNext = m_pCurrent->GetRight();
					return true;
				}

				// 叶子节点，需要向上回溯
				CTreeNode<K, V> *pTemp = m_pCurrent;
				CTreeNode<K, V> *pUp = m_pCurrent->GetParent();
				while (pUp->IsNull() == false)
				{
					if (pTemp->IsLeftChild() == true)
					{
						m_pNext = pUp->GetRight();
						return true;
					}
					pTemp = pUp;
					pUp = pUp->GetParent();
				}

				return true;
			}

			void StartForPostOrder()
			{
				CTreeNode<K, V> *pLow = NULL;
				CTreeNode<K, V> *pNode = m_pRoot;
				while (pNode != NULL && pNode->IsNull() == false)
				{
					pLow = pNode;
					pNode = pLow->GetLeft();
					
					if (pNode->IsNull() == true)
						pNode = pLow->GetRight();
				}

				m_pNext = pLow;
			}

			bool StepForPostOrder()
			{
				m_pNext = NULL;
				if (m_pCurrent->IsLeftChild() == true && m_pCurrent->GetParent()->GetRight()->IsNull() == false)
				{
					// 右兄弟节点存在，找到右兄弟子树中的最低节点（左叶子优先）
					CTreeNode<K, V> *pLow = NULL;
					CTreeNode<K, V> *pNode = m_pCurrent->GetParent()->GetRight();
					while (pNode->IsNull() == false)
					{
						pLow = pNode;
						pNode = pLow->GetLeft();

						if (pNode->IsNull() == true)
							pNode = pLow->GetRight();
					}
					m_pNext = pLow;
					return true;
				}

				if (m_pCurrent->IsLeftChild() == true && m_pCurrent->GetParent()->GetRight()->IsNull() == true)
				{
					m_pNext = m_pCurrent->GetParent();
					return true;
				}

				if (m_pCurrent->IsRightChild())
				{
					m_pNext = m_pCurrent->GetParent();
					return true;
				}

				return true;
			}
		};
	} // End namespace Collection
}// End namespace Ext