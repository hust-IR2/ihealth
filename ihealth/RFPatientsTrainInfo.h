#pragma once
#include "RFMySQLThread.h"


#define PATIENT_TRAIN_PAGE_NUMBER 8

class RFPatientsTrainInfo
{
public:
	static RFPatientsTrainInfo* get();
	static void release();
	static RFPatientsTrainInfo* m_sigleton;

public:
	RFPatientsTrainInfo(void);
	~RFPatientsTrainInfo(void);

	int LoadPatientTrainInfo();
	void modify(const PatientInfo& patient);
	void remove(int patientid);
	void add(const PatientInfo& pateint);
	std::list<PatientTrainInfo> search(std::wstring patientname);
	PatientTrainInfo getTrainInfo(std::wstring patientid);

	void setDoctorID(int id);
	void setHospitalID(int id);
	void setCurrentPage(int page);
	int pageElementNum(int page);
	std::list<PatientTrainInfo> getPageElement(int page);
	void removeElement(int patientid);

	void exportTrainInfo(std::wstring templatepath, std::wstring savepath);

	int m_hospitalid;
	int m_doctorid;

	int	m_current_page;

	std::list<PatientTrainInfo> m_patienttraininfos;

public:
	static int OnLoadPatientTrainInfoOK(EventArg* pArg);
};

