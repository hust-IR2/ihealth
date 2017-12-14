
#pragma once

//文件最大4M
static const int g_nMaxSize = 1024*1024*4;

class CSimpleLog
{
public:
	static CSimpleLog*	Instance()
	{
		static CSimpleLog log;
		return &log;
	}
	bool	WriteLog(const char* lpLog);
	bool	WriteLog(const wchar_t* lpLog);
	bool	WriteLog(const char* lpFile, long lLine, const char* lpLog);
	bool	WriteLog(const char* lpFile, long lLine, const wchar_t* lpLog);
	bool	WriteJoinLog(const wchar_t* lpText, const wchar_t* lpLog);
	bool	WriteJoinLog(const char* lpFile, long lLine, const wchar_t* lpText, const wchar_t* lpLog);

protected:
	CSimpleLog();
	~CSimpleLog();
	bool	InitLogFile();
	char*	WcharToChar(const wchar_t* lpSource);

private:
	wchar_t	m_szLog[MAX_PATH];
	int		m_nWriteSize;
};
#ifdef _DEBUG
#define SLOG1(x)	
#define SLOG2(x, y)	
#define LOG1(x)		
#define LOG2(x, y)	
#else
#define SLOG1(x)			CSimpleLog::Instance()->WriteLog(x);
#define SLOG2(x, y)			CSimpleLog::Instance()->WriteJoinLog(x, y);	
#define LOG1(x)				CSimpleLog::Instance()->WriteLog(__FILE__, __LINE__, x)
#define LOG2(x, y)			CSimpleLog::Instance()->WriteJoinLog(__FILE__, __LINE__, x, y)
#endif