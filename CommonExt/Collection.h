#ifndef __COLLECTION_H__
#define __COLLECTION_H__

#include <assert.h>
#include <crtdbg.h>

//#ifdef _DEBUG
//#define DEBUG_NEW new (_NORMAL_BLOCK, THIS_FILE, __LINE__)
//#endif

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

#ifdef _DEBUG
#ifndef ASSERT
#define ASSERT  assert
#endif
#else   // not _DEBUG
#ifndef ASSERT
#define ASSERT
#endif
#endif  // _DEBUG

namespace Ext
{

	namespace Collection
	{
		template<class T>
		static int CompareAB(const T &a, const T &b)
		{
			if (a < b) 
				return -1;
			if (b < a) 
				return  1;

			return 0;
		}
	}// End namespace Collection
}// End namespace Ext;

#endif  // __COLLECTION_H__