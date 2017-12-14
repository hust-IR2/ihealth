#pragma once
#include "RFMySQLThread.h"

class RFEvaluationData
{
public:
	RFEvaluationData(void);
	~RFEvaluationData(void);
public:
	static RFEvaluationData* get();
	static void release();
	static RFEvaluationData* m_sigleton;

	void Load(int type);
	void Add(EvaluationData& data);
	std::list<EvaluationData> Get(int page);
	int GetElementNumber(int page);
	//std::wstring GetRecentScore();

	void setCurPage(int page);
	void setDoctorID(int id);

	static int OnLoadOK(EventArg* pArg);

	int m_nextid;
	std::list<EvaluationData> m_datas;
	int m_doctorid;
	int m_current_page;
};

class RFEvaluationDetailData
{
public:
	RFEvaluationDetailData();
	~RFEvaluationDetailData();

public:
	static RFEvaluationDetailData* get();
	static void release();
	static RFEvaluationDetailData* m_sigleton;

	void Load(int id);
	void setCurPage(int page);
	std::list<EvaluationRecordData> Get(int page);
	int GetElementNumber(int page);

	int m_current_page;
	std::list<EvaluationRecordData> m_datas;
	std::wstring m_score;
};