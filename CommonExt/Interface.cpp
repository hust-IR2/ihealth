#include "StdAfx.h"
#include "Interface.h"
#include "List.h"
#include "RBTree.h"
#include "shared_ptr.h"

using namespace Ext::Collection;

namespace Ext
{
	namespace CPP
	{
		Interface::Interface(void)
		{
			m_pIter = NULL;
			m_pData = NULL;
			m_iType = NullType;
		}

		Interface::~Interface(void)
		{
			Clear();
			m_pData = NULL;
			m_iType = NullType;
		}

		Interface::Interface(const Interface& val)
		{
			m_pIter = NULL;
			m_pData = NULL;
			*this = val;
		}

		Interface::Interface(const bool& val)
		{
			m_pIter = NULL;
			m_pData = NULL;
			*this = val;
		}

		Interface::Interface(const int& val)
		{
			m_pIter = NULL;
			m_pData = NULL;
			*this = val;
		}

		Interface::Interface(const UINT64& val)
		{
			m_pIter = NULL;
			m_pData = NULL;
			*this = val;
		}

		Interface::Interface(const long& val)
		{
			m_pIter = NULL;
			m_pData = NULL;
			*this = val;
		}

		Interface::Interface(const double& val)
		{
			m_pIter = NULL;
			m_pData = NULL;
			*this = val;
		}

		Interface::Interface(const TCHAR* val)
		{
			m_pIter = NULL;
			m_pData = NULL;
			*this = val;
		}

		Interface::Interface(const CString& val)
		{
			m_pIter = NULL;
			m_pData = NULL;
			*this = val;
		}

		Interface& Interface::operator=(const Interface& val)
		{
			m_iType = val.m_iType;

			switch(m_iType)
			{
			case BoolType:
				{
					m_pData = new shared_ptr<bool>;
					*((shared_ptr<bool>*)m_pData) = *((shared_ptr<bool>* const)(val.m_pData));
				}
				break;
			case IntType:
				{
					m_pData = new shared_ptr<int>;
					*((shared_ptr<int>*)m_pData) = *((shared_ptr<int>* const)(val.m_pData));
				}
				break;
			case Uint64Type:
				{
					m_pData = new shared_ptr<UINT64>;
					*((shared_ptr<UINT64>*)m_pData) = *((shared_ptr<UINT64>* const)(val.m_pData));
				}
				break;
			case LongType:
				{
					m_pData = new shared_ptr<long>;
					*((shared_ptr<long>*)m_pData) = *((shared_ptr<long>* const)(val.m_pData));
				}
				break;
			case DoubleType:
				{
					m_pData = new shared_ptr<double>;
					*((shared_ptr<double>*)m_pData) = *((shared_ptr<double>* const)(val.m_pData));
				}
				break;
			case StringType:
				{
					m_pData = new shared_ptr<CString>;
					*((shared_ptr<CString>*)m_pData) = *((shared_ptr<CString>* const)(val.m_pData));
				}
				break;
			case ArrayType:
				{
					m_pData = new shared_ptr<List<Interface>>;
					*((shared_ptr<List<Interface>>*)m_pData) = *((shared_ptr<List<Interface>>* const)(val.m_pData));
				}
				break;
			case ObjectType:
				{
					m_pData = new shared_ptr<CRBTree<CString, Interface>>;
					*((shared_ptr<CRBTree<CString, Interface>>*)m_pData) = *((shared_ptr<CRBTree<CString, Interface>>* const)(val.m_pData));
				}	
				break;
			default:
				break;
			}
			
			return *this;
		}

		Interface& Interface::operator=(const bool& val)
		{
			SetBoolVal(val);

			return *this;
		}

		Interface& Interface::operator=(const int& val)
		{
			SetIntVal(val);

			return *this;
		}

		Interface& Interface::operator=(const UINT64& val)
		{
			SetUint64Val(val);

			return *this;
		}

		Interface& Interface::operator=(const long& val)
		{
			SetLongVal(val);

			return *this;
		}

		Interface& Interface::operator=(const double& val)
		{
			SetDoubleVal(val);

			return *this;
		}

		Interface& Interface::operator=(const TCHAR* val)
		{
			SetStringVal(val);

			return *this;
		}

		Interface& Interface::operator=(const CString& val)
		{
			SetStringVal(val);

			return *this;
		}

		Interface::operator bool&() const
		{
			return GetBoolVal();
		}

		Interface::operator int&() const
		{
			return GetIntVal();
		}

		Interface::operator UINT64&() const
		{
			return GetUint64Val();
		}

		Interface::operator long&() const
		{
			return GetLongVal();
		}

		Interface::operator double&() const
		{
			return GetDoubleVal();
		}

		Interface::operator CString&() const
		{
			return GetStringVal();
		}

		bool& Interface::GetBoolVal() const
		{
			return *((shared_ptr<bool>*)m_pData)->get();
		}

		int& Interface::GetIntVal() const
		{
			if (m_iType == LongType)
			{
				long& t = GetLongVal();
				SetIntVal((int)t);
			}
			else if (m_iType == DoubleType)
			{
				double& t = GetDoubleVal();
				SetIntVal((int)t);
			}
			else if (m_iType == Uint64Type)
			{
				UINT64& t = GetUint64Val();
				SetIntVal((int)t);
			}

			return *((shared_ptr<int>*)m_pData)->get();
		}

		UINT64& Interface::GetUint64Val() const
		{
			if (m_iType == LongType)
			{
				long& t =GetLongVal();
				SetUint64Val((UINT64)t);
			}
			else if (m_iType == DoubleType)
			{
				double& t = GetDoubleVal();
				SetUint64Val((UINT64)t);
			}
			else if (m_iType == IntType)
			{
				int& t = GetIntVal();
				SetUint64Val((UINT64)t);
			}

			return *((shared_ptr<UINT64>*)m_pData)->get();
		}

		long& Interface::GetLongVal() const
		{
			if (m_iType == Uint64Type)
			{
				UINT64& t = GetUint64Val();
				SetLongVal((long)t);
			}
			else if (m_iType == DoubleType)
			{
				double& t = GetDoubleVal();
				SetLongVal((long)t);
			}
			else if (m_iType == IntType)
			{
				int& t = GetIntVal();
				SetLongVal((long)t);
			}

			return *((shared_ptr<long>*)m_pData)->get();
		}

		double& Interface::GetDoubleVal() const
		{
			if (m_iType == LongType)
			{
				long& t = GetLongVal();
				SetDoubleVal((double)t);
			}
			else if (m_iType == Uint64Type)
			{
				UINT64& t = GetUint64Val();
				SetDoubleVal((double)t);
			}
			else if (m_iType == IntType)
			{
				int& t = GetIntVal();
				SetDoubleVal((double)t);
			}


			return *((shared_ptr<double>*)m_pData)->get();
		}

		CString& Interface::GetStringVal() const
		{
			return *((shared_ptr<CString>*)m_pData)->get();
		}

		void Interface::SetBoolVal(const bool& val) const
		{
			Clear();

			shared_ptr<bool> *p = new shared_ptr<bool>;
			bool* pVal = new bool;
			*pVal = val;
			p->reset(pVal);
	
			m_pData = p;

			m_iType = BoolType;
		}

		void Interface::SetIntVal(const int& val) const
		{
			Clear();

			shared_ptr<int> *p = new shared_ptr<int>;
			int *pVal = new int;
			*pVal = val;	
			p->reset(pVal);

			m_pData = p;

			m_iType = IntType;
		}

		void Interface::SetUint64Val(const UINT64& val) const
		{
			Clear();

			shared_ptr<UINT64> *p = new shared_ptr<UINT64>;
			UINT64 *pVal = new UINT64;
			*pVal = val;
			p->reset(pVal);

			m_pData = p;

			m_iType = Uint64Type;
		}

		void Interface::SetLongVal(const long& val) const
		{
			Clear();

			shared_ptr<long> *p = new shared_ptr<long>;
			long *pVal = new long;
			*pVal = val;
			p->reset(pVal);

			m_pData = p;

			m_iType = LongType;
		}

		void Interface::SetDoubleVal(const double& val) const
		{
			Clear();

			shared_ptr<double> *p = new shared_ptr<double>;
			double *pVal = new double;
			*pVal = val;
			p->reset(pVal);

			m_pData = p;

			m_iType = DoubleType;
		}

		void Interface::SetStringVal(const CString& val) const
		{
			Clear();

			shared_ptr<CString> *p = new shared_ptr<CString>;
			CString *pVal = new CString;
			*pVal = val;
			p->reset(pVal);

			m_pData = p;

			m_iType = StringType;
		}

		void Interface::Insert(const CString& k, const Interface& v) const
		{
			shared_ptr<CRBTree<CString, Interface>>* p = NULL;
			if (m_iType != ObjectType || m_pData == NULL)
			{
				Clear();

				shared_ptr<CRBTree<CString, Interface>>* p = new shared_ptr<CRBTree<CString, Interface>>;
				CRBTree<CString, Interface>* pVal = new CRBTree<CString, Interface>;
				p->reset(pVal);
				m_pData = p;
				m_iType = ObjectType;
			}
			p = (shared_ptr<CRBTree<CString, Interface>>*)m_pData;
			(*p)->Insert(k, v);
		}

		void Interface::Delete(const CString& k) const
		{
			shared_ptr<CRBTree<CString, Interface>>* p = NULL;
			if (m_iType != ObjectType || m_pData == NULL)
			{
				Clear();

				shared_ptr<CRBTree<CString, Interface>>* p = new shared_ptr<CRBTree<CString, Interface>>;
				CRBTree<CString, Interface>* pVal = new CRBTree<CString, Interface>;
				p->reset(pVal);
				m_pData = p;
				m_iType = ObjectType;
			}
			p = (shared_ptr<CRBTree<CString, Interface>>*)m_pData;
			(*p)->Delete(k);
		}

		Interface& Interface::Find(const CString& k) const
		{
			shared_ptr<CRBTree<CString, Interface>>* p = NULL;
			if (m_iType != ObjectType || m_pData == NULL)
			{
				Clear();

				shared_ptr<CRBTree<CString, Interface>>* p = new shared_ptr<CRBTree<CString, Interface>>;
				CRBTree<CString, Interface>* pVal = new CRBTree<CString, Interface>;
				p->reset(pVal);
				m_pData = p;
				m_iType = ObjectType;
			}
			p = (shared_ptr<CRBTree<CString, Interface>>*)m_pData;

			return (*p->get())[k];
		}

		Interface& Interface::operator [](const CString& k) const
		{
			return Find(k);
		}

		Interface& Interface::operator [](const wchar_t* k) const
		{
			return Find(k);
		}

		bool Interface::Exists(const CString& k) const
		{
			shared_ptr<CRBTree<CString, Interface>>* p = NULL;
			if (m_iType != ObjectType || m_pData == NULL)
			{
				Clear();

				CRBTree<CString, Interface>* pVal = new CRBTree<CString, Interface>;
				p = new shared_ptr<CRBTree<CString, Interface>>;
				p->reset(pVal);
				m_pData = p;
				m_iType = ObjectType;
			}
			p = (shared_ptr<CRBTree<CString, Interface>>*)m_pData;
			
			if ((*p)->Exits(k))
			{
				return true;
			}

			return false;
		}

		void Interface::Start() const
		{
			if (m_pIter != NULL)
			{
				delete (CTreeIter<CString, Interface>*)m_pIter;
				m_pIter = NULL;
			}
			shared_ptr<CRBTree<CString, Interface>>* p = NULL;
			if (m_iType == ObjectType || m_pData != NULL)
			{
				p = (shared_ptr<CRBTree<CString, Interface> >*)m_pData;
				CTreeIter<CString, Interface>* pIter = new CTreeIter<CString, Interface>((*p)->Iter(EIterOrder::PostOrder));
				pIter->Start();
				m_pIter = pIter;
			}
		}

		bool Interface::Step() const
		{
			if (m_pIter == NULL || m_iType != ObjectType || m_pData == NULL)
			{
				return false;
			}

			CTreeIter<CString, Interface>* pIter = (CTreeIter<CString, Interface>*)m_pIter;
			return pIter->Step();
		}

		bool Interface::KeepUp() const
		{
			bool bFlag = false;
			if (m_pIter == NULL || m_iType != ObjectType || m_pData == NULL)
			{
				return bFlag;
			}

			CTreeIter<CString, Interface>* pIter = (CTreeIter<CString, Interface>*)m_pIter;
			bFlag = pIter->Keepup();
			if (bFlag == false)
			{
				delete pIter;
				m_pIter = NULL;
			}
			return bFlag;
		}

		Element Interface::Current() const
		{
			Element v;

			if (m_pIter == NULL || m_iType != ObjectType || m_pData == NULL)
			{
				return v;
			}

			CTreeIter<CString, Interface>* pIter = (CTreeIter<CString, Interface>*)m_pIter;
			CTreeNode<CString, Interface>* pNode = pIter->GetCurrentNode();
			if (pNode != NULL)
			{
				v.k = pNode->GetKey();
				v.v = pNode->GetValue();					 
			}

			return v;
		}

		void Interface::Append(const Interface& v) const
		{
			shared_ptr<List<Interface>>* p = NULL;
			if (m_iType != ArrayType || m_pData == NULL)
			{
				Clear();

				p = new shared_ptr<List<Interface>>;
				List<Interface>* pVal = new List<Interface>;
				p->reset(pVal);

				m_pData = p;
				m_iType = ArrayType;
			}

			p = (shared_ptr<List<Interface>>*)m_pData;
			(*p)->Append(v);
		}

		void Interface::Delete(const int& i) const
		{
			shared_ptr<List<Interface>>* p = NULL;
			if (m_iType != ArrayType || m_pData == NULL)
			{
				Clear();

				p = new shared_ptr<List<Interface>>;
				List<Interface>* pVal = new List<Interface>;
				p->reset(pVal);

				m_pData = p;
				m_iType = ArrayType;
			}

			p = (shared_ptr<List<Interface>>*)m_pData;
			if (i < 0 || i >= (int)(*p)->Count())
			{
				return;
			}

			(*p)->Remove(i);
		}

		Interface& Interface::At(const int& i) const
		{
			shared_ptr<List<Interface>>* p = NULL;
			if (m_iType != ArrayType || m_pData == NULL)
			{
				Clear();

				p = new shared_ptr<List<Interface>>;
				List<Interface>* pVal = new List<Interface>;
				p->reset(pVal);

				m_pData = p;
				m_iType = ArrayType;
			}

			p = (shared_ptr<List<Interface>>*)m_pData;
			if (i < 0 || i >= (int)(*p)->Count())
			{
				return Interface();	// ????????????
			}

			return (*p)->At(i);
		}

		Interface& Interface::operator [](const int& i) const
		{
			return At(i);
		}

		bool Interface::IsEmpty() const
		{
			bool bFlag = false;
			if (m_iType == ObjectType && m_pData != NULL)
			{
				shared_ptr<CRBTree<CString, Interface>>* p = (shared_ptr<CRBTree<CString, Interface>>*)m_pData;
				bFlag = (*p)->IsEmpty();
			}
			else if (m_iType == ArrayType && m_pData != NULL)
			{
				shared_ptr<List<Interface>>* p = (shared_ptr<List<Interface>>*)m_pData;
				bFlag = (*p)->IsEmpty();
			}
			return bFlag;
		}

		void Interface::RemoveAll() const
		{
			if (m_iType == ObjectType && m_pData != NULL)
			{
				shared_ptr<CRBTree<CString, Interface>>* p = (shared_ptr<CRBTree<CString, Interface>>*)m_pData;
				(*p)->Clear();
			}
			else if (m_iType == ArrayType && m_pData != NULL)
			{
				shared_ptr<List<Interface>>* p = (shared_ptr<List<Interface>>*)m_pData;
				(*p)->Clear();
			}
		}

		int Interface::Count() const
		{
			int iCount = 0;

			if (m_iType == ObjectType && m_pData != NULL)
			{
				shared_ptr<CRBTree<CString, Interface>>* p = (shared_ptr<CRBTree<CString, Interface>>*)m_pData;
				iCount = (int)(*p)->GetNodeCount();
			}
			else if (m_iType == ArrayType && m_pData != NULL)
			{
				shared_ptr<List<Interface>>* p = (shared_ptr<List<Interface>>*)m_pData;
				iCount = (int)(*p)->Count();
			}

			return iCount;
		}

		Interface::ValueType Interface::GetType() const
		{
			return this->m_iType;
		}

		bool Interface::IsNullType() const
		{ 
			return (m_iType == Interface::NullType);
		}

		bool Interface::IsNumeric() const
		{
			if (m_iType == Interface::IntType)
			{
				return true;
			}
			else if (m_iType == Interface::LongType)
			{
				return true;
			}
			else if (m_iType == Interface::DoubleType)
			{
				return true;
			}
			return false;
		}

		void Interface::ClearBool() const
		{
			if (m_pData != NULL && m_iType == BoolType)
			{
				delete ((shared_ptr<bool>*)m_pData);
				m_pData = NULL;
			}
		}

		void Interface::ClearInt() const
		{
			if (m_pData != NULL && m_iType == IntType)
			{
				delete ((shared_ptr<int>*)m_pData);
				m_pData = NULL;
			}
		}

		void Interface::ClearUint64() const
		{
			if (m_pData != NULL && m_iType == Uint64Type)
			{
				delete ((shared_ptr<UINT64>*)m_pData);
				m_pData = NULL;
			}
		}

		void Interface::ClearLong() const
		{
			if (m_pData != NULL && m_iType == LongType)
			{
				delete ((shared_ptr<long>*)m_pData);
				m_pData = NULL;
			}
		}

		void Interface::ClearDouble() const
		{
			if (m_pData != NULL && m_iType == DoubleType)
			{
				delete ((shared_ptr<double>*)m_pData);
				m_pData = NULL;
			}
		}

		void Interface::ClearString() const
		{
			if (m_pData != NULL && m_iType == StringType)
			{
				delete ((shared_ptr<CString>*)m_pData);
				m_pData = NULL;
			}
		}

		void Interface::ClearArray() const
		{
			if (m_pData != NULL && m_iType == ArrayType)
			{
				delete ((shared_ptr<List<Interface>>*)m_pData);
				m_pData = NULL;		
			}
		}

		void Interface::ClearObject() const
		{
			if (m_pData != NULL && m_iType == ObjectType)
			{				
				delete ((shared_ptr<CRBTree<CString, Interface>>*)m_pData);
				m_pData = NULL;
			}
		}

		void Interface::Clear() const
		{
			ClearBool();
			ClearInt();
			ClearUint64();
			ClearLong();
			ClearDouble();
			ClearString();
			ClearArray();
			ClearObject();

			if (m_pIter != NULL)
			{
				delete m_pIter;
				m_pIter = NULL;
			}
		}

	} // End namespace CPP

} // End namesapce Ext