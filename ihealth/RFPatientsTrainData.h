#pragma once
#include "RFMySQLThread.h"

class RFPatientsTrainData
{
public:
	static RFPatientsTrainData* get();
	static void release();
	static RFPatientsTrainData* m_sigleton;

public:
	RFPatientsTrainData(void);
	~RFPatientsTrainData(void);

	void LoadTrainData(const PatientTrainDetails& details);
	static int OnLoadTrainDataOK(EventArg* pArg);

	void exportTrainData(std::wstring savepath);

	std::list<PatientTrainData> m_current_traindatas;
	PatientTrainDetails m_current_traindetail;
};
