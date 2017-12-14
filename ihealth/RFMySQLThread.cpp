#include "StdAfx.h"
#include "RFMySQLThread.h"
#include "RFMainWindow.h"
#include "RFCharSet.h"
#include "RFPatientsManager.h"
#include "RFPatientsTrainInfo.h"
#include "RFPatientsTrainDetails.h"
#include "RFPassiveTrain.h"
#include "RFPatientsTrainData.h"
#include "RFTTSSample.h"
#include "RFEvaluationData.h"
//#include "cJSON.h"
#include <sstream>

void DecodeMovements(const std::string& movements, std::vector<double>& moves)
{
	std::string src = movements;

	size_t pos = src.find(",");
	size_t size = src.size();

	std::string tmp = "";
	while (pos != std::string::npos) {
		tmp = src.substr(0,pos);

		moves.push_back(atof(tmp.c_str()));
		src = src.substr(pos+1,size);
		pos = src.find(",");
	}
}

std::string EncodeMovements(const std::vector<double>& moves)
{
	std::string movements;

	std::stringstream ss;
	double value = .0f;
	for (int i = 0; i < moves.size(); i++) {
		value = moves.at(i);

		ss << value;

		ss << ",";
	}
	movements = ss.str();
	ss.clear();

	return movements;
}

std::string RFReplaceString(std::string & src, std::string find, std::string replacement)  
{  
	std::string dest;  
	int  i = 0;  
	int len = find.length();  

	while(true)  
	{  
		int start = i;  
		i = src.find_first_of(find, i);  

		if(i < 0)  
		{         
			dest += src.substr(start);        
			break;  
		}  

		dest += src.substr(start, i - start);  
		dest += replacement;  
		i += len;  
	}  

	return dest;  
}  

int RFSymbolNum(std::string & src, std::string find)  
{  
	int nSymbolNum = 0;

	int  i = 0;  
	int len = find.length();  

	while(true)  
	{  
		int start = i;  
		i = src.find_first_of(find, i);  

		if(i < 0)  
		{          
			break;  
		}  

		i += len; 
		nSymbolNum++;
	}  

	return nSymbolNum;  
}  

RFMySQLThread::RFMySQLThread(void)
{
	m_patientFilter.age_from = _T("");
	m_patientFilter.age_to = _T("");
	m_patientFilter.create_from = _T("1900-01-01");
	m_patientFilter.create_to = _T("2100-01-01");
	m_patientFilter.name = _T("");
	m_patientFilter.sex = _T("");
}

RFMySQLThread::~RFMySQLThread(void)
{
	//this->OnDispose();

	if (m_db.IsOpen()) {
		m_db.Close();
	}
}

RFMySQLThread* RFMySQLThread::Create()
{
	RFMySQLThread *pThread = new RFMySQLThread();
	pThread->m_Name=_T("RFMySQLThread");
	pThread->InitialThread();

	CTask::Assign(CTask::NotWait, Panic(), NULL, EventHandle(&RFMySQLThread::Connect), RFMainWindow::UIThread, pThread);
	return pThread;
}

void RFMySQLThread::Release(RFMySQLThread *&pThread)
{
	if (pThread != NULL)
	{
		delete pThread;
		pThread = NULL;
	}
}

int RFMySQLThread::Connect(EventArg *pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();

	if (pCurrentThread->m_db.Open("139.224.24.116", "rfhk", "runfeng168", "instruments", 3306) < 0)
	{
		CTask::Assign(CTask::NotWait, Panic(), NULL, EventHandle(&RFMainWindow::OnConnectFail), pCurrentThread, RFMainWindow::UIThread);
		return 1;
	}

	RFMYSQLStmt stmt;
	stmt.Prepare(pCurrentThread->m_db, "set names utf8");
	stmt.Finalize();
	CTask::Assign(CTask::NotWait, Panic(), NULL, EventHandle(&RFMainWindow::OnConnectOK), pCurrentThread, RFMainWindow::UIThread);
	return 1;
}

int RFMySQLThread::ReConnect(EventArg *pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();

	if (pCurrentThread->m_db.Reopen() < 0)
	{
		CTask::Assign(CTask::NotWait, Panic(), NULL, EventHandle(&RFMainWindow::OnConnectFail), pCurrentThread, RFMainWindow::UIThread);
		return 1;
	}
	RFMYSQLStmt stmt;
	stmt.Prepare(pCurrentThread->m_db, "set names utf8");
	stmt.Finalize();

	CTask::Assign(CTask::NotWait, Panic(), NULL, EventHandle(&RFMainWindow::OnConnectOK), pCurrentThread, RFMainWindow::UIThread);
	return 1;
}

int RFMySQLThread::Login(EventArg *pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();

	std::wstring name = L"";
	int			login_state = -1;

	LoginInfo *pLoginInfo = pTask->GetContext<LoginInfo*>();
	if (pLoginInfo != NULL) {
		std::string usr = TGUTF16ToUTF8(pLoginInfo->usrname);
		std::string pwd = TGUTF16ToUTF8(pLoginInfo->usrpwd);
		int role = pLoginInfo->role;

		usr = RFReplaceString(usr, "\\", "\\\\");
		pwd = RFReplaceString(pwd, "\\", "\\\\");
		usr = RFReplaceString(usr, "'", "\\'");
		pwd = RFReplaceString(pwd, "'", "\\'");

		char sql[1024] = "";
		sprintf(sql, "select id from logininfo where username='%s' and pwd='%s' and role='%d'", usr.c_str(), pwd.c_str(), role);

		RFMYSQLStmt stmt;
		if (stmt.Prepare(RFMainWindow::DBThread->m_db, sql) > 0) {
			login_state = -2;
			if (stmt.Step() > 0) {
				login_state = -3;
				int id = stmt.GetInt(0);
				stmt.Finalize();

				if (id > 0) {
					char fields[256] = "id,loginid,hospitalid,employeenumber,name,sex,birthday,certid,telephone,bloodtype,address,education,nation,birthplace,householdprop,department,projgroup,positon,entrydate,flag,createtime";
					memset(sql, 0, 1024);
					sprintf(sql, "select %s from doctor where loginid=%d", fields, id);

					if (stmt.Prepare(RFMainWindow::DBThread->m_db, sql) > 0) {
						if (stmt.Step() > 0) {
							pLoginInfo->doctorid = stmt.GetInt(0);
							pLoginInfo->login_id = stmt.GetInt(1);
							pLoginInfo->hospitalid = stmt.GetInt(2);
							pLoginInfo->employeenumber = TGUTF8ToUTF16(stmt.GetString(3));
							pLoginInfo->name = TGUTF8ToUTF16(stmt.GetString(4));
							pLoginInfo->login_user = pLoginInfo->name;
							pLoginInfo->sex = TGUTF8ToUTF16(stmt.GetString(5));
							pLoginInfo->birthday = TGUTF8ToUTF16(stmt.GetString(6));
							pLoginInfo->certid = TGUTF8ToUTF16(stmt.GetString(7));
							pLoginInfo->telephone = TGUTF8ToUTF16(stmt.GetString(8));
							pLoginInfo->bloodtype = stmt.GetInt(9);
							pLoginInfo->address = TGUTF8ToUTF16(stmt.GetString(10));
							pLoginInfo->education = TGUTF8ToUTF16(stmt.GetString(11));
							pLoginInfo->nation = TGUTF8ToUTF16(stmt.GetString(12));
							pLoginInfo->birthplace = TGUTF8ToUTF16(stmt.GetString(13));
							pLoginInfo->householdprop = TGUTF8ToUTF16(stmt.GetString(14));
							pLoginInfo->department = TGUTF8ToUTF16(stmt.GetString(15));
							pLoginInfo->group = TGUTF8ToUTF16(stmt.GetString(16));
							pLoginInfo->positon = TGUTF8ToUTF16(stmt.GetString(17));
							pLoginInfo->entrydate = TGUTF8ToUTF16(stmt.GetString(18));
							pLoginInfo->flag = stmt.GetInt(19);
							pLoginInfo->createtime = TGUTF8ToUTF16(stmt.GetString(20));
							
							login_state = 1;
						}
					}

					stmt.Finalize();
				}
			}
			stmt.Finalize();
		}
	}

	pLoginInfo->logined = login_state;
	
	CTask::Assign(CTask::NotWait, Panic(), pLoginInfo, EventHandle(&RFMainWindow::OnLoginOK), pCurrentThread, RFMainWindow::UIThread);
	return 1;
}

int RFMySQLThread::SearchPatient(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	SearchPatientParam *pParam = pTask->GetContext<SearchPatientParam*>();
	if (!pParam) {
		return 1;
	}

	LoadPatientResult* pResult = new LoadPatientResult;
	char sql[1024] = "";
	char fields[128] = "id,hospitalid,doctorid,name,sex,age,createtime,lasttreattime,totaltreattime,recoverdetail,remarks,flag";
	sprintf(sql, "select %s from patient where name='%s' and flag=0",fields, TGUTF16ToUTF8(pParam->patientname).c_str());

	RFMYSQLStmt stmt;
	if (stmt.Prepare(RFMainWindow::DBThread->m_db, sql) > 0) {
		while (stmt.Step() > 0) {
			PatientInfo patient;

			patient.id = stmt.GetInt(0);
			patient.hospitalid = stmt.GetInt(1);
			patient.doctorid = stmt.GetInt(2);
			patient.name = TGUTF8ToUTF16(stmt.GetString(3));
			patient.sex = TGUTF8ToUTF16(stmt.GetString(4));
			patient.age = stmt.GetInt(5);
			patient.createtime = TGUTF8ToUTF16(stmt.GetString(6));
			patient.lasttreattime = TGUTF8ToUTF16(stmt.GetString(7));
			patient.totaltreattime = TGUTF8ToUTF16(stmt.GetString(8));
			patient.recoverdetail = TGUTF8ToUTF16(stmt.GetString(9));
			patient.remarks = TGUTF8ToUTF16(stmt.GetString(10));

			pResult->patients.push_back(patient);
		}

		stmt.Finalize();

		pResult->num = pResult->patients.size();
	}

	delete pParam;
	pParam = NULL;


	CTask::Assign(CTask::NotWait, Panic(), pResult, EventHandle(&RFMainWindow::OnSearchOK), pCurrentThread, RFMainWindow::UIThread);
	return 1;
}


int RFMySQLThread::FilterPatient(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	PatientFilterParam *pParam = pTask->GetContext<PatientFilterParam*>();
	if (!pParam) {
		return 1;
	}

	std::string condition = "";
	if (pParam->name != _T("")) {
		condition += " and name='"+TGUTF16ToUTF8(pParam->name) + "'";
	}

	if (pParam->sex == _T("男") || pParam->sex == _T("女")) {
		condition += " and sex='" + TGUTF16ToUTF8(pParam->sex) + "'";
	}

	int age_from = _wtoi(pParam->age_from.c_str());
	if (age_from > 0) {
		condition += " and age>=" + TGUTF16ToUTF8(pParam->age_from);
	}

	int age_to = _wtoi(pParam->age_to.c_str());
	if (age_to > 0) {
		condition += " and age<=" + TGUTF16ToUTF8(pParam->age_to);
	}

	if (pParam->create_from != _T("")) {
		condition += " and createtime>='" + TGUTF16ToUTF8(pParam->create_from) + "'";
	}

	if (pParam->create_to != _T("")) {
		condition += " and createtime<='" + TGUTF16ToUTF8(pParam->create_to) + "'";
	}
	
	std::string fields = "select id,hospitalid,doctorid,name,sex,age,createtime,lasttreattime,totaltreattime,recoverdetail,remarks,flag from patient where flag=0";
	char condition1[1024];
	sprintf(condition1, " and hospitalid=%d and doctorid=%d and flag=0 ", pParam->hospitalid, pParam->doctorid);
	std::string sql = "";
	sql += fields;
	sql += condition1;
	sql += condition;

	RFMainWindow::DBThread->m_patientFilter = *pParam;
	LoadPatientResult* pResult = new LoadPatientResult;
	RFMYSQLStmt stmt;
	if (stmt.Prepare(RFMainWindow::DBThread->m_db, sql.c_str()) > 0) {
		while (stmt.Step() > 0) {
			PatientInfo patient;
			patient.id = stmt.GetInt(0);
			patient.hospitalid = stmt.GetInt(1);
			patient.doctorid = stmt.GetInt(2);
			patient.name = TGUTF8ToUTF16(stmt.GetString(3));
			patient.sex = TGUTF8ToUTF16(stmt.GetString(4));
			patient.age = stmt.GetInt(5);
			patient.createtime = TGUTF8ToUTF16(stmt.GetString(6));
			patient.lasttreattime = TGUTF8ToUTF16(stmt.GetString(7));
			patient.totaltreattime = TGUTF8ToUTF16(stmt.GetString(8));
			patient.recoverdetail = TGUTF8ToUTF16(stmt.GetString(9));
			patient.remarks = TGUTF8ToUTF16(stmt.GetString(10));

			pResult->patients.push_back(patient);
		}

		stmt.Finalize();

		pResult->num = pResult->patients.size();
	}

	delete pParam;
	pParam = NULL;


	CTask::Assign(CTask::NotWait, Panic(), pResult, EventHandle(&RFMainWindow::OnFilterOK), pCurrentThread, RFMainWindow::UIThread);
	return 1;
}

int RFMySQLThread::ExportPatient(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	ExportPatientParam *pParam = pTask->GetContext<ExportPatientParam*>();
	if (!pParam) {
		return 1;
	}

	std::string condition = "";

	if (pParam->patientid <= 0) {
		PatientFilterParam param = RFMainWindow::DBThread->m_patientFilter;
		if (param.name != _T("")) {
			condition += " and name='"+TGUTF16ToUTF8(param.name) + "'";
		}

		if (param.sex == _T("男") || param.sex == _T("女")) {
			condition += " and sex='" + TGUTF16ToUTF8(param.sex) + "'";
		}

		int age_from = _wtoi(param.age_from.c_str());
		if (age_from > 0) {
			condition += " and age>=" + TGUTF16ToUTF8(param.age_from);
		}

		int age_to = _wtoi(param.age_to.c_str());
		if (age_to > 0) {
			condition += " and age<=" + TGUTF16ToUTF8(param.age_to);
		}

		if (param.create_from != _T("")) {
			condition += " and createtime>='" + TGUTF16ToUTF8(param.create_from) + "'";
		}

		if (param.create_to != _T("")) {
			condition += " and createtime<='" + TGUTF16ToUTF8(param.create_to) + "'";
		}
	} else {
		char tmp[128] = "";
		sprintf(tmp, " and id=%d ", pParam->patientid);
		condition = tmp;
	}
	

	std::string fields = "select id,hospitalid,doctorid,name,sex,age,createtime,lasttreattime,totaltreattime,recoverdetail,remarks,flag from patient where flag=0";
	char condition1[1024];
	sprintf(condition1, " and hospitalid=%d and doctorid=%d and flag=0 ", pParam->hospitalid, pParam->doctorid);
	std::string sql = "";
	sql += fields;
	sql += condition1;
	sql += condition;

	ExportPatientResult* pResult = new ExportPatientResult;
	pResult->path = pParam->path;
	pResult->savepath = pParam->savepath;
	RFMYSQLStmt stmt;
	if (stmt.Prepare(RFMainWindow::DBThread->m_db, sql.c_str()) > 0) {
		while (stmt.Step() > 0) {
			PatientInfo patient;
			patient.id = stmt.GetInt(0);
			patient.hospitalid = stmt.GetInt(1);
			patient.doctorid = stmt.GetInt(2);
			patient.name = TGUTF8ToUTF16(stmt.GetString(3));
			patient.sex = TGUTF8ToUTF16(stmt.GetString(4));
			patient.age = stmt.GetInt(5);
			patient.createtime = TGUTF8ToUTF16(stmt.GetString(6));
			patient.lasttreattime = TGUTF8ToUTF16(stmt.GetString(7));
			patient.totaltreattime = TGUTF8ToUTF16(stmt.GetString(8));
			patient.recoverdetail = TGUTF8ToUTF16(stmt.GetString(9));
			patient.remarks = TGUTF8ToUTF16(stmt.GetString(10));

			pResult->patients.push_back(patient);
		}

		stmt.Finalize();

		pResult->num = pResult->patients.size();
	}

	delete pParam;
	pParam = NULL;


	CTask::Assign(CTask::NotWait, Panic(), pResult, EventHandle(&RFPatientsManager::OnExportPatientFilterOK), pCurrentThread, CWorkThread::GetWorker());

}

int RFMySQLThread::Load(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	LoadPatientParam *pParam = pTask->GetContext<LoadPatientParam*>();
	if (!pParam) {
		return 1;
	}

	PatientFilterParam filter = RFMainWindow::DBThread->m_patientFilter;
	std::string condition = "";
	if (filter.name != _T("")) {
		condition += " and name='"+TGUTF16ToUTF8(filter.name) + "'";
	}

	if (filter.sex == _T("男") || filter.sex == _T("女")) {
		condition += " and sex='" + TGUTF16ToUTF8(filter.sex) + "'";
	}

	int age_from = _wtoi(filter.age_from.c_str());
	if (age_from > 0) {
		condition += " and age>=" + TGUTF16ToUTF8(filter.age_from);
	}

	int age_to = _wtoi(filter.age_to.c_str());
	if (age_to > 0) {
		condition += " and age<=" + TGUTF16ToUTF8(filter.age_to);
	}

	if (filter.create_from != _T("")) {
		condition += " and createtime>='" + TGUTF16ToUTF8(filter.create_from) + "'";
	}

	if (filter.create_to != _T("")) {
		condition += " and createtime<='" + TGUTF16ToUTF8(filter.create_to) + "'";
	}

	//char sql[1024] = "";
	char fields[256] = "select id,hospitalid,doctorid,name,sex,age,createtime,lasttreattime,totaltreattime,recoverdetail,remarks,flag from patient where ";
	char condition1[1024];
	sprintf(condition1, " hospitalid=%d and doctorid=%d and flag=0 ", pParam->hospitalid, pParam->doctorid);
	char limit[128];
	sprintf(limit, " limit %d,%d", pParam->start, pParam->num);
	std::string sql = "";
	sql += fields;
	sql += condition1;
	sql += condition;
	sql += limit;
	
	LoadPatientResult *pResult = new LoadPatientResult();
	pResult->num = -1;

	RFMYSQLStmt stmt;
	if (stmt.Prepare(RFMainWindow::DBThread->m_db, sql.c_str()) > 0) {
		pResult->num = 0;

		std::list<PatientInfo> patients;
		while(stmt.Step() > 0) {
			PatientInfo patient;
			
			patient.id = stmt.GetInt(0);
			patient.hospitalid = stmt.GetInt(1);
			patient.doctorid = stmt.GetInt(2);
			patient.name = TGUTF8ToUTF16(stmt.GetString(3));
			patient.sex = TGUTF8ToUTF16(stmt.GetString(4));
			patient.age = stmt.GetInt(5);
			patient.createtime = TGUTF8ToUTF16(stmt.GetString(6));
			patient.lasttreattime = TGUTF8ToUTF16(stmt.GetString(7));
			patient.totaltreattime = TGUTF8ToUTF16(stmt.GetString(8));
			patient.recoverdetail = TGUTF8ToUTF16(stmt.GetString(9));
			patient.remarks = TGUTF8ToUTF16(stmt.GetString(10));

			patients.push_back(patient);
		}
		stmt.Finalize();

		pResult->num = patients.size();
		pResult->patients = patients;
	}

	delete pParam;
	pParam = NULL;


	CTask::Assign(CTask::NotWait, Panic(), pResult, EventHandle(&RFPatientsManager::OnStepOK), pCurrentThread, RFMainWindow::UIThread);
	return 1;
}


int RFMySQLThread::Save(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	PatientInfo *pParam = pTask->GetContext<PatientInfo*>();
	if (!pParam) {
		return 1;
	}
	
	wchar_t fields[256] = _T("");
	wsprintf(fields, _T("hospitalid=%d, doctorid=%d, age=%d, flag=%d where id=%d"), pParam->hospitalid, pParam->doctorid, pParam->age, pParam->flag, pParam->id);

	std::wstring sql = _T("update patient set ");
	sql += _T("name='") + pParam->name + _T("',");
	sql += _T("sex='") + pParam->sex + _T("',");
	sql += _T("createtime='") + pParam->createtime + _T("',");
	sql += _T("lasttreattime='") + pParam->lasttreattime + _T("',");
	sql += _T("totaltreattime='") + pParam->totaltreattime + _T("',");
	sql += _T("recoverdetail='") + pParam->recoverdetail + _T("',");
	sql += _T("remarks='") + pParam->remarks + _T("',");
	sql += fields;
	
	RFMainWindow::DBThread->m_db.Exec(TGUTF16ToUTF8(sql).c_str());

	delete pParam;
	pParam = NULL;

	CTask::Assign(CTask::NotWait, Panic(), NULL, EventHandle(&RFPatientsManager::OnModifyOK), pCurrentThread, RFMainWindow::UIThread);

	return 1;
}

int RFMySQLThread::Add(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	PatientInfo *pParam = pTask->GetContext<PatientInfo*>();
	if (!pParam) {
		return 1;
	}
	wchar_t age[64] = _T("");
	wsprintf(age, _T("%d,"), pParam->age);

	wchar_t hospitalid[64] = _T("");
	wsprintf(hospitalid, _T("%d,"), pParam->hospitalid);

	wchar_t doctorid[64] = _T("");
	wsprintf(doctorid, _T("%d,"), pParam->doctorid);

	std::wstring sql = _T("INSERT INTO patient (hospitalid,doctorid,name,sex,age,createtime,lasttreattime,totaltreattime,recoverdetail,remarks,flag) value(");
	sql += hospitalid;
	sql += doctorid,
	sql += _T("'") + pParam->name + _T("',");
	sql += _T("'") + pParam->sex + _T("',");
	sql += age;
	sql += _T("'") + pParam->createtime + _T("',");
	sql += _T("'") + pParam->lasttreattime + _T("',");
	sql += _T("'") + pParam->totaltreattime + _T("',");
	sql += _T("'") + pParam->recoverdetail + _T("',");
	sql += _T("'") + pParam->remarks + _T("',");
	sql += _T("0)");

	RFMainWindow::DBThread->m_db.Exec(TGUTF16ToUTF8(sql).c_str());

	CTask::Assign(CTask::NotWait, Panic(), NULL, EventHandle(&RFPatientsManager::OnAddOK), pCurrentThread, RFMainWindow::UIThread);

	delete pParam;
	pParam = NULL;
	return 1;
}

int RFMySQLThread::AddMultiPatients(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	LoadPatientResult *pParam = pTask->GetContext<LoadPatientResult*>();
	if (!pParam) {
		return 1;
	}

	std::list<PatientInfo>::iterator begin = pParam->patients.begin();
	for (; begin != pParam->patients.end(); begin++) {
		wchar_t age[64] = _T("");
		wsprintf(age, _T("%d,"), begin->age);

		wchar_t hospitalid[64] = _T("");
		wsprintf(hospitalid, _T("%d,"), begin->hospitalid);

		wchar_t doctorid[64] = _T("");
		wsprintf(doctorid, _T("%d,"), begin->doctorid);

		std::wstring sql = _T("INSERT INTO patient (hospitalid,doctorid,name,sex,age,createtime,lasttreattime,totaltreattime,recoverdetail,remarks,flag) value(");
		sql += hospitalid;
		sql += doctorid,
		sql += _T("'") + begin->name + _T("',");
		sql += _T("'") + begin->sex + _T("',");
		sql += age;
		sql += _T("'") + begin->createtime + _T("',");
		sql += _T("'") + begin->lasttreattime + _T("',");
		sql += _T("'") + begin->totaltreattime + _T("',");
		sql += _T("'") + begin->recoverdetail + _T("',");
		sql += _T("'") + begin->remarks + _T("',");
		sql += _T("0)");

		RFMainWindow::DBThread->m_db.Exec(TGUTF16ToUTF8(sql).c_str());
	}

	CTask::Assign(CTask::NotWait, Panic(), NULL, EventHandle(&RFPatientsManager::OnAddOK), pCurrentThread, RFMainWindow::UIThread);

	delete pParam;
	pParam = NULL;
	return 1;
}

int RFMySQLThread::Delete(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	PatientInfo *pParam = pTask->GetContext<PatientInfo*>();
	if (!pParam) {
		return 1;
	}

	wchar_t sql[256] = _T("");
	wsprintf(sql, _T("update patient set flag=1 where id=%d"), pParam->id);

	RFMainWindow::DBThread->m_db.Exec(TGUTF16ToUTF8(sql).c_str());

	delete pParam;
	pParam = NULL;
	return 1;
}

int RFMySQLThread::NextPatientID(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	if (!pCurrentThread) {
		return 1;
	}

	NextPatientResult *pResult = new NextPatientResult;
	pResult->patientid = _T("-1");

	RFMYSQLStmt stmt;
	if (stmt.Prepare(RFMainWindow::DBThread->m_db, "SELECT AUTO_INCREMENT FROM information_schema.tables where TABLE_NAME='patient'") > 0) {

		if (stmt.Step() > 0) {
			wchar_t patient_id[256];
			wsprintf(patient_id, _T("%d"), stmt.GetInt(0));

			pResult->patientid = patient_id;
		}
	}

	CTask::Assign(CTask::NotWait, Panic(), pResult, EventHandle(&RFPatientsManager::OnNextPatientIDOK), pCurrentThread, RFMainWindow::UIThread);
}

int RFMySQLThread::ModifyPWD(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	ModifyPWDInfo *pParam = pTask->GetContext<ModifyPWDInfo*>();
	if (!pParam) {
		return 1;
	}

	wchar_t fields[256] = _T("");
	wsprintf(fields, _T(" where id=%d and pwd='%s'"), pParam->loginid, pParam->oldpwd.c_str());
	std::wstring sql = _T("update logininfo set ");
	sql += _T("pwd='") + pParam->pwd + _T("' ");
	sql += fields;

	RFMainWindow::DBThread->m_db.Exec(TGUTF16ToUTF8(sql).c_str());

	delete pParam;
	pParam = NULL;
	return 1;
}

int RFMySQLThread::SaveDoctor(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	LoginInfo *pParam = pTask->GetContext<LoginInfo*>();
	if (!pParam) {
		return 1;
	}

	wchar_t fields[256] = _T("");
	wsprintf(fields, _T("bloodtype=%d where id=%d"), pParam->bloodtype, pParam->doctorid);

	std::wstring sql = _T("update doctor set ");
	sql += _T("employeenumber='") + pParam->employeenumber + _T("',");
	sql += _T("name='") + pParam->name + _T("',");
	sql += _T("sex='") + pParam->sex + _T("',");
	sql += _T("birthday='") + pParam->birthday + _T("',");
	sql += _T("certid='") + pParam->certid + _T("',");
	sql += _T("telephone='") + pParam->telephone + _T("',");
	sql += _T("address='") + pParam->address + _T("',");
	sql += _T("education='") + pParam->education + _T("',");
	sql += _T("nation='") + pParam->nation + _T("',");
	sql += _T("birthplace='") + pParam->birthplace + _T("',");
	sql += _T("householdprop='") + pParam->householdprop + _T("',");
	sql += _T("department='") + pParam->department + _T("',");
	sql += _T("projgroup='") + pParam->group + _T("',");
	sql += _T("positon='") + pParam->positon + _T("',");
	sql += _T("entrydate='") + pParam->entrydate + _T("',");
	sql += fields;

	RFMainWindow::DBThread->m_db.Exec(TGUTF16ToUTF8(sql).c_str());

	CTask::Assign(CTask::NotWait, Panic(), pParam, EventHandle(&RFPatientsManager::OnModifyDoctorOK), pCurrentThread, RFMainWindow::UIThread);

}

int RFMySQLThread::AddPatientTrainDetails(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	PatientTrainDetails *pParam = pTask->GetContext<PatientTrainDetails*>();
	if (!pParam) {
		return 1;
	}

	int detailid = 0;
	RFMYSQLStmt stmt;
	if (stmt.Prepare(RFMainWindow::DBThread->m_db, "SELECT AUTO_INCREMENT FROM information_schema.tables where TABLE_NAME='patienttrain'") > 0) {

		if (stmt.Step() > 0) {
			detailid = stmt.GetInt(0);
		}
	}

	wchar_t patientid[64] = _T("");
	wsprintf(patientid, _T("%d,"), pParam->patientid);

	std::wstring sql = _T("INSERT INTO patienttrain (patientid,traindate,content,duration,traintime,createtime) value(");
	sql += patientid;
	sql += _T("'") + pParam->traindate + _T("',");
	sql += _T("'") + pParam->content + _T("',");
	sql += _T("'") + pParam->duration + _T("',");
	sql += _T("'") + pParam->traintime + _T("',");
	sql += _T("'") + pParam->createtime + _T("')");
	RFMainWindow::DBThread->m_db.Exec(TGUTF16ToUTF8(sql).c_str());


	{
		char szSQL[512] = "";
		sprintf(szSQL, "SELECT totaltreattime FROM patient WHERE id = %d", pParam->patientid);
		
		RFMYSQLStmt stmt1;
		if (stmt1.Prepare(RFMainWindow::DBThread->m_db, szSQL) > 0) {

			if (stmt1.Step() > 0) {
				std::string totaltreattime = stmt1.GetString(0);

				
				std::string traintime = RFReplaceString(TGUTF16ToUTF8(pParam->totaltreattime), ":", ",");
				int v1 = -1, v2 = -1, v3 = -1;
				LPSTR pstr = NULL;
				v1 = strtol(traintime.c_str(), &pstr, 10);
				if (pstr > 0) {
					v2 = strtol(pstr+1, &pstr, 10);
				} 
				if (pstr) {
					v3 = strtol(pstr+1, &pstr, 10);
				}


				int nSymbolNum = RFSymbolNum(TGUTF16ToUTF8(pParam->totaltreattime), ":");
				int totaltime = 0;
				if (nSymbolNum == 1) {
					totaltime = 60 * v1 + v2;
				} else if (nSymbolNum == 2) {
					totaltime = 60*60 * v1 + 60*v2 + v3;
				}

				double treattime = atof(totaltreattime.c_str());
				treattime += ((double)totaltime / (double)3600);

				char szsql[1024];
				sprintf(szsql, "UPDATE patient SET lasttreattime='%s', totaltreattime='%.3f%s', recoverdetail='%s' where id=%d", 
					TGUTF16ToUTF8(pParam->lasttreattime).c_str(), treattime, TGUTF16ToUTF8(_T("小时")).c_str(), TGUTF16ToUTF8(pParam->recoverdetail).c_str(), pParam->patientid);
				RFMainWindow::DBThread->m_db.Exec(szsql);
			}
		}
	}
	
	wchar_t traindetailid[64] = _T("");
	wsprintf(traindetailid, _T("%d,"), detailid);

	sql = _T("");
	wchar_t traintype[32];
	std::wstring tracevalue;
	if (pParam->traintype == RF_TRAINTYPE_STRING_BD)
	{
		wsprintf(traintype, _T("%d"), 4);	// 肩关节
		tracevalue = TGUTF8ToUTF16(EncodeMovements(pParam->target_vel[0]));

		sql = _T("INSERT INTO traindata (traindetailid,tracevalue,traintype) value(");
		sql += traindetailid;
		sql += _T("'") + tracevalue + _T("',");
		sql += traintype;
		sql += _T(")");

		RFMainWindow::DBThread->m_db.Exec(TGUTF16ToUTF8(sql).c_str());


		wsprintf(traintype, _T("%d"), 5);	// 肘关节
		tracevalue = TGUTF8ToUTF16(EncodeMovements(pParam->target_vel[1]));

		sql = _T("INSERT INTO traindata (traindetailid,tracevalue,traintype) value(");
		sql += traindetailid;
		sql += _T("'") + tracevalue + _T("',");
		sql += traintype;
		sql += _T(")");

		RFMainWindow::DBThread->m_db.Exec(TGUTF16ToUTF8(sql).c_str());
	} else if (pParam->traintype == RF_TRAINTYPE_STRING_EMG) {
		std::vector<double> emg;
		for (int i = 0; i < pParam->emg_signal[0].size(); i++) {
			emg.push_back(pParam->emg_signal[0].at(i));
			emg.push_back(pParam->emg_signal[1].at(i));
			emg.push_back(pParam->emg_signal[2].at(i));
			emg.push_back(pParam->emg_signal[3].at(i));
		}

		wsprintf(traintype, _T("%d"), 3);	// EMG
		tracevalue = TGUTF8ToUTF16(EncodeMovements(emg));

		sql = _T("INSERT INTO traindata (traindetailid,tracevalue,traintype) value(");
		sql += traindetailid;
		sql += _T("'") + tracevalue + _T("',");
		sql += traintype;
		sql += _T(")");


		std::vector<double> angle;
		for (int i = 0; i < pParam->emg_angle[0].size(); i++) {
			angle.push_back(pParam->emg_angle[0].at(i));
			angle.push_back(pParam->emg_angle[1].at(i));
		}
		wsprintf(traintype, _T("%d"), 1);	// 运动角度
		tracevalue = TGUTF8ToUTF16(EncodeMovements(angle));

		sql = _T("INSERT INTO traindata (traindetailid,tracevalue,traintype) value(");
		sql += traindetailid;
		sql += _T("'") + tracevalue + _T("',");
		sql += traintype;
		sql += _T(")");

		RFMainWindow::DBThread->m_db.Exec(TGUTF16ToUTF8(sql).c_str());
	}  else if (pParam->traintype == RF_TRAINTYPE_STRING_ZD) {
		wsprintf(traintype, _T("%d"), 1);	// 运动角度
		tracevalue = TGUTF8ToUTF16(EncodeMovements(pParam->target_angle));

		sql = _T("INSERT INTO traindata (traindetailid,tracevalue,traintype) value(");
		sql += traindetailid;
		sql += _T("'") + tracevalue + _T("',");
		sql += traintype;
		sql += _T(")");

		RFMainWindow::DBThread->m_db.Exec(TGUTF16ToUTF8(sql).c_str());

		wsprintf(traintype, _T("%d"), 2);	// 握力
		tracevalue = TGUTF8ToUTF16(EncodeMovements(pParam->target_wl));

		sql = _T("INSERT INTO traindata (traindetailid,tracevalue,traintype) value(");
		sql += traindetailid;
		sql += _T("'") + tracevalue + _T("',");
		sql += traintype;
		sql += _T(")");

		RFMainWindow::DBThread->m_db.Exec(TGUTF16ToUTF8(sql).c_str());
	} else if(pParam->traintype == RF_TRAINTYPE_STRING_YD) {
		wsprintf(traintype, _T("%d"), 1);	// 运动角度
		tracevalue = TGUTF8ToUTF16(EncodeMovements(pParam->target_angle));

		sql = _T("INSERT INTO traindata (traindetailid,tracevalue,traintype) value(");
		sql += traindetailid;
		sql += _T("'") + tracevalue + _T("',");
		sql += traintype;
		sql += _T(")");

		RFMainWindow::DBThread->m_db.Exec(TGUTF16ToUTF8(sql).c_str());
	}

	delete pParam;	
	pParam = NULL;
	return 0;
}

int RFMySQLThread::LoadPatientTrainInfo(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	LoadPatientTrainInfoParam *pParam = pTask->GetContext<LoadPatientTrainInfoParam*>();
	if (!pParam) {
		return 1;
	}

	std::string sql = "select DISTINCT(a.patientid), b.name, b.sex, b.age, b.createtime from patienttrain a, patient b where a.patientid = b.id and b.flag=0 ";
	char condition[256];
	sprintf(condition, " and b.hospitalid=%d and b.doctorid=%d ", pParam->hospitalid, pParam->doctorid);
	sql += condition;
	

	LoadPatientTrainInfoResult *pResult = new LoadPatientTrainInfoResult;
	RFMYSQLStmt stmt;
	if (stmt.Prepare(RFMainWindow::DBThread->m_db, sql.c_str()) > 0) {
		
		std::list<PatientTrainInfo> patients;
		while(stmt.Step() > 0) {
			PatientTrainInfo patient;

			patient.id = stmt.GetInt(0);
			patient.name = TGUTF8ToUTF16(stmt.GetString(1));
			patient.sex = TGUTF8ToUTF16(stmt.GetString(2));
			patient.age = TGUTF8ToUTF16(stmt.GetString(3));
			patient.createtime = TGUTF8ToUTF16(stmt.GetString(4));
			
			patients.push_back(patient);
		}
		stmt.Finalize();
		pResult->patienttraininfos = patients;
		
		CTask::Assign(CTask::NotWait, Panic(), pResult, EventHandle(&RFPatientsTrainInfo::OnLoadPatientTrainInfoOK), pCurrentThread, RFMainWindow::UIThread);
	}

	delete pParam;
	pParam = NULL;
	return 0;
}

int RFMySQLThread::LoadPatientTrainDetails(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	LoadPatientTrainDetailsParam *pParam = pTask->GetContext<LoadPatientTrainDetailsParam*>();
	if (!pParam) {
		return 1;
	}

	std::string sql = "select DISTINCT(a.id),a.patientid, a.traindate, a.content, a.duration, a.traintime, a.createtime, a.flag from patienttrain a, patient b where a.patientid = b.id and b.flag=0 ";
	char condition[256];
	sprintf(condition, " and b.hospitalid=%d and b.doctorid=%d  order by a.id desc", pParam->hospitalid, pParam->doctorid);
	sql += condition;


	LoadPatientTrainDetailsResult *pResult = new LoadPatientTrainDetailsResult;
	RFMYSQLStmt stmt;
	if (stmt.Prepare(RFMainWindow::DBThread->m_db, sql.c_str()) > 0) {

		std::list<PatientTrainDetails> patients;
		while(stmt.Step() > 0) {
			PatientTrainDetails patient;

			patient.id = stmt.GetInt(0);
			patient.patientid = stmt.GetInt(1);
			patient.traindate = TGUTF8ToUTF16(stmt.GetString(2));
			patient.content = TGUTF8ToUTF16(stmt.GetString(3));

			wchar_t val[256] = _T("");
			wcscpy(val, patient.content.c_str());
			wchar_t* p = wcstok(val, _T("-"));
			if (p) {
				patient.traintype = p;
				p = wcstok(NULL, _T("-"));
			}
	
			patient.content = patient.traintype;
			if (p) {
				patient.game = p;
				p = wcstok(NULL, _T("-"));
				patient.content += _T("-");
				patient.content += patient.game;
			}
			if (p) {
				patient.nandu = p;
				p = wcstok(NULL, _T("-"));
			}
			if (p) {
				patient.defen = p;
				p = wcstok(NULL, _T("-"));
			}


			patient.duration = TGUTF8ToUTF16(stmt.GetString(4));
			patient.traintime = TGUTF8ToUTF16(stmt.GetString(5));
			patient.createtime = TGUTF8ToUTF16(stmt.GetString(6));
			patient.flag = stmt.GetInt(7);

			patients.push_back(patient);
		}
		stmt.Finalize();
		pResult->patienttraindetails = patients;

		CTask::Assign(CTask::NotWait, Panic(), pResult, EventHandle(&RFPatientsTrainDetails::OnLoadPatientTrainDetailsOK), pCurrentThread, RFMainWindow::UIThread);
	}

	delete pParam;
	pParam = NULL;
	return 0;
}

int RFMySQLThread::LoadPassiveTrainInfo(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();

	std::string sql = "select id, name, traintype, movements, timelen from passivetrain";

	LoadPassiveTrainInfoResult *pResult = new LoadPassiveTrainInfoResult;
	RFMYSQLStmt stmt;
	if (stmt.Prepare(RFMainWindow::DBThread->m_db, sql.c_str()) > 0) {

		std::list<PassiveTrainInfo> trains;
		while(stmt.Step() > 0) {
			PassiveTrainInfo train;


			wchar_t id[64] = _T("");
			wsprintf(id, _T("%d"), stmt.GetInt(0));
			train.id = id;
			train.name = TGUTF8ToUTF16(stmt.GetString(1));
			train.traintype = TGUTF8ToUTF16(stmt.GetString(2));
			train.timelen = TGUTF8ToUTF16(stmt.GetString(4));

			std::vector<double> moves;
			DecodeMovements(stmt.GetString(3), moves);
			if (moves.size() % 4 == 0) {
				for (int i = 0; i < moves.size(); ){
					train.target_pos[0].push_back(moves.at(i));
					train.target_pos[1].push_back(moves.at(i+1));
					train.target_vel[0].push_back(moves.at(i+2));
					train.target_vel[1].push_back(moves.at(i+3));

					i += 4;
				}
			}
			

			trains.push_back(train);
		}
		stmt.Finalize();
		pResult->passivetraininfos = trains;

		CTask::Assign(CTask::NotWait, Panic(), pResult, EventHandle(&RFPassiveTrain::OnLoadPassiveTrainInfoOK), pCurrentThread, RFMainWindow::UIThread);
	}

	return 0;
}

int RFMySQLThread::AddPassiveTrainInfo(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	PassiveTrainInfo *pParam = pTask->GetContext<PassiveTrainInfo*>();
	if (!pParam) {
		return 1;
	}

	if (pParam->target_pos[0].size() != pParam->target_pos[1].size() &&
		pParam->target_pos[1].size() != pParam->target_vel[0].size() &&
		pParam->target_vel[0].size() != pParam->target_vel[1].size() &&
		pParam->target_pos[0].size() > 1) {
			delete pParam;
			pParam = NULL;
			return 1;
	}

	std::vector<double> vDoubles;
	for (int i = 0; i < pParam->target_pos[0].size(); i++) {
		vDoubles.push_back(pParam->target_pos[0].at(i));
		vDoubles.push_back(pParam->target_pos[1].at(i));
		vDoubles.push_back(pParam->target_vel[0].at(i));
		vDoubles.push_back(pParam->target_vel[1].at(i));
	}
	std::string movements =EncodeMovements(vDoubles);

	std::wstring sql = _T("INSERT INTO passivetrain (name,traintype,movements,timelen) value(");
	sql += _T("'") + pParam->name + _T("',");
	sql += _T("'") + pParam->traintype + _T("',");
	sql += _T("'") + TGUTF8ToUTF16(movements) + _T("',");
	sql += _T("'") + pParam->timelen + _T("')");
	RFMainWindow::DBThread->m_db.Exec(TGUTF16ToUTF8(sql).c_str());

	CTask::Assign(CTask::NotWait, Panic(), pParam, EventHandle(&RFPassiveTrain::OnAddPassiveTrainInfoOK), pCurrentThread, RFMainWindow::UIThread);

	return 1;
}

int RFMySQLThread::DeletePassiveTrainInfo(EventArg *pArg) {
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread *>();
	CTask *pTask = pArg->GetAttach<CTask *>();
	PassiveTrainInfo *pParam = pTask->GetContext<PassiveTrainInfo *>();
	if (!pParam) {
		return 1;
	}

	std::wstring sql = _T("DELETE FROM passivetrain WHERE name = ");
	sql += _T("'") + pParam->name + _T("'");
	RFMainWindow::DBThread->m_db.Exec(TGUTF16ToUTF8(sql).c_str());
	
	CTask::Assign(CTask::NotWait, Panic(), pParam, EventHandle(&RFPassiveTrain::OnDeletePassiveTrainInfoOK), pCurrentThread, RFMainWindow::UIThread);
}

int RFMySQLThread::LoadPatientTrainData(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	LoadPatientTrainDataParam *pParam = pTask->GetContext<LoadPatientTrainDataParam*>();
	if (!pParam) {
		return 1;
	}

	char sql[256];
	sprintf(sql, "select id, traindetailid, tracevalue, traintype from traindata where traindetailid=%d", pParam->traindetailid);

	LoadPatientTrainDataResult *pResult = new LoadPatientTrainDataResult;
	RFMYSQLStmt stmt;
	if (stmt.Prepare(RFMainWindow::DBThread->m_db, sql) > 0) {

		std::list<PatientTrainData> trains;
		while (stmt.Step() > 0) {
			int id = stmt.GetInt(0);
			int traindetailid = stmt.GetInt(1);
			std::string tracevalue = stmt.GetString(2);
			int tracetype = stmt.GetInt(3);

			std::vector<double> moves;
			DecodeMovements(tracevalue, moves);

			
			for (int i = 0; i < moves.size(); i++) {
				PatientTrainData train;

				train.id = id;
				train.traindetailid = traindetailid;

				if (tracetype == 3)	{
					train.timetrace = (i/4) * 100;		// EMG 10HZ
				} else if (tracetype == 1) {
					train.timetrace = (i/2) * 200;		// 其他 5HZ
				} else {
					train.timetrace = i * 200;
				}

				train.tracevalue = moves.at(i);
				train.traintype = tracetype;

				trains.push_back(train);
			}
			
			
		}
		stmt.Finalize();
		pResult->patienttraindatas = trains;

		CTask::Assign(CTask::NotWait, Panic(), pResult, EventHandle(&RFPatientsTrainData::OnLoadTrainDataOK), pCurrentThread, RFMainWindow::UIThread);
	}

	delete pParam;
	pParam = NULL;
	return 0;
}

void RFMySQLThread::LoadEvaluationRecords(int id, std::list<EvaluationRecordData>& datas)
{
	char sql[256];
	sprintf(sql, "select id, evalid, item, result from evaluationrecord where evalid=%d", id);
	RFMYSQLStmt stmt;
	if (stmt.Prepare(RFMainWindow::DBThread->m_db, sql) > 0) {
		while (stmt.Step() > 0) {
			EvaluationRecordData data;

			data.id = stmt.GetInt(0);
			data.evalid = stmt.GetInt(1);
			data.item = TGUTF8ToUTF16(stmt.GetString(2));
			data.result = TGUTF8ToUTF16(stmt.GetString(3));

			datas.push_back(data);
		}
	}
}

int RFMySQLThread::LoadEvaluationDatas(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	LoadEvaluationParam *pParam = pTask->GetContext<LoadEvaluationParam*>();
	if (!pParam) {
		return 1;
	}


	int nextid = 0;
	RFMYSQLStmt stmt;
	if (stmt.Prepare(RFMainWindow::DBThread->m_db, "SELECT AUTO_INCREMENT FROM information_schema.tables where TABLE_NAME='evaluation'") > 0) {

		if (stmt.Step() > 0) {
			nextid = stmt.GetInt(0);
		}
	}

	char sql[256];
	sprintf(sql, "select id, patientid, evaldate, name, score from evaluation where doctorid=%d and type=%d order by id desc", pParam->doctorid, pParam->type);

//	RFMYSQLStmt stmt;
	if (stmt.Prepare(RFMainWindow::DBThread->m_db, sql) > 0) {
		LoadEvaluationResult* pResult = new LoadEvaluationResult;
		pResult->nextid = nextid;
		while (stmt.Step() > 0) {
			EvaluationData data;

			data.doctorid = pParam->doctorid;
			data.id = stmt.GetInt(0);
			data.patientid = stmt.GetInt(1);
			data.date = TGUTF8ToUTF16(stmt.GetString(2));
			data.name = TGUTF8ToUTF16(stmt.GetString(3));
			data.score = TGUTF8ToUTF16(stmt.GetString(4));
			data.type = pParam->type;

			LoadEvaluationRecords(data.id, data.datas);
			pResult->datas.push_back(data);
		}

		CTask::Assign(CTask::NotWait, Panic(), pResult, EventHandle(&RFEvaluationData::OnLoadOK), pCurrentThread, RFMainWindow::UIThread);
	}

	delete pParam;
	pParam = NULL;
	return 0;
}

int RFMySQLThread::AddEvaluationData(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	EvaluationData *pParam = pTask->GetContext<EvaluationData*>();
	if (!pParam) {
		return 1;
	}

	wchar_t sqlParam[128] = _T("");
	wsprintf(sqlParam, _T("%d, %d, %d,%d,"), pParam->id, pParam->doctorid, pParam->patientid, pParam->type);

	std::wstring sql = _T("INSERT INTO evaluation (id, doctorid, patientid, type, evaldate, name, score) value(");
	sql += sqlParam;
	sql += _T("'") + pParam->date + _T("',");
	sql += _T("'") + pParam->name + _T("',");
	sql += _T("'") + pParam->score + _T("')");
	RFMainWindow::DBThread->m_db.Exec(TGUTF16ToUTF8(sql).c_str());

	std::list<EvaluationRecordData>::iterator begin = pParam->datas.begin();
	for (; begin != pParam->datas.end(); begin++) {
		wsprintf(sqlParam, _T("%d,"), begin->evalid);
		sql = _T("INSERT INTO evaluationrecord (evalid, item, result) value(");
		sql += sqlParam;
		sql += _T("'") + begin->item + _T("',");
		sql += _T("'") + begin->result + _T("')");
		RFMainWindow::DBThread->m_db.Exec(TGUTF16ToUTF8(sql).c_str());
	}
	
	delete pParam;
	return 0;
}

int RFMySQLThread::TTSSample(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	TTSSampleData *pParam = pTask->GetContext<TTSSampleData*>();
	if (!pParam) {
		return 1;
	}

	RFTTSSample(pParam->text.c_str(), pParam->filepath.c_str());

	delete pParam;
	pParam = NULL;
}
