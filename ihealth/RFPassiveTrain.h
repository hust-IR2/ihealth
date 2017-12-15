#pragma once
#include "RFMySQLThread.h"

class RFPassiveTrain
{
public:
	static RFPassiveTrain* get();
	static void release();
	static RFPassiveTrain* m_sigleton;

public:
	RFPassiveTrain(void);
	~RFPassiveTrain(void);

	int LoadPassiveTrainInfo();
	void AddPassiveTrainInfo(PassiveTrainInfo train);
	
	std::map<std::wstring, int> m_robot_indexs;
	std::list<PassiveTrainInfo> m_passivetraininfos;

	static int OnLoadPassiveTrainInfoOK(EventArg* pArg);
	static int OnAddPassiveTrainInfoOK(EventArg* pArg);
	static int OnDeletePassiveTrainInfoOK(EventArg *pArg);
};
