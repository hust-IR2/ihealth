#include "stdafx.h"
#include "UIThread.h"
#include "Task.h"

CUIThread::CUIThread(HWND hwnd) : CThreadBase()
{
	CThreadBase::m_Name = _T("UI Thread");
	CThreadBase::m_bRefThread = true;
	this->m_Hwnd = hwnd;
}

CUIThread::~CUIThread()
{
	::PostMessage(m_Hwnd, WM_THREAD_WANT_QUIT, 0, 0);

	MSG msg = { 0 };
	while (::PeekMessage(&msg, m_Hwnd, 0, 0, PM_REMOVE)) 
	{
		if (msg.message == WM_THREAD_WANT_QUIT)
			break;

		if (msg.message != WM_COMMUNICATE)
			continue;	// 可能接收到系统的其他消息

		CTask *pTask = (CTask*)msg.lParam;
		if (pTask == NULL)
			continue;

		pTask->Do();
		CTask::ReleaseTask(pTask);
	}
}

Panic CUIThread::Create(HWND hwnd)
{
	Panic panic;
	CUIThread *pThread = new CUIThread(hwnd);
	panic.SetTag(pThread);
	return panic;
}

Panic CUIThread::Release(CUIThread *&pThread)
{
	Panic panic;

	if (pThread != NULL)
	{
		delete pThread;
		pThread = NULL;
	}

	return panic;
}

TaskNumber CUIThread::AssignTask(CTask *pTask)
{
	std::wstring taskInfo = pTask->ToString();
	//DUITRACE(_T("UIThread assign task. (%s)"), taskInfo);

	TaskNumber num = pTask->GetTaskNumber();
	int count = 5;
	while (count > 0)
	{
		// 后台线程的消息队列若还未建立就向该线程发送消息，则发送会失败。出现此种情况的概率极低
		// 调用 PostMessage 以后，当前线程不能对 pTask 做任何访问
		BOOL ok = ::PostMessage(m_Hwnd, WM_COMMUNICATE, (WPARAM)this, (LPARAM)pTask);
		if (ok == TRUE)
		{
			//DUITRACE(_T("UIThread post message ok (%s)"), taskInfo);
			break;
		}

		//DUITRACE(_T("UIThread post message fail (%s)"), taskInfo);
		::Sleep(100);
		count -= 1;
	}

	//DUITRACE(_T("UIThread post message finish (%s)"), taskInfo);
	return num;
}

Panic CUIThread::WaitResponse(TaskNumber num)
{
	Panic panic;
	return panic;
}