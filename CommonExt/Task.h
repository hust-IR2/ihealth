#pragma once

#ifdef DCICOMMONEXT_MODULE
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllexport)
#else
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllimport)
#endif

#include "ThreadBase.h"

class CThreadBase;

class DCICOMMONEXT_MODULE_EXPIMP CTask
{
public:
	enum WaitEnums
	{
		NotWait	= 0,
		Wait    = 1
	};

protected:

	// 任务在当前线程的任务编号
	TaskNumber m_Number;

	CThreadBase  *m_pProducer;	// 引用的任务的创建者，在析构时不能释放其资源
	CThreadBase  *m_pCustomer;	// 引用任务的消费者，即任务的执行者，在析构时不能释放其资源
	void		 *m_pContext;	// 引用的上下文环境，在析构时不能清理其资源

	bool m_bNeedResponse;
	bool m_bUseForResponse;		// 指示当前任务只是做为之前同一个任务编号的取消任务
	

public:
	Ext::CPP::Event Command;	// 任务的执行函数
	Panic State;				// 记录任务的状态，只对响应任务有效。创建响应任务时，当前任务的 m_Panic 会传递到对应的响应任务的 m_Panic

private:
	CTask();
	~CTask();

public:
	static CTask* CreateTask(CThreadBase *pProducer, CThreadBase *pCustomer);
	static void ReleaseTask(CTask *&pTask);
	CTask* CreateResponseTask();

	// 仅供内部调用
	virtual void Do();
	void SetTaskNumber(TaskNumber num);

	CThreadBase* GetProducer();
	CThreadBase* GetCustomer();

	bool IsUseForResponse();
	void SetUseForResponse(bool response);
	bool IsNeedResponse();
	TaskNumber GetTaskNumber();
	std::wstring ToString();

	// 将当前任务分配到 Owner 的线程中，并在执行完以后，自动清理当前 Task 实例的资源
	TaskNumber Assign(void *pContext, bool needResponse);
	static Panic Assign(WaitEnums wait, Panic state, void *pContext, EventHandle handle, CThreadBase *pFrom, CThreadBase *pTo);

	// 指针类型转换。调用：CAdoRow *pRow = pTask->GetAttach<CAdoRow*>();	// pTask 是CTask类型的指针，Attach 指向指针
	template<typename T>
	T GetContext()
	{
		T pointer = static_cast<T>(m_pContext);
		return pointer;
	}
};