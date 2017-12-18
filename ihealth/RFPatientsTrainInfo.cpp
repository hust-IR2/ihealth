#include "StdAfx.h"
#include "RFPatientsTrainInfo.h"
#include "RFMainWindow.h"
#include "RFPatientsManager.h"

RFPatientsTrainInfo* RFPatientsTrainInfo::m_sigleton = NULL;

RFPatientsTrainInfo* RFPatientsTrainInfo::get()
{
	if (m_sigleton == NULL) {
		m_sigleton = new RFPatientsTrainInfo();
		//m_sigleton->step();
	}

	return m_sigleton;
}

void RFPatientsTrainInfo::release()
{
	if (m_sigleton) {
		delete m_sigleton;
		m_sigleton = NULL;
	}
}

RFPatientsTrainInfo::RFPatientsTrainInfo(void)
{
	m_current_page = 0;
	m_doctorid = 0;
	m_hospitalid = 0;
}

RFPatientsTrainInfo::~RFPatientsTrainInfo(void)
{
}

int RFPatientsTrainInfo::LoadPatientTrainInfo()
{
	LoadPatientTrainInfoParam *pParam = new LoadPatientTrainInfoParam();

	pParam->hospitalid = m_hospitalid;
	pParam->doctorid = m_doctorid;

	CTask::Assign(CTask::NotWait, Panic(), pParam, EventHandle(&RFMySQLThread::LoadPatientTrainInfo), RFMainWindow::UIThread, RFMainWindow::DBThread);
	return 1;
}

void RFPatientsTrainInfo::modify(const PatientInfo& patient)
{
	std::list<PatientTrainInfo>::iterator begin = m_patienttraininfos.begin();
	for (; begin != m_patienttraininfos.end(); begin++) {
		if (begin->id == patient.id) {
			wchar_t age[64] = _T("");
			wsprintf(age, _T("%d"), patient.age);
			begin->age = age;
			begin->createtime = patient.createtime;
			begin->name = patient.name;
			begin->sex = patient.sex;
		} 
	}
}

void RFPatientsTrainInfo::remove(int patientid)
{
	std::list<PatientTrainInfo>::iterator begin = m_patienttraininfos.begin();
	for (; begin != m_patienttraininfos.end(); ) {
		if (begin->id == patientid) {
			std::list<PatientTrainInfo>::iterator iter = begin;
			begin++;
			m_patienttraininfos.erase(iter);
		} else {
			begin++;
		}
	}
}

void RFPatientsTrainInfo::add(const PatientInfo& pateint)
{

}

std::list<PatientTrainInfo> RFPatientsTrainInfo::search(std::wstring patientname)
{
	std::list<PatientTrainInfo> trains;

	std::list<PatientTrainInfo>::iterator begin = m_patienttraininfos.begin();
	for (; begin != m_patienttraininfos.end(); begin++) {
		if (begin->name == patientname) {
			trains.push_back(*begin);
		}
	}

	return trains;
}

PatientTrainInfo RFPatientsTrainInfo::getTrainInfo(std::wstring patientid)
{
	PatientTrainInfo train;

	std::list<PatientTrainInfo>::iterator begin = m_patienttraininfos.begin();
	for (; begin != m_patienttraininfos.end(); begin++) {
		if (begin->id == _wtoi(patientid.c_str())) {
			train = *begin;
		}
	}

	return train;
}

void RFPatientsTrainInfo::setDoctorID(int id)
{
	m_doctorid = id;
}

void RFPatientsTrainInfo::setHospitalID(int id)
{
	m_hospitalid = id;
}

void RFPatientsTrainInfo::setCurrentPage(int page)
{
	m_current_page = page;
}

int RFPatientsTrainInfo::pageElementNum(int page)
{
	return (m_patienttraininfos.size() - page * PATIENT_TRAIN_PAGE_NUMBER) % PATIENT_TRAIN_PAGE_NUMBER;
}

std::list<PatientTrainInfo> RFPatientsTrainInfo::getPageElement(int page)
{
	std::list<PatientTrainInfo> patientTrainInfos;

	int i = 0;
	std::list<PatientTrainInfo>::iterator begin = m_patienttraininfos.begin();
	for (; begin != m_patienttraininfos.end(); begin++) {

		if ( page * PATIENT_TRAIN_PAGE_NUMBER <= i && i < (page + 1) * PATIENT_TRAIN_PAGE_NUMBER ) {
			patientTrainInfos.push_back(*begin);
		}
		i++;
	}


	return patientTrainInfos;
}

void RFPatientsTrainInfo::removeElement(int patientid)
{
	std::list<PatientTrainInfo>::iterator begin = m_patienttraininfos.begin();
	for (; begin != m_patienttraininfos.end(); ) {

		if (begin->id == patientid) {
			std::list<PatientTrainInfo>::iterator iter = begin;
			begin++;
			m_patienttraininfos.erase(iter);
		} else {
			begin++;
		}

	}
}

void RFPatientsTrainInfo::exportTrainInfo(std::wstring templatepath, std::wstring savepath)
{
	RFPatientsManager::get()->m_excel.OpenExcelFile(templatepath.c_str());
	CString strSheetName = RFPatientsManager::get()->m_excel.GetSheetName(1);
	RFPatientsManager::get()->m_excel.LoadSheet(strSheetName);

	int i = 3;
	std::list<PatientTrainInfo>::iterator begin = m_patienttraininfos.begin();
	for (; begin != m_patienttraininfos.end(); begin++) {
		RFPatientsManager::get()->m_excel.SetCellInt(i, 1, begin->id);
		RFPatientsManager::get()->m_excel.SetCellString(i, 2, begin->name);
		RFPatientsManager::get()->m_excel.SetCellString(i, 3, begin->sex);
		RFPatientsManager::get()->m_excel.SetCellString(i, 4, begin->age);
		RFPatientsManager::get()->m_excel.SetCellString(i, 5, begin->createtime);
		i++;
	}

	RFPatientsManager::get()->m_excel.SaveasXSLFile(savepath.c_str());
	RFPatientsManager::get()->m_excel.CloseExcelFile(FALSE);
}

int RFPatientsTrainInfo::OnLoadPatientTrainInfoOK(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	LoadPatientTrainInfoResult *pParam = pTask->GetContext<LoadPatientTrainInfoResult*>();
	if (!pParam) {
		return 1;
	}

	RFPatientsTrainInfo::get()->m_patienttraininfos = pParam->patienttraininfos;

	delete pParam;
	return 1;
}