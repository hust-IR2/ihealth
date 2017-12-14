#include "RFEvaluationData.h"
#include "RFMainWindow.h"

RFEvaluationDetailData* RFEvaluationDetailData::m_sigleton = NULL;

RFEvaluationDetailData* RFEvaluationDetailData::get()
{
	if (m_sigleton == NULL) {
		m_sigleton = new RFEvaluationDetailData();
		//m_sigleton->step();
	}

	return m_sigleton;
}

void RFEvaluationDetailData::release()
{
	if (m_sigleton) {
		delete m_sigleton;
		m_sigleton = NULL;
	}
}

RFEvaluationDetailData::RFEvaluationDetailData()
{

}

RFEvaluationDetailData::~RFEvaluationDetailData()
{

}

void RFEvaluationDetailData::Load(int id)
{
	std::list<EvaluationData>::iterator begin = RFEvaluationData::get()->m_datas.begin();
	for (; begin != RFEvaluationData::get()->m_datas.end(); begin++) 
	{
		if (begin->id == id) {
			m_datas = begin->datas;
			m_score = begin->score;
			break;
		}
	}
}

void RFEvaluationDetailData::setCurPage(int page)
{
	m_current_page = page;
}

std::list<EvaluationRecordData> RFEvaluationDetailData::Get(int page)
{
	std::list<EvaluationRecordData> datas;

	int i = 0;
	std::list<EvaluationRecordData>::iterator begin = m_datas.begin();
	for (; begin != m_datas.end(); begin++) {

		if ( page * 4 <= i && i < (page + 1) * 4 ) {
			datas.push_back(*begin);
		}
		i++;
	}


	return datas;
}

int RFEvaluationDetailData::GetElementNumber(int page)
{
	return (m_datas.size() - page * 4) % 4;
}


//////////////////////////////////////////////////////////////////////////////////////////
RFEvaluationData::RFEvaluationData(void)
{
	setCurPage(0);
	setDoctorID(0);
	m_nextid = 1;
}

RFEvaluationData::~RFEvaluationData(void)
{
}


RFEvaluationData* RFEvaluationData::m_sigleton = NULL;

RFEvaluationData* RFEvaluationData::get()
{
	if (m_sigleton == NULL) {
		m_sigleton = new RFEvaluationData();
		//m_sigleton->step();
	}

	return m_sigleton;
}

void RFEvaluationData::release()
{
	if (m_sigleton) {
		delete m_sigleton;
		m_sigleton = NULL;
	}
}

void RFEvaluationData::Load(int type)
{
	LoadEvaluationParam *pParam = new LoadEvaluationParam();

	pParam->type = type;
	pParam->doctorid = m_doctorid;

	CTask::Assign(CTask::NotWait, Panic(), pParam, EventHandle(&RFMySQLThread::LoadEvaluationDatas), RFMainWindow::UIThread, RFMainWindow::DBThread);
}

void RFEvaluationData::Add(EvaluationData& data)
{
	RFEvaluationData::get()->m_datas.push_front(data);

	m_nextid++;
	EvaluationData* pParam = new EvaluationData;
	*pParam = data;

	CTask::Assign(CTask::NotWait, Panic(), pParam, EventHandle(&RFMySQLThread::AddEvaluationData), RFMainWindow::UIThread, RFMainWindow::DBThread);
}

int RFEvaluationData::GetElementNumber(int page)
{
	return (m_datas.size() - page * 5) % 5;
}

std::list<EvaluationData> RFEvaluationData::Get(int page)
{
	std::list<EvaluationData> datas;

	int i = 0;
	std::list<EvaluationData>::iterator begin = m_datas.begin();
	for (; begin != m_datas.end(); begin++) {

		if ( page * 5 <= i && i < (page + 1) * 5 ) {
			datas.push_back(*begin);
		}
		i++;
	}


	return datas;
}

void RFEvaluationData::setCurPage(int page)
{
	m_current_page = page;
}

void RFEvaluationData::setDoctorID(int id)
{
	m_doctorid = id;
}

int RFEvaluationData::OnLoadOK(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	LoadEvaluationResult *pParam = pTask->GetContext<LoadEvaluationResult*>();
	if (!pParam) {
		return 1;
	}

	RFEvaluationData::get()->m_datas = pParam->datas;
	RFEvaluationData::get()->m_nextid = pParam->nextid;

	if (RFMainWindow::MainWindow) {
		std::wstring score = _T("ÎÞÊý¾Ý");
		std::list<EvaluationData>::iterator begin = pParam->datas.begin();
		for (; begin != pParam->datas.end(); begin++) {
			if (RFMainWindow::MainWindow->m_current_patient.id == begin->patientid) {
				score = begin->score;
				break;
			}
		}

		RFMainWindow::MainWindow->UpdateEvaluationScore(score);
		RFMainWindow::MainWindow->UpdateEvaluationNumber(0);
		RFMainWindow::MainWindow->UpdateEvaluationPage(RFEvaluationData::get()->Get(0));
	}
	
	delete pParam;
	return 0;
}
