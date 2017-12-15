#include "stdafx.h"
#include "Log.h"
#include <time.h>
#include <ShlObj.h>


static const char* g_lpEnd = "\n";

CSimpleLog::CSimpleLog()
: m_nWriteSize(0)
{
	InitLogFile();
}

CSimpleLog::~CSimpleLog()
{

}

bool CSimpleLog::InitLogFile()
{
	wchar_t szPath[MAX_PATH]={0};
	::GetModuleFileName(NULL, szPath, MAX_PATH);
	for ( int i=wcslen(szPath)-1; i>=0; --i )
	{
		if ( szPath[i] == '\\' )
			break;
		szPath[i] = '\0';
	}
	wcscat(szPath, L"Log");
	SHCreateDirectory(NULL, szPath);
	SYSTEMTIME st;
	GetLocalTime(&st);
	swprintf(m_szLog, L"%s\\%d%02d%02d-%02d%02d%02d%03d.log", szPath, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	m_nWriteSize = 0;
	return true;
}

bool CSimpleLog::WriteLog( const char* lpLog )
{
	if ( NULL == lpLog )
		return false;
	int nLen = strlen(lpLog);
	if ( 0 == nLen )
		return true;
	if ( m_nWriteSize>=g_nMaxSize )
		InitLogFile();
	FILE* fp = _wfopen(m_szLog, L"a+");
	if ( NULL == fp )
	{
		OutputDebugString(L"打开日志文件失败");
		return false;
	}
	SYSTEMTIME st;
	::GetLocalTime(&st);
	char szTime[30]={0};
	sprintf(szTime, "%d-%02d-%02d %02d:%02d:%02d  ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	fwrite(szTime, 1, strlen(szTime), fp);
	fwrite(lpLog, 1, nLen, fp);
	fwrite(g_lpEnd, 1, 1, fp);
	fclose(fp);
	m_nWriteSize += nLen+20;
	return true;
}

bool CSimpleLog::WriteLog( const wchar_t* lpLog )
{
	char* pBuffer = WcharToChar(lpLog);
	if ( NULL == pBuffer )
		return false;
	bool bRet = WriteLog(pBuffer);
	free(pBuffer);
	return bRet;
}

bool CSimpleLog::WriteLog( const char* lpFile, long lLine, const char* lpLog )
{
	if ( NULL == lpLog )
		return false;
	int nLen = strlen(lpLog);
	if ( 0 == nLen )
		return true;
	if ( m_nWriteSize>=g_nMaxSize )
		InitLogFile();
	FILE* fp = _wfopen(m_szLog, L"a+");
	if ( NULL == fp )
	{
		OutputDebugString(L"打开日志文件失败");
		return false;
	}
	SYSTEMTIME st;
	::GetLocalTime(&st);
	char szTime[30]={0};
	sprintf(szTime, "%d-%02d-%02d %02d:%02d:%02d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	fwrite(szTime, 1, strlen(szTime), fp);
	if ( NULL != lpFile )
		fwrite(lpFile, 1, strlen(lpFile), fp);
	char szLine[30];
	sprintf_s(szLine, " line=%ld：", lLine);
	fwrite(szLine, 1, strlen(szLine), fp);
	fwrite(lpLog, 1, nLen, fp);
	fwrite(g_lpEnd, 1, 1, fp);
	fclose(fp);
	m_nWriteSize += nLen+20;
	return true;
}

bool CSimpleLog::WriteLog( const char* lpFile, long lLine, const wchar_t* lpLog )
{
	char* lpBuffer = WcharToChar(lpLog);
	if ( NULL == lpBuffer )
		return false;
	bool bRet = WriteLog(lpFile, lLine, lpBuffer);
	free(lpBuffer);
	return bRet;
}

char* CSimpleLog::WcharToChar( const wchar_t* lpSource )
{
	if ( NULL == lpSource )
		return NULL;
	int nLen = wcslen(lpSource);
	if ( 0 == nLen )
		return NULL;
	int nNeedSize = WideCharToMultiByte(CP_ACP, 0, lpSource, nLen, NULL, 0, NULL, NULL);
	if ( 0 == nNeedSize )
		return NULL;
	char* pBuffer = (char*)malloc(sizeof(char)*(nNeedSize+1));
	if ( NULL == pBuffer )
		return NULL;
	WideCharToMultiByte(CP_ACP, 0, lpSource, nLen, pBuffer, nNeedSize, NULL, NULL);
	pBuffer[nNeedSize] = '\0';
	return pBuffer;
}

bool CSimpleLog::WriteJoinLog( const wchar_t* lpText, const wchar_t* lpLog )
{
	if ( NULL == lpText || NULL == lpLog )
		return false;
	int nTextLen = wcslen(lpText);
	int nLogLen = wcslen(lpLog);
	wchar_t* lpBuffer = (wchar_t*)malloc(sizeof(wchar_t)*(nTextLen+nLogLen+1));
	wcscpy(lpBuffer, lpText);
	wcscpy(lpBuffer+nTextLen, lpLog);
	lpBuffer[nTextLen+nLogLen] = '\0';
	bool bRet = WriteLog(lpBuffer);
	free(lpBuffer);
	return bRet;
}

bool CSimpleLog::WriteJoinLog( const char* lpFile, long lLine, const wchar_t* lpText, const wchar_t* lpLog )
{
	if ( NULL == lpFile || NULL == lpText || NULL == lpLog )
		return false;
	int nTextLen = wcslen(lpText);
	int nLogLen = wcslen(lpLog);
	wchar_t* lpBuffer = (wchar_t*)malloc(sizeof(wchar_t)*(nTextLen+nLogLen+1));
	wcscpy(lpBuffer, lpText);
	wcscpy(lpBuffer+nTextLen, lpLog);
	lpBuffer[nTextLen+nLogLen] = '\0';
	bool bRet = WriteLog(lpFile, lLine, lpBuffer);
	free(lpBuffer);
	return bRet;
}
