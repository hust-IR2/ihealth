#include "StdAfx.h"
#include "Tool.h"

using namespace Ext;

CTool::CTool(void)
{
}

CTool::~CTool(void)
{
}

int CTool::CompareStringWithNoCase(const CString &a, const CString &b)
{
	return a.CompareNoCase(b);
}
