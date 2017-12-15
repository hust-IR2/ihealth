#ifndef __DICT_H__
#define __DICT_H__

#include "TreeIter.h"
#include "RBTree.h"

namespace Ext
{
	namespace Collection
	{
		template<typename K, typename V>
		class __declspec(dllexport) DictIter : public CTreeIter<K, V>
		{
		public:
			DictIter(CTreeIter base) : CTreeIter(base)
			{
			}
		};


		template<typename K, typename V>
		class __declspec(dllexport) Dict : public CRBTree<K, V>
		{
		public:
			Dict() : CRBTree()
			{
			}

			Dict(CompareFunc pComparor) : CRBTree(pComparor)
			{
			}

			Dict(const Dict& src) : CRBTree(src)
			{
			}

			const Dict & operator= (const Dict &src)
			{
				return dynamic_cast<const Dict&>(CRBTree::operator=(src));
			}

			virtual ~Dict()  
			{
				CRBTree::~CRBTree();
			}

			DictIter<K, V> Iter(EIterOrder::Enums order) const
			{
				CTreeIter<K, V> iter = CRBTree::Iter(CRBTree::m_pRoot, order);
				DictIter<K, V> diter(iter);
				return diter;
			}
		};
	}//End namespace Collection
}//End namespace Ext

#endif  // __DICT_H__