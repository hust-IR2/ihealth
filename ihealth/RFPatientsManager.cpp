#include "StdAfx.h"
#include "RFPatientsManager.h"
#include "RFMainWindow.h"
#include "RFExcel.h"
#include "RFPatientsTrainInfo.h"

RFPatientsManager* RFPatientsManager::m_sigleton = NULL;

RFPatientsManager* RFPatientsManager::get()
{
	if (m_sigleton == NULL) {
		m_sigleton = new RFPatientsManager();
		OleInitialize(NULL);
		m_sigleton->m_excel.InitExcel();
		//m_sigleton->step();
	}

	return m_sigleton;
}

void RFPatientsManager::release()
{
	if (m_sigleton) {
		m_sigleton->m_excel.ReleaseExcel();
		OleUninitialize();
		delete m_sigleton;
		m_sigleton = NULL;
	}
}

RFPatientsManager::RFPatientsManager(void)
{
	m_current_group = -1;
	m_current_page = -1;
	m_hospitalid = -1;
	m_doctorid = -1;
}

RFPatientsManager::~RFPatientsManager(void)
{
}

void RFPatientsManager::SetHospitalID(int id)
{
	m_hospitalid = id;
}

void RFPatientsManager::setDoctorID(int id)
{
	m_doctorid = id;
}

int RFPatientsManager::step(int deletenum)
{
	if (m_current_group < 0 || (m_patients.size() - 1) / PATIENT_GROUP_NUMBER < (m_current_group + 1)) {
		LoadPatientParam *pParam = new LoadPatientParam();

		pParam->hospitalid = m_hospitalid;
		pParam->doctorid = m_doctorid;
		pParam->start = m_patients.size();
		pParam->num = PATIENT_GROUP_NUMBER + deletenum;

		CTask::Assign(CTask::NotWait, Panic(), pParam, EventHandle(&RFMySQLThread::Load), RFMainWindow::UIThread, RFMainWindow::DBThread);
		return 1;
	}

	return 0;
}

void RFPatientsManager::setPage(int num)
{
	m_current_page = num;

	m_current_group = num / 4;
}

void RFPatientsManager::remove(int patientid, int autoload)
{
	for (std::list<PatientInfo>::iterator it=m_patients.begin(); it!=m_patients.end(); ++it)
	{
		if (it->id == patientid) {
			m_patients.erase(it);
			break;
		}	
	}

	PatientInfo *pParam = new PatientInfo;
	pParam->id = patientid;
	RFPatientsTrainInfo::get()->remove(patientid);
	CTask::Assign(CTask::NotWait, Panic(), pParam, EventHandle(&RFMySQLThread::Delete), RFMainWindow::UIThread, RFMainWindow::DBThread);



	if (autoload) {
		step(1);
	}
}

void RFPatientsManager::modifyPwd(const ModifyPWDInfo& pwd)
{
	ModifyPWDInfo* pParam = new ModifyPWDInfo;
	*pParam = pwd;

	CTask::Assign(CTask::NotWait, Panic(), pParam, EventHandle(&RFMySQLThread::ModifyPWD), RFMainWindow::UIThread, RFMainWindow::DBThread);

}

void RFPatientsManager::modify(const LoginInfo& doctor)
{
	LoginInfo* pParam = new LoginInfo;
	*pParam = doctor;

	CTask::Assign(CTask::NotWait, Panic(), pParam, EventHandle(&RFMySQLThread::SaveDoctor), RFMainWindow::UIThread, RFMainWindow::DBThread);

}

void RFPatientsManager::modify(const PatientInfo &patient)
{
	std::list<PatientInfo>::iterator iter = m_patients.begin();
	while (iter != m_patients.end()) {
		if (iter->id == patient.id) {
			*iter = patient;
			break;
		}
		
		iter++;
	}

	PatientInfo* pParam = new PatientInfo;
	*pParam = patient;
	RFPatientsTrainInfo::get()->modify(patient);
	CTask::Assign(CTask::NotWait, Panic(), pParam, EventHandle(&RFMySQLThread::Save), RFMainWindow::UIThread, RFMainWindow::DBThread);
}

void RFPatientsManager::add(const PatientInfo &patient)
{
	PatientInfo* pParam = new PatientInfo;
	*pParam = patient;

	CTask::Assign(CTask::NotWait, Panic(), pParam, EventHandle(&RFMySQLThread::Add), RFMainWindow::UIThread, RFMainWindow::DBThread);
}

void RFPatientsManager::nextPatientID()
{
	CTask::Assign(CTask::NotWait, Panic(), NULL, EventHandle(&RFMySQLThread::NextPatientID), RFMainWindow::UIThread, RFMainWindow::DBThread);
}

void RFPatientsManager::search(std::wstring patientname)
{
	if (patientname.empty()) {
		return;
	}

	SearchPatientParam *pParam = new SearchPatientParam();

	pParam->patientname = patientname;

	CTask::Assign(CTask::NotWait, Panic(), pParam, EventHandle(&RFMySQLThread::SearchPatient), RFMainWindow::UIThread, RFMainWindow::DBThread);
}

void RFPatientsManager::filter(const PatientFilterParam& param)
{

	PatientFilterParam *pParam = new PatientFilterParam();

	*pParam = param;

	CTask::Assign(CTask::NotWait, Panic(), pParam, EventHandle(&RFMySQLThread::FilterPatient), RFMainWindow::UIThread, RFMainWindow::DBThread);
}

void RFPatientsManager::exportPatient(const ExportPatientParam& param)
{
	ExportPatientParam *pParam = new ExportPatientParam();

	*pParam = param;

	CTask::Assign(CTask::NotWait, Panic(), pParam, EventHandle(&RFMySQLThread::ExportPatient), RFMainWindow::UIThread, RFMainWindow::DBThread);

}


std::list<PatientInfo> RFPatientsManager::getPage(int num)
{
	if (m_patients.size() < (num + 3) * PATIENT_PAGE_NUMBER && num > m_current_page) {
		step();
	}

	std::list<PatientInfo> pages;
	int i = 0;
	for (std::list<PatientInfo>::iterator iter = m_patients.begin(); iter != m_patients.end(); iter++) {
		

		if (i >= (num + 1)*PATIENT_PAGE_NUMBER) {
			break;
		}
		if (i >= (num)*PATIENT_PAGE_NUMBER) {
			pages.push_back(*iter);
		}
		i++;
	}

	return pages;
}

PatientInfo RFPatientsManager::getPatient(int patientid)
{
	PatientInfo patient;
	patient.id = -1;
	for (std::list<PatientInfo>::iterator iter = m_patients.begin(); iter != m_patients.end(); iter++) {

		if (iter->id == patientid) {
			patient = *iter;
			break;
		}
	}

	return patient;
}

PatientInfo RFPatientsManager::getPatient(int page, int index)
{
	PatientInfo patient;
	patient.id = -1;

	int i = 0;
	for (std::list<PatientInfo>::iterator iter = m_patients.begin(); iter != m_patients.end(); iter++) {


		if (i >= (page + 1)*PATIENT_PAGE_NUMBER) {
			break;
		}
		if (i >= (page)*PATIENT_PAGE_NUMBER && i%PATIENT_PAGE_NUMBER == index) {
			patient = *iter;
			break;
		}
		i++;
	}

	return patient;
}

int RFPatientsManager::OnAddOK(EventArg* pArg)
{
	if (RFPatientsManager::get()->step() == 0) {
		if (RFPatientsManager::get()->m_current_page == -1) {
			RFPatientsManager::get()->setPage(0);
		}

		if (RFMainWindow::MainWindow && RFMainWindow::MainWindow->m_patient_add_page->IsVisible() == true) {

			CLabelUI* pLabel = static_cast<CLabelUI*>(RFMainWindow::MainWindow->m_pm.FindControl(_T("manager_patient_welcom")));
			pLabel->SetText((_T("»¶Ó­Äú£¬") + RFMainWindow::MainWindow->m_login_info.login_user + _T("!¡Å")).c_str());

			RFMainWindow::MainWindow->UpdateManagePatientPage(RFPatientsManager::get()->getPage(RFPatientsManager::get()->m_current_page));
			RFMainWindow::MainWindow->UpdateManagePageNumber(RFPatientsManager::get()->m_current_page);
			RFMainWindow::MainWindow->ShowManagerPatientPage();
		}
	}
	return 1;
}

int RFPatientsManager::OnStepOK(EventArg *pArg)
{
	CTask *pTask = pArg->GetAttach<CTask*>();

	LoadPatientResult *pResult = pTask->GetContext<LoadPatientResult*>();
	if (pResult->num < 0) {
		delete pResult;
		return 1;
	}

	if (pResult->patients.size() > 0) {

		if (RFPatientsManager::get()->m_patients.rbegin() != RFPatientsManager::get()->m_patients.rend()) {
			if (RFPatientsManager::get()->m_patients.rbegin()->id < pResult->patients.begin()->id) {
				for (std::list<PatientInfo>::iterator it = pResult->patients.begin();
					it != pResult->patients.end(); it++) {
						RFPatientsManager::get()->m_patients.push_back(*it);
			 }
		 }
		} else {
			for (std::list<PatientInfo>::iterator it = pResult->patients.begin();
				it != pResult->patients.end(); it++) {
					RFPatientsManager::get()->m_patients.push_back(*it);
		 }
		}
	}


	if (RFPatientsManager::get()->m_current_page == -1) {
		RFPatientsManager::get()->setPage(0);
	}
	
	if (RFMainWindow::MainWindow && RFMainWindow::MainWindow->m_patient_add_page->IsVisible() == true) {

		CLabelUI* pLabel = static_cast<CLabelUI*>(RFMainWindow::MainWindow->m_pm.FindControl(_T("manager_patient_welcom")));
		pLabel->SetText((_T("»¶Ó­Äú£¬") + RFMainWindow::MainWindow->m_login_info.login_user + _T("!¡Å")).c_str());

		RFMainWindow::MainWindow->UpdateManagePatientPage(RFPatientsManager::get()->getPage(RFPatientsManager::get()->m_current_page));
		RFMainWindow::MainWindow->UpdateManagePageNumber(RFPatientsManager::get()->m_current_page);
		RFMainWindow::MainWindow->ShowManagerPatientPage();
	}

	delete pResult;
	pResult = NULL;

	return 1;
}


int RFPatientsManager::OnModifyOK(EventArg *pArg)
{
	CTask *pTask = pArg->GetAttach<CTask*>();
	if (!pTask) {
		return 1;
	}
	
	RFMainWindow* mainWidow = RFMainWindow::MainWindow;
	if (mainWidow) {
		CLabelUI* pLabel = static_cast<CLabelUI*>(mainWidow->m_pm.FindControl(_T("manager_patient_welcom")));
		pLabel->SetText((_T("»¶Ó­Äú£¬") + mainWidow->m_login_info.login_user + _T("!¡Å")).c_str());

		mainWidow->UpdateManagePatientPage(RFPatientsManager::get()->getPage(RFPatientsManager::get()->m_current_page));
		mainWidow->UpdateManagePageNumber(RFPatientsManager::get()->m_current_page);
		mainWidow->ShowManagerPatientPage();
	}
	
	return 1;
}

int RFPatientsManager::OnModifyDoctorOK(EventArg* pArg)
{
	CTask *pTask = pArg->GetAttach<CTask*>();
	if (!pTask) {
		return 1;
	}
	LoginInfo *pResult = pTask->GetContext<LoginInfo*>();
	if (!pResult) {
		return 1;
	}

	RFMainWindow* mainWidow = RFMainWindow::MainWindow;
	if (mainWidow) {
		mainWidow->m_login_info = *pResult;
		
		CLabelUI* pLabel = static_cast<CLabelUI*>(mainWidow->m_pm.FindControl(_T("welcom_menu")));
		pLabel->SetText((_T("»¶Ó­Äú£¬") + mainWidow->m_login_info.login_user + _T("!¡Å")).c_str());

		mainWidow->ShowMainPage();
	}

	delete pResult;
	pResult = NULL;
}

int RFPatientsManager::OnNextPatientIDOK(EventArg* pArg)
{
	CTask *pTask = pArg->GetAttach<CTask*>();
	
	NextPatientResult *pResult = pTask->GetContext<NextPatientResult*>();
	if (pResult->patientid == _T("-1")) {
		return 1;
	}

	RFMainWindow::MainWindow->ShowPatientAdd(pResult->patientid);
	delete pResult;
	pResult = NULL;

	return 1;
}

int RFPatientsManager::OnExportPatientFilterOK(EventArg* pArg)
{
	CWorkThread *pCurrentThread = pArg->GetSender<CWorkThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	if (!pTask) {
		return 1;
	}

	ExportPatientResult *pResult = pTask->GetContext<ExportPatientResult*>();
	if (NULL == pResult || pResult->path == _T("") || pResult->patients.size() < 1) {
		return 1;
	}

	bool bRet = RFPatientsManager::get()->m_excel.OpenExcelFile(pResult->path.c_str());
	CString strSheetName = RFPatientsManager::get()->m_excel.GetSheetName(1);
	bool bLoad = RFPatientsManager::get()->m_excel.LoadSheet(strSheetName);

	int i = 3;
	std::list<PatientInfo>::iterator begin = pResult->patients.begin();
	for (; begin != pResult->patients.end(); begin++) {
		RFPatientsManager::get()->m_excel.SetCellInt(i, 1, begin->id);
		RFPatientsManager::get()->m_excel.SetCellInt(i, 2, begin->hospitalid);
		RFPatientsManager::get()->m_excel.SetCellInt(i, 3, begin->doctorid);
		RFPatientsManager::get()->m_excel.SetCellString(i, 4, begin->name);
		RFPatientsManager::get()->m_excel.SetCellString(i, 5, begin->sex);
		RFPatientsManager::get()->m_excel.SetCellInt(i, 6, begin->age);
		RFPatientsManager::get()->m_excel.SetCellString(i, 7, begin->createtime);
		RFPatientsManager::get()->m_excel.SetCellString(i, 8, begin->lasttreattime);
		RFPatientsManager::get()->m_excel.SetCellString(i, 9, begin->totaltreattime);
		RFPatientsManager::get()->m_excel.SetCellString(i, 10, begin->recoverdetail);
		RFPatientsManager::get()->m_excel.SetCellString(i, 11, begin->remarks);
		i++;
	}

	RFPatientsManager::get()->m_excel.SaveasXSLFile(pResult->savepath.c_str());
	RFPatientsManager::get()->m_excel.CloseExcelFile(FALSE);
}

void RFPatientsManager::exportPatient(int patientid, std::wstring templatepath, std::wstring savepath)
{
	PatientInfo patient = getPatient(patientid);
	bool bRet = m_excel.OpenExcelFile(templatepath.c_str());
	CString strSheetName = m_excel.GetSheetName(1);
	bool bLoad = m_excel.LoadSheet(strSheetName);

	int rownum = 2;
	m_excel.SetCellInt(rownum, 2, patientid);
	m_excel.SetCellString(rownum, 4, patient.createtime);
	rownum++;
	m_excel.SetCellString(rownum, 2, patient.name);
	m_excel.SetCellString(rownum, 4, patient.lasttreattime);
	rownum++;
	m_excel.SetCellString(rownum, 2, patient.sex);
	m_excel.SetCellString(rownum, 4, patient.totaltreattime);
	rownum++;
	m_excel.SetCellInt(rownum, 2, patient.age);
	m_excel.SetCellString(rownum, 4, patient.recoverdetail);
	rownum++;
	m_excel.SetCellString(rownum, 2, patient.remarks);

	m_excel.SaveasXSLFile(savepath.c_str());
	m_excel.CloseExcelFile(FALSE);
}


void RFPatientsManager::importPatient(std::wstring path)
{
	bool bRet = m_excel.OpenExcelFile(path.c_str());
	CString strSheetName = m_excel.GetSheetName(1);
	m_excel.LoadSheet(strSheetName);

	CString title = m_excel.GetCellString(1, 1);
	if (title != _T("ÉÏÖ«Íâ¹Ç÷À¿µ¸´»úÆ÷ÈËÑµÁ·ÏµÍ³")) {
		m_excel.CloseExcelFile(FALSE);
		return;
	}

	std::wstring tableheader = m_excel.GetCellString(2,2);
	if (tableheader != _T("Ò½Ôº±àºÅ")) {
		m_excel.CloseExcelFile(FALSE);
		return;
	}

	tableheader = m_excel.GetCellString(2,3);
	if (tableheader != _T("Ò½Éú±àºÅ")) {
		m_excel.CloseExcelFile(FALSE);
		return;
	}

	tableheader = m_excel.GetCellString(2,4);
	if (tableheader != _T("ÐÕÃû")) {
		m_excel.CloseExcelFile(FALSE);
		return;
	}

	tableheader = m_excel.GetCellString(2,5);
	if (tableheader != _T("ÐÔ±ð")) {
		m_excel.CloseExcelFile(FALSE);
		return;
	}

	int nRowCount = m_excel.GetRowCount();
	int nColumnCount = m_excel.GetColumnCount();
	if (nColumnCount >= 11) {
		std::list<PatientInfo> patients;
		for (int i = 3; i < (nRowCount+1); i++) {
			PatientInfo patient;

			patient.hospitalid = m_excel.GetCellInt(i, 2);
			patient.doctorid = m_excel.GetCellInt(i, 3);
			patient.name = m_excel.GetCellString(i, 4);
			patient.sex = m_excel.GetCellString(i, 5);
			patient.age = m_excel.GetCellInt(i, 6);
			patient.createtime = m_excel.GetCellString(i, 7, true);
			patient.lasttreattime = m_excel.GetCellString(i, 8, true);
			patient.totaltreattime = m_excel.GetCellString(i, 9);
			patient.recoverdetail = m_excel.GetCellString(i, 10);
			patient.remarks = m_excel.GetCellString(i, 11);

			if (patient.name.empty()) {
				break;
			}

			patients.push_back(patient);
		}

		LoadPatientResult* pParam = new LoadPatientResult;
		pParam->patients = patients;

		CTask::Assign(CTask::NotWait, Panic(), pParam, EventHandle(&RFMySQLThread::AddMultiPatients), RFMainWindow::UIThread, RFMainWindow::DBThread);
	}

	m_excel.CloseExcelFile(FALSE);
}