#pragma once
#include "RFMySQLThread.h"

#define PATIENT_TRAIN_DETAILS_NUMBER 5

class RFPatientsTrainDetails
{
public:
	static RFPatientsTrainDetails* get();
	static void release();
	static RFPatientsTrainDetails* m_sigleton;

public:
	RFPatientsTrainDetails(void);
	~RFPatientsTrainDetails(void);

	void SetDoctorID(int id);
	void SetHospitalID(int id);
	void SetCurrentPage(int page);
	void SetCurrentPatientID(int patientid);

	int pageElementNum(int page);
	std::list<PatientTrainDetails> getPageElement(int page);
	void removeElement(int patientid);

	PatientTrainDetails getTrainDetail(int id);

	std::list<PatientTrainDetails> search(std::wstring createtime);

	int LoadPatientTrainDetails();

	void AddPatientTrainDetails(const PatientTrainDetails& detail);
	void exportTrainDetail(std::wstring templatepath, std::wstring savepath);

	int m_patientid;
	int m_hospitalid;
	int m_doctorid;
	int m_currentpage;
	std::list<PatientTrainDetails> m_patienttraindetails;

	static int OnLoadPatientTrainDetailsOK(EventArg* pArg);
};
