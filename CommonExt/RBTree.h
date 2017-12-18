#pragma once

#ifdef COMMONLIB_MODULE
#define COMMONLIBDLLIMPEXP __declspec(dllexport)
#else
#define COMMONLIBDLLIMPEXP __declspec(dllimport)
#endif

#include "TreeNode.h"
#include "TreeIter.h"

namespace Ext
{

	namespace Collection
	{
		// 在一条路径上的黑节点的数目包括应当包括绿色节点的数目，这里的绿色用于标识 NullNode
		enum EColor 
		{ 
			Red		= 1,
			Black	= 2,
			Green	= 4,
			Blue	= 8,
			BlackGreen = 6,		// Black | Green
			BlackBlue  = 10,	// Black | Blue
		};

		template<typename K, typename V>
		class __declspec(dllexport) CRBNode : public CTreeNode<K, V>
		{
		public:  
			CRBNode()  
			{  
				//m_Color = Black;
				m_pRight = NULL;  
				m_pLeft = NULL;  
				m_pParent = NULL;  
			}  
			K m_Key;
			V m_Value;

			CRBNode* m_pParent;
			CRBNode* m_pLeft;
			CRBNode* m_pRight;
			EColor m_Color;

			virtual const K& GetKey() { return m_Key; }
			virtual V& GetValue() { return m_Value; }
			virtual CTreeNode<K, V>* GetParent() { return m_pParent; }
			virtual CTreeNode<K, V>* GetLeft() { return m_pLeft; }
			virtual CTreeNode<K, V>* GetRight() { return m_pRight; }
			virtual bool IsNull() { return m_Color == BlackGreen; }
		};

		template<typename K, typename V>
		class __declspec(dllexport) CRBTree
		{
		public:
			typedef int(*CompareFunc)(const K &a, const K &b);

		public:
			CRBTree(const CRBTree& input)
			{
				this->Initial();
				this->m_pComparor = input.m_pComparor;
				*this = input;
			}

			const CRBTree& operator= (const CRBTree& input)
			{
				if (!this->IsEmpty())
					this->Clear();

				CTreeIter<K, V> iter = input.Iter(EIterOrder::PostOrder);
				for (iter.Start(); iter.Keepup(); iter.Step())
					this->Insert(iter.GetKey(), iter.GetValue());

				return *this;
			}

		protected:
			CRBNode<K, V> *NullNode; // 哨兵节点，表示空节点或叶子节点
			CRBNode<K, V> *m_pRoot;
			CompareFunc m_pComparor;
			long m_NodeCount;

		public:  
			CRBTree()  
			{  
				this->Initial();
				this->m_pComparor = Ext::Collection::CompareAB;
			}

			CRBTree(CompareFunc pComparor)  
			{  
				this->Initial();
				this->m_pComparor = pComparor;
			}

			virtual ~CRBTree()  
			{  
				Destroy();
			}

			bool IsEmpty() const
			{  
				if(this->m_pRoot == NullNode)  
					return true;
				else  
					return false;
			}

			void AsEmpty() const
			{
				this->m_pRoot = NullNode;
			}

			long GetNodeCount() const
			{
				return this->m_NodeCount;
			}

			bool Find(K key, V &outputValue) const
			{
				CRBNode<K, V> *pNode = this->FindNode(key);
				if (pNode == NullNode)
					return false;

				outputValue = pNode->m_Value;
				return true;
			}

			bool Exits(K key) const
			{
				CRBNode<K, V> *pNode = this->FindNode(key);
				if (pNode == NullNode)
					return false;

				return true;
			}

			// 使用键索引获取关联的值，当键不存在时，将插入新键与关联的默认值
			// 若要确保不插入新键，可使用 Exits(...)、Find(...) 查询键是否存在
			V& operator[](const K& key)
			{
				CRBNode<K, V> *pNode = this->FindNode(key);
				if (pNode != NullNode)
					return pNode->m_Value;

				V defaultVal;
				this->Insert(key, defaultVal);

				pNode = this->FindNode(key);
				return pNode->m_Value;
			}

			virtual void Insert(const K &key, const V &val) 
			{
				this->InsertNode(key, val);
			}

			virtual bool Delete(const K &key)
			{  
				CRBNode<K, V> *pNode = FindNode(key);
				if (pNode == NullNode)
					return false;

				if (pNode->m_pLeft != NullNode && pNode->m_pRight != NullNode)  
				{  
					CRBNode<K, V> *successor = InOrderSuccessor(pNode);  
					pNode->m_Value = successor->m_Value;  
					pNode->m_Key = successor->m_Key;  
					pNode = successor;
				}

				CRBNode<K, V> *pChild;  
				if (pNode->m_pRight != NullNode)  
				{  
					pChild = pNode->m_pRight;  
				}  
				else if (pNode->m_pLeft != NullNode)  
				{  
					pChild = pNode->m_pLeft;  
				}  
				else  
				{  
					pChild = NullNode;  
				}  
				pChild->m_pParent = pNode->m_pParent;  
				if (pNode->m_pParent == NullNode) 
				{  
					m_pRoot = pChild;
					NullNode->m_pParent = m_pRoot;
					NullNode->m_pLeft = m_pRoot;
					NullNode->m_pRight = m_pRoot;
				}  
				else if (pNode == pNode->m_pParent->m_pRight)  
				{  
					pNode->m_pParent->m_pRight = pChild;  
				}  
				else  
				{  
					pNode->m_pParent->m_pLeft = pChild;  
				}  
				if (pNode->m_Color == Black && !(pChild == NullNode && pChild->m_pParent == NullNode))  
				{  
					DeleteFixUp(pChild);
				}

				pNode->m_pParent = NULL;
				pNode->m_pLeft = NULL;
				pNode->m_pRight = NULL;
				delete pNode;
				pNode = NULL;

				--m_NodeCount;
				return true;  
			}  

			virtual void DeleteFixUp(CRBNode<K, V> *pNode)  
			{
				while (pNode != m_pRoot && pNode->m_Color == Black)  
				{  
					if (pNode == pNode->m_pParent->m_pLeft)
					{  
						CRBNode<K, V> *brother = pNode->m_pParent->m_pRight;  
						if (brother->m_Color == Red)   //情况1：x的兄弟w是红色的。  
						{  
							brother->m_Color = Black;  
							pNode->m_pParent->m_Color = Red;  
							RotateLeft(pNode->m_pParent);  
						}  
						else      
						{  
							//情况2：x的兄弟w是黑色的，
							if ((brother->m_pLeft->m_Color & Black) != 0 && (brother->m_pRight->m_Color & Black) != 0)
							{  
								//且w的俩个孩子都是黑色的。
								brother->m_Color = Red;  
								pNode = pNode->m_pParent;  
							}  
							else if((brother->m_pRight->m_Color & Black) != 0)
							{  
								//情况3：x的兄弟w是黑色的，w的右孩子是黑色（w的左孩子是红色）。  
								brother->m_Color = Red;  
								brother->m_pLeft->m_Color = Black;  
								RotateRight(brother);
							}  
							else if(brother->m_pRight->m_Color == Red)  
							{  
								//情况4：x的兄弟w是黑色的，且w的右孩子时红色的。  
								brother->m_Color = pNode->m_pParent->m_Color;  
								pNode->m_pParent->m_Color = Black;  
								brother->m_pRight->m_Color = Black;  
								RotateLeft(pNode->m_pParent);  
								pNode = m_pRoot;  
							}  
						}  
					}  
					else  
					{  
						//下述情况针对上面的情况1中，node作为右孩子而阐述的。  
						//22        else (same as then clause with "m_pRight" and "m_pLeft" exchanged)  
						//同样，原理一致，只是遇到左旋改为右旋，遇到右旋改为左旋，即可。其它代码不变。  
						CRBNode<K, V>* brother = pNode->m_pParent->m_pLeft;  
						if (brother->m_Color == Red)  
						{  
							brother->m_Color = Black;  
							pNode->m_pParent->m_Color = Red;  
							RotateRight(pNode->m_pParent);  
						}  
						else  
						{  
							if ((brother->m_pLeft->m_Color & Black) != 0 && (brother->m_pRight->m_Color & Black) != 0)
							{  
								brother->m_Color = Red;  
								pNode = pNode->m_pParent;  
							}  
							else if ((brother->m_pLeft->m_Color & Black) != 0)
							{  
								brother->m_Color = Red;  
								brother->m_pRight->m_Color = Black;  
								RotateLeft(brother);  
							}  
							else if (brother->m_pLeft->m_Color == Red)  
							{  
								brother->m_Color = pNode->m_pParent->m_Color;  
								pNode->m_pParent->m_Color = Black;  
								brother->m_pLeft->m_Color = Black;  
								RotateRight(pNode->m_pParent);  
								pNode = m_pRoot;  
							}  
						}  
					}  
				}  
				NullNode->m_pParent = m_pRoot;   //最后将node置为根结点，  
				pNode->m_Color = Black;    //并改为黑色

				if (pNode == NullNode)
					pNode->m_Color = BlackGreen;
			}  

			//左旋代码实现  
			CRBNode<K, V>* RotateLeft(CRBNode<K, V> *pNode)  
			{  
				if (pNode == NullNode || pNode->m_pRight == NullNode)    
					return pNode;

				CRBNode<K, V>* lowerRight = pNode->m_pRight;  
				lowerRight->m_pParent =  pNode->m_pParent;  
				pNode->m_pRight = lowerRight->m_pLeft;  
				if (lowerRight->m_pLeft != NullNode)  
				{  
					lowerRight->m_pLeft->m_pParent = pNode;  
				}  
				if (pNode->m_pParent == NullNode) //pNode是根节点
				{  
					m_pRoot = lowerRight;
					this->ResetNullNode(m_pRoot);
				}  
				else  
				{  
					if (pNode->IsLeftChild() == true)
						pNode->m_pParent->m_pLeft = lowerRight;  
					else  
						pNode->m_pParent->m_pRight = lowerRight;
				}  
				pNode->m_pParent = lowerRight;  
				lowerRight->m_pLeft = pNode;

				return lowerRight;
			}  

			//右旋代码实现  
			CRBNode<K, V>* RotateRight(CRBNode<K, V> *pNode)  
			{  
				if (pNode == NullNode || pNode->m_pLeft == NullNode)
					return pNode;

				CRBNode<K, V> *lowerLeft = pNode->m_pLeft;  
				pNode->m_pLeft = lowerLeft->m_pRight;  
				lowerLeft->m_pParent = pNode->m_pParent;  
				if (lowerLeft->m_pRight != NullNode)
					lowerLeft->m_pRight->m_pParent = pNode;  

				if (pNode->m_pParent == NullNode) //pNode是根节点
				{  
					m_pRoot = lowerLeft;
					this->ResetNullNode(m_pRoot);
				}  
				else  
				{  
					if (pNode->IsLeftChild() == true)
						pNode->m_pParent->m_pLeft = lowerLeft;
					else
						pNode->m_pParent->m_pRight = lowerLeft;
				}  
				pNode->m_pParent = lowerLeft;  
				lowerLeft->m_pRight = pNode;

				return lowerLeft;
			}

			//  
			inline CRBNode<K, V>* Predecessor(CRBNode<K, V> *pNode)  
			{  
				if (pNode == NullNode)
					return NullNode;  

				CRBNode<K, V> *result = pNode->m_pLeft;      //当pNode节点有右孩子时，返回右子树中最大的节点 
				while (result != NullNode)
				{  
					if (result->m_pRight != NullNode)
						result = result->m_pRight;			
					else
						break;  
				}

				if (result == NullNode)  
				{
					// 若pNode的左子树为空，一直往上，直到第一次出现一个节点（p）是其父节点的右节点，则p.Parent就是pNode的前继节点
					CRBNode<K, V>* index = pNode->m_pParent;  
					result = pNode;  
					while (index != NullNode && result == index->m_pLeft)  
					{
						result = index;
						index = index->m_pParent;
					}
					result = index;
				}  
				return result;  
			}  

			inline CRBNode<K, V>* InOrderSuccessor(CRBNode<K, V> *pNode)
			{  
				if (pNode == NullNode)
					return NullNode;

				CRBNode<K, V> *result = pNode->m_pRight;   //当pNode节点有右孩子时，返回右子树中最小的节点  
				while (result != NullNode)
				{  
					if (result->m_pLeft != NullNode)
						result = result->m_pLeft;
					else
						break;
				}

				if (result == NullNode)  
				{  
					// 若pNode的右子树为空，一直往上，直到第一次出现一个节点（p）是其父节点的左节点，则p.Parent就是pNode的后继节点
					CRBNode<K, V> *parent = pNode->m_pParent;
					result = pNode;  
					while (parent != NullNode && result == parent->m_pRight)  
					{  
						result = parent;  
						parent = parent->m_pParent;  
					}  
					result = parent;
				}  
				return result;  
			}

			CTreeIter<K, V> Iter(EIterOrder::Enums order) const
			{
				//CTreeIter<K, V> iter(this->m_pRoot, order);
				//return iter;
				return this->Iter(this->m_pRoot, order);
			}

			// 清空所有的节点，当前红黑树还可以继续添加新的节点
			virtual void Clear()
			{
				this->m_NodeCount = 0;
				CRBNode<K, V> *pTemp = NullNode;

				CTreeIter<K, V> iter = this->Iter(EIterOrder::PostOrder);
				for (iter.Start(); iter.Keepup(); iter.Step())
				{
					if (pTemp != NullNode)
					{
						pTemp->m_pParent = NULL;
						pTemp->m_pLeft = NULL;
						pTemp->m_pRight = NULL;
						delete pTemp;
						pTemp = NULL;
					}

					CRBNode<K, V> *pNode = (CRBNode<K, V>*)iter.GetCurrentNode();
					pTemp = pNode;
				}

				if (pTemp != NullNode)
				{
					pTemp->m_pParent = NULL;
					pTemp->m_pLeft = NULL;
					pTemp->m_pRight = NULL;
					delete pTemp;
					pTemp = NULL;
				}

				this->m_pRoot = NullNode;
				this->ResetNullNode(m_pRoot);
			}

			// 销毁当前红黑树后，不能再执行添加或删除节点操作
			virtual void Destroy()
			{
				if (this->m_pRoot == NULL)
					return;

				this->Clear();

				// Clear() 之后，m_pRoot 已经指向 NullNode，故简单地设置 m_pRoot 为 NULL 即可
				this->m_pRoot = NULL;

				if (NullNode != NULL)
				{
					NullNode->m_pParent = NULL;
					NullNode->m_pLeft = NULL;
					NullNode->m_pRight = NULL;
					delete NullNode;
					NullNode = NULL;
				}
			}

		protected:
			//查找key  
			CRBNode<K, V>* FindNode(K key) const
			{
				if (m_pRoot == NullNode)
					return NullNode;

				CRBNode<K, V> *pNode = m_pRoot;
				int flag = 0;
				while (pNode != NullNode)
				{
					flag = m_pComparor(key, pNode->m_Key);
					if (flag < 0)     
					{  
						pNode  = pNode->m_pLeft;  //比当前的小，往左  
					}  
					else if (flag > 0)  
					{  
						pNode = pNode->m_pRight;  //比当前的大，往右  
					}  
					else  
					{  
						break;  
					}  
				}  
				return pNode; 
			}

			void Initial()
			{
				this->NullNode = new CRBNode<K, V>();
				this->NullNode->m_Color = BlackGreen;

				this->m_pRoot = NullNode;
				this->ResetNullNode(m_pRoot);
				
				this->m_NodeCount = 0;
			}

			void ResetNullNode(CRBNode<K, V> *pRoot)
			{
				// NullNode 的父节点、左右子节点永远指向根节点。可用此特征判断一个节点是否为 NullNode 节点
				NullNode->m_pParent = pRoot;
				NullNode->m_pLeft = pRoot;
				NullNode->m_pRight = pRoot;
			}

			CTreeIter<K, V> Iter(CRBNode<K, V> *pStartNode, EIterOrder::Enums order) const
			{
				CTreeIter<K, V> iter(pStartNode, order);
				return iter;
			}

			virtual CRBNode<K, V>* InsertNode(const K &key, const V &val)
			{  
				CRBNode<K, V> *newParent = NullNode;
				CRBNode<K, V> *pNode = m_pRoot;
				int flag = -1;
				while (pNode != NullNode)
				{  
					newParent = pNode;
					flag = m_pComparor(key, pNode->m_Key);
					if (flag < 0)
					{
						pNode = pNode->m_pLeft;  
					}
					else if(flag > 0)
					{  
						pNode = pNode->m_pRight; 
					}
					else  
					{
						pNode->m_Value = val;
						return NullNode;  
					}  
				}
				++this->m_NodeCount;
				CRBNode<K, V> *newNode = new CRBNode<K, V>();
				newNode->m_Key = key;
				newNode->m_Value = val;
				newNode->m_Color = Red;  
				newNode->m_pRight = NullNode;
				newNode->m_pLeft = NullNode;
				if (newParent == NullNode) //如果插入的是一颗空树
				{  
					m_pRoot = newNode;
					m_pRoot->m_pParent = NullNode;
					this->ResetNullNode(m_pRoot);
				}  
				else  
				{  
					if (flag < 0)
						newParent->m_pLeft = newNode;  
					else
						newParent->m_pRight = newNode;

					newNode->m_pParent = newParent;  
				}  
				InsertFixUp(newNode);    //调用InsertFixUp修复红黑树性质
				return newNode;
			}

			virtual void InsertFixUp(CRBNode<K, V> *pNode)  
			{
				while (pNode != NullNode && pNode != m_pRoot && pNode->m_pParent->m_Color == Red)
				{  
					if (pNode->m_pParent->IsLeftChild() == true)
					{  
						CRBNode<K, V> *uncle = pNode->m_pParent->m_pParent->m_pRight;  
						if (uncle != NullNode && uncle->m_Color == Red)  
						{ 
							//插入情况1，pNode的叔叔是红色
							pNode->m_pParent->m_Color = Black;
							uncle->m_Color = Black;
							pNode->m_pParent->m_pParent->m_Color = Red;
							pNode = pNode->m_pParent->m_pParent;
						}  
						else
						{  
							if (pNode->IsRightChild() == true)
							{  
								//插入情况2：pNode的叔叔是黑色或者pNode的叔叔节点不存在，且pNode是其父节点的右节点
								pNode = pNode->m_pParent;
								RotateLeft(pNode);
							}

							//插入情况3：pNode的叔叔是黑色或pNode的叔叔节点不存在，但pNode是其父节点的左节点
							pNode->m_pParent->m_Color = Black;  
							pNode->m_pParent->m_pParent->m_Color = Red;  
							RotateRight(pNode->m_pParent->m_pParent);  
						}  
					}  
					else 
					{  
						CRBNode<K, V> *uncle = pNode->m_pParent->m_pParent->m_pLeft;  
						if (uncle != NullNode && uncle->m_Color == Red)  
						{  
							pNode->m_pParent->m_Color = Black;  
							uncle->m_Color = Black;  
							uncle->m_pParent->m_Color = Red;  
							pNode = pNode->m_pParent->m_pParent;  
						}  
						else
						{  
							if (pNode->IsLeftChild() == true)
							{
								pNode = pNode->m_pParent;
								RotateRight(pNode);     //与上述代码相比，左旋改为右旋  
							}

							pNode->m_pParent->m_Color = Black;  
							pNode->m_pParent->m_pParent->m_Color = Red;  
							RotateLeft(pNode->m_pParent->m_pParent);   //右旋改为左旋
						}  
					}  
				}  
				m_pRoot->m_Color = Black;
			}
		};  
	} // End namespace Collection
}// End namespace Ext
