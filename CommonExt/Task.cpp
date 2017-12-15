#include "stdafx.h"
#include "Task.h"
#include "ThreadBase.h"

CTask::CTask()
{
	m_Number = 0;
	m_pProducer = NULL;
	m_pCustomer = NULL;
	m_pContext = NULL;
	m_bNeedResponse = false;
	m_bUseForResponse = false;
	State.Reset();
}

CTask::~CTask()
{	
	m_Number = 0;
	m_pProducer = NULL;
	m_pCustomer = NULL;
	m_pContext = NULL;
	m_bNeedResponse = false;
	//m_bUseForResponse = false;
	State.Reset();
}

CTask* CTask::CreateTask(CThreadBase *pProducer, CThreadBase *pCustomer)
{
	CTask *pResult = new CTask();
	pResult->m_pProducer = pProducer;
	pResult->m_pCustomer = pCustomer;

	if (pProducer != NULL)
		pResult->m_Number = pProducer->GenerateTaskNumber();	// 任务的编号由创建线程产生

	return pResult;
}

CTask* CTask::CreateResponseTask()
{
	CTask *pResult = new CTask();
	pResult->m_pProducer = m_pCustomer;
	pResult->m_pCustomer = m_pProducer;
	//pResult->m_pContext = m_pContext;		// 不能复制上下文环境！！！
	pResult->m_Number = m_Number;
	pResult->m_bUseForResponse = true;
	pResult->m_bNeedResponse = false;
	pResult->State = State;
	return pResult;
}

void CTask::ReleaseTask(CTask *&pTask)
{
	//DUITRACE(_T("Going to release task (%s)"), pTask->ToString());
	delete pTask;
	pTask = NULL;
}

CThreadBase* CTask::GetProducer()
{
	return m_pProducer;
}

CThreadBase* CTask::GetCustomer()
{
	return m_pCustomer;
}

void CTask::SetTaskNumber(TaskNumber num)
{
	m_Number = num;
}

void CTask::Do()
{
	Ext::CPP::EventArg *pArg =new Ext::CPP::EventArg(m_pCustomer, this);
	Command.Raise(pArg);			// 触发当前的任务包含的命令，开始执行任务
	delete pArg;
	pArg = NULL;
}

TaskNumber CTask::Assign(void *pContext, bool needResponse)
{
	m_pContext = pContext;
	m_bNeedResponse = needResponse;
	TaskNumber num = m_pCustomer->AssignTask(this);
	return num;
}

Panic CTask::Assign(WaitEnums wait, Panic state, void *pContext, EventHandle handle, CThreadBase *pFrom, CThreadBase *pTo)
{
	CTask *pTask = CTask::CreateTask(pFrom, pTo);
	pTask->Command += handle;
	pTask->State = state;
	TaskNumber num = pTask->Assign(pContext, wait & Wait);

	Panic panic;
	if (wait == Wait && pFrom != NULL)
		panic = pFrom->WaitResponse(num);
	return panic;
}

bool CTask::IsUseForResponse()
{
	return m_bUseForResponse;
}

void CTask::SetUseForResponse(bool response)
{
	m_bUseForResponse = response;
}


TaskNumber CTask::GetTaskNumber()
{
	return m_Number;
}

bool CTask::IsNeedResponse()
{
	return m_bNeedResponse;
}

std::wstring CTask::ToString()
{
	std::wstring needResponse = m_bNeedResponse == true ? _T("true") : _T("false");
	std::wstring useForResponse = m_bUseForResponse == true ? _T("true") : _T("false");

	//wchar_t result[1024];

	std::wstring result = _T("");
	result += _T("Producer:");
	result += m_pProducer != NULL ? m_pProducer->GetName() : _T("Unknown");
	result += _T(", Customer:");
	result += m_pCustomer != NULL ? m_pCustomer->GetName() : _T("Unknown");
	result += _T(", UseForResponse:");
	result += needResponse;
	result += _T(", Num:");
	wchar_t number[32];
	wsprintf(number, _T("%d"), m_Number);
	result += number;

	return result;
}