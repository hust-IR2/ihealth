#include "StdAfx.h"
#include "RFPatientsTrainDetails.h"
#include "RFMainWindow.h"
#include "RFPatientsManager.h"
#include "RFPatientsTrainInfo.h"

RFPatientsTrainDetails* RFPatientsTrainDetails::m_sigleton = NULL;

RFPatientsTrainDetails* RFPatientsTrainDetails::get()
{
	if (m_sigleton == NULL) {
		m_sigleton = new RFPatientsTrainDetails();
		//m_sigleton->step();
	}

	return m_sigleton;
}

void RFPatientsTrainDetails::release()
{
	if (m_sigleton) {
		delete m_sigleton;
		m_sigleton = NULL;
	}
}

RFPatientsTrainDetails::RFPatientsTrainDetails(void)
{
	m_hospitalid = 0;
	m_doctorid = 0;
	m_currentpage = 0;
}

RFPatientsTrainDetails::~RFPatientsTrainDetails(void)
{
}

void RFPatientsTrainDetails::SetDoctorID(int id)
{
	m_doctorid = id;
}

void RFPatientsTrainDetails::SetHospitalID(int id)
{
	m_hospitalid = id;
}

void RFPatientsTrainDetails::SetCurrentPage(int page)
{
	m_currentpage = page;
}

void RFPatientsTrainDetails::AddPatientTrainDetails(const PatientTrainDetails& detail)
{
	PatientTrainDetails* pParam = new PatientTrainDetails;
	*pParam = detail;

	CTask::Assign(CTask::NotWait, Panic(), pParam, EventHandle(&RFMySQLThread::AddPatientTrainDetails), RFMainWindow::UIThread, RFMainWindow::DBThread);
}

int RFPatientsTrainDetails::LoadPatientTrainDetails()
{
	LoadPatientTrainDetailsParam *pParam = new LoadPatientTrainDetailsParam();

	pParam->hospitalid = m_hospitalid;
	pParam->doctorid = m_doctorid;

	CTask::Assign(CTask::NotWait, Panic(), pParam, EventHandle(&RFMySQLThread::LoadPatientTrainDetails), RFMainWindow::UIThread, RFMainWindow::DBThread);
	return 1;
}

int RFPatientsTrainDetails::pageElementNum(int page)
{
	return (m_patienttraindetails.size() - page*PATIENT_TRAIN_DETAILS_NUMBER) % PATIENT_TRAIN_DETAILS_NUMBER;
}

void RFPatientsTrainDetails::SetCurrentPatientID(int patientid)
{
	m_patientid = patientid;
}

std::list<PatientTrainDetails> RFPatientsTrainDetails::getPageElement(int page)
{
	std::list<PatientTrainDetails> patientTrainInfos;

	int i = 0;
	std::list<PatientTrainDetails>::iterator begin = m_patienttraindetails.begin();
	for (; begin != m_patienttraindetails.end(); begin++) {
		if (begin->patientid != m_patientid) {
			continue;
		}
		if ( page * PATIENT_TRAIN_DETAILS_NUMBER <= i && i < (page + 1) * PATIENT_TRAIN_DETAILS_NUMBER ) {
			patientTrainInfos.push_back(*begin);
		}

		i++;
	}


	return patientTrainInfos;
}

void RFPatientsTrainDetails::removeElement(int patientid)
{
	std::list<PatientTrainDetails>::iterator begin = m_patienttraindetails.begin();
	for (; begin != m_patienttraindetails.end(); ) {

		if (begin->patientid == patientid) {
			m_patienttraindetails.erase(begin);
		} else {
			begin++;
		}
	}
}

PatientTrainDetails RFPatientsTrainDetails::getTrainDetail(int id)
{
	PatientTrainDetails trainDetail;

	std::list<PatientTrainDetails>::iterator begin = m_patienttraindetails.begin();
	for (; begin != m_patienttraindetails.end(); begin++) {
		if (begin->patientid != m_patientid) {
			continue;
		}

		if (begin->id == id) {
			trainDetail = *begin;
			break;
		}
	}

	return trainDetail;
}

std::list<PatientTrainDetails> RFPatientsTrainDetails::search(std::wstring createtime)
{
	std::list<PatientTrainDetails> trains;

	std::list<PatientTrainDetails>::iterator begin = m_patienttraindetails.begin();
	for (; begin != m_patienttraindetails.end(); begin++) {
		if (begin->patientid != m_patientid) {
			continue;
		}

		if (begin->traindate == createtime.c_str()) {
			trains.push_back(*begin);
		}
	}

	return trains;
}

int RFPatientsTrainDetails::OnLoadPatientTrainDetailsOK(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	LoadPatientTrainDetailsResult *pParam = pTask->GetContext<LoadPatientTrainDetailsResult*>();
	if (!pParam) {
		return 1;
	}

	RFPatientsTrainDetails::get()->m_patienttraindetails = pParam->patienttraindetails;

	delete pParam;
	return 1;
}

void RFPatientsTrainDetails::exportTrainDetail(std::wstring templatepath, std::wstring savepath)
{
	RFPatientsManager::get()->m_excel.OpenExcelFile(templatepath.c_str());
	CString strSheetName = RFPatientsManager::get()->m_excel.GetSheetName(1);
	RFPatientsManager::get()->m_excel.LoadSheet(strSheetName);

	int row = 3;
	wchar_t patientid[64] = _T("");
	wsprintf(patientid, _T("%d"), m_patientid);
	PatientTrainInfo traininfo = RFPatientsTrainInfo::get()->getTrainInfo(patientid);
	RFPatientsManager::get()->m_excel.SetCellInt(row, 1, traininfo.id);
	RFPatientsManager::get()->m_excel.SetCellString(row, 2, traininfo.name);
	RFPatientsManager::get()->m_excel.SetCellString(row, 3, traininfo.sex);
	RFPatientsManager::get()->m_excel.SetCellString(row, 4, traininfo.age);
	RFPatientsManager::get()->m_excel.SetCellString(row, 5, traininfo.createtime);

	row = 6;
	std::list<PatientTrainDetails>::iterator begin = m_patienttraindetails.begin();
	for (; begin != m_patienttraindetails.end(); begin++) {
		if (begin->patientid != m_patientid) {
			continue;
		}

		RFPatientsManager::get()->m_excel.SetCellString(row, 1, begin->traindate);
		RFPatientsManager::get()->m_excel.SetCellString(row, 2, begin->content);
		RFPatientsManager::get()->m_excel.SetCellString(row, 4, begin->traintime);
		RFPatientsManager::get()->m_excel.SetCellString(row, 5, begin->duration);

		row++;
	}


	RFPatientsManager::get()->m_excel.SaveasXSLFile(savepath.c_str());
	RFPatientsManager::get()->m_excel.CloseExcelFile(FALSE);
}