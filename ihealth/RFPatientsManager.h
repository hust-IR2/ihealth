#pragma once
#include "RFMySQLThread.h"
#include "RFExcel.h"

#define PATIENT_PAGE_NUMBER 8
#define PATIENT_GROUP_NUMBER 32

class RFPatientsManager
{
public:
	static RFPatientsManager* get();
	static void release();
	static RFPatientsManager* m_sigleton;


public:
	RFPatientsManager(void);
	~RFPatientsManager(void);

	void setDoctorID(int id);
	void SetHospitalID(int id);

	int step(int deletenum = 0);

	void setPage(int num);
	void remove(int patientid, int autoload = 1);

	void modifyPwd(const ModifyPWDInfo& pwd);
	void modify(const LoginInfo& doctor);
	void modify(const PatientInfo &patient);
	void add(const PatientInfo &patient);
	void nextPatientID();

	void search(std::wstring patientname);
	void filter(const PatientFilterParam& param);
	void exportPatient(const ExportPatientParam& param);
	void exportPatient(int patientid, std::wstring templatepath, std::wstring savepath);
	void importPatient(std::wstring path);

	std::list<PatientInfo> getPage(int num);
	PatientInfo	getPatient(int page, int index);
	PatientInfo getPatient(int patientid);

	static int OnAddOK(EventArg* pArg);
	static int OnStepOK(EventArg *pArg);
	static int OnModifyOK(EventArg* pArg);
	static int OnModifyDoctorOK(EventArg* pArg);
	static int OnNextPatientIDOK(EventArg* pArg);
	static int OnExportPatientFilterOK(EventArg* pArg);

	std::list<PatientInfo>	m_patients;
	int					m_current_page;
	int					m_current_group;

	int					m_hospitalid;
	int					m_doctorid;
	IllusionExcelFile	m_excel;
};
