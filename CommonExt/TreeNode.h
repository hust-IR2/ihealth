#pragma once

#ifdef COMMONLIB_MODULE
#define COMMONLIBDLLIMPEXP __declspec(dllexport)
#else
#define COMMONLIBDLLIMPEXP __declspec(dllimport)
#endif

namespace Ext
{

	namespace Collection
	{
		template<typename K, typename V>
		class __declspec(dllexport) CTreeNode
		{
		public:
			virtual const K& GetKey() = 0;
			virtual V& GetValue() = 0;

			virtual CTreeNode* GetParent() = 0;
			virtual CTreeNode* GetLeft() = 0;
			virtual CTreeNode* GetRight() = 0;
			virtual bool IsNull() = 0;

			bool IsLeftChild()
			{
				CTreeNode *parent = this->GetParent();
				if (parent->IsNull() == true)
					return false;

				if (parent->GetLeft() == this)
					return true;
				return false;
			}

			bool IsRightChild()
			{
				CTreeNode *parent = this->GetParent();
				if (parent->IsNull() == true)
					return false;

				if (parent->GetRight() == this)
					return true;
				return false;
			}

			bool IsLeaf()
			{
				if (this->GetLeft()->IsNull() == true && this->GetRight()->IsNull() == true)
					return true;
				return false;
			}

			bool HasChild()
			{
				if (this->GetLeft()->IsNull() == false || this->GetRight()->IsNull() == false)
					return true;
				return false;
			}
		};
	} // End namespace Collection
}// End namespace Ext