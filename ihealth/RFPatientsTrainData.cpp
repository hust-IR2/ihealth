#include "StdAfx.h"
#include "RFPatientsTrainData.h"
#include "RFMySQLThread.h"
#include "RFMainWindow.h"
#include "RFPatientsManager.h"

RFPatientsTrainData* RFPatientsTrainData::m_sigleton = NULL;

RFPatientsTrainData* RFPatientsTrainData::get()
{
	if (m_sigleton == NULL) {
		m_sigleton = new RFPatientsTrainData();
		//m_sigleton->step();
	}

	return m_sigleton;
}

void RFPatientsTrainData::release()
{
	if (m_sigleton) {
		delete m_sigleton;
		m_sigleton = NULL;
	}
}

RFPatientsTrainData::RFPatientsTrainData(void)
{
}

RFPatientsTrainData::~RFPatientsTrainData(void)
{
}

void RFPatientsTrainData::LoadTrainData(const PatientTrainDetails& details)
{
	LoadPatientTrainDataParam *pParam = new LoadPatientTrainDataParam();

	m_current_traindetail = details;
	pParam->traindetailid = details.id;

	CTask::Assign(CTask::NotWait, Panic(), pParam, EventHandle(&RFMySQLThread::LoadPatientTrainData), RFMainWindow::UIThread, RFMainWindow::DBThread);
}

int RFPatientsTrainData::OnLoadTrainDataOK(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	LoadPatientTrainDataResult *pParam = pTask->GetContext<LoadPatientTrainDataResult*>();
	if (!pParam) {
		return 1;
	}

	if (!RFMainWindow::MainWindow) {
		delete pParam;
		return 1;
	}

	RFPatientsTrainData::get()->m_current_traindatas = pParam->patienttraindatas;
	if (RFMainWindow::MainWindow->m_current_chart == RF_TRAIN_TYPE_ZD) {
		
		std::list<LineWaveData> gjjd_waves;
		std::list<LineWaveData> wl_waves;

		std::list<PatientTrainData>::iterator begin = pParam->patienttraindatas.begin();
		for (; begin != pParam->patienttraindatas.end(); begin++) {
			LineWaveData wave;

			if (begin->traintype == 1) {
				wave.x = begin->timetrace;
				wave.y = begin->tracevalue;
				gjjd_waves.push_back(wave);
			} else if (begin->traintype == 2) {
				wave.x = begin->timetrace;
				wave.y = begin->tracevalue;
				wl_waves.push_back(wave);
			}
		}

		RFMainWindow::MainWindow->UpdateZDWLWaveData(wl_waves);
		RFMainWindow::MainWindow->UpdateZDGJJDWaveData(gjjd_waves);
	} else if (RFMainWindow::MainWindow->m_current_chart == RF_TRAIN_TYPE_BD) {
		std::list<LineWaveData> jgj_waves;
		std::list<LineWaveData> zgj_waves;

		std::list<PatientTrainData>::iterator begin = pParam->patienttraindatas.begin();
		for (; begin != pParam->patienttraindatas.end(); begin++) {
			LineWaveData wave;

			if (begin->traintype == 4) {
				wave.x = begin->timetrace;
				wave.y = begin->tracevalue;
				jgj_waves.push_back(wave);
			} else if (begin->traintype == 5) {
				wave.x = begin->timetrace;
				wave.y = begin->tracevalue;
				zgj_waves.push_back(wave);
			}
		}
		RFMainWindow::MainWindow->UpdateJGJWaveData(jgj_waves);
		RFMainWindow::MainWindow->UpdateZGJWaveData(zgj_waves);
	} else if (RFMainWindow::MainWindow->m_current_chart == RF_TRAIN_TYPE_YD) {
		std::list<LineWaveData> yd_waves;

		std::list<PatientTrainData>::iterator begin = pParam->patienttraindatas.begin();
		for (; begin != pParam->patienttraindatas.end(); begin++) {
			LineWaveData wave;

			if (begin->traintype == 1) {
				wave.x = begin->timetrace;
				wave.y = begin->tracevalue;
				yd_waves.push_back(wave);
			} 
		}
		RFMainWindow::MainWindow->UpdateYDGJJDWaveData(yd_waves);
	} else if (RFMainWindow::MainWindow->m_current_chart == RF_TRAIN_TYPE_EMG) {
		std::list<LineWaveData> emg_waves;
		std::list<LineWaveData> emg_gjyd_waves;

		std::list<PatientTrainData>::iterator begin = pParam->patienttraindatas.begin();
		for (; begin != pParam->patienttraindatas.end(); begin++) {
			LineWaveData wave;

			if (begin->traintype == 3) {
				wave.x = begin->timetrace;
				wave.y = begin->tracevalue;
				emg_waves.push_back(wave);
			} else if (begin->traintype == 1) {
				wave.x = begin->timetrace;
				wave.y = begin->tracevalue;
				emg_gjyd_waves.push_back(wave);
			}
		}
		RFMainWindow::MainWindow->UpdateEMGWaveData(emg_waves);
		RFMainWindow::MainWindow->UpdateEMGGJYDWaveData(emg_gjyd_waves);
	}

	delete pParam;
	return 1;
}

void RFPatientsTrainData::exportTrainData(std::wstring savepath)
{
	std::wstring current_chart_type = RFMainWindow::MainWindow->m_current_chart;
	bool unknown_mode = true;


	int row = 3;
	std::wstring templatePath = _T("");
	if (current_chart_type == RF_TRAIN_TYPE_ZD) {
		templatePath = CPaintManagerUI::GetResourcePath() + _T("traindata_zd.xlsx");

		RFPatientsManager::get()->m_excel.OpenExcelFile(templatePath.c_str());
		CString strSheetName = RFPatientsManager::get()->m_excel.GetSheetName(1);
		RFPatientsManager::get()->m_excel.LoadSheet(strSheetName);
		unknown_mode = false;

		RFPatientsManager::get()->m_excel.SetCellString(row, 1, m_current_traindetail.createtime);
		RFPatientsManager::get()->m_excel.SetCellString(row, 2, m_current_traindetail.traintype);
		RFPatientsManager::get()->m_excel.SetCellString(row, 3, m_current_traindetail.game);
		RFPatientsManager::get()->m_excel.SetCellString(row, 4, m_current_traindetail.nandu);
		RFPatientsManager::get()->m_excel.SetCellString(row, 5, m_current_traindetail.defen);

	} else if (current_chart_type == RF_TRAIN_TYPE_BD) {
		templatePath = CPaintManagerUI::GetResourcePath() + _T("traindata_bd.xlsx");


		RFPatientsManager::get()->m_excel.OpenExcelFile(templatePath.c_str());
		CString strSheetName = RFPatientsManager::get()->m_excel.GetSheetName(1);
		RFPatientsManager::get()->m_excel.LoadSheet(strSheetName);
		unknown_mode = false;

		RFPatientsManager::get()->m_excel.SetCellString(row, 1, m_current_traindetail.createtime);
		RFPatientsManager::get()->m_excel.SetCellString(row, 2, m_current_traindetail.traintype);
		RFPatientsManager::get()->m_excel.SetCellString(row, 3, m_current_traindetail.game);
		RFPatientsManager::get()->m_excel.SetCellString(row, 4, m_current_traindetail.traintime);
	} else if (current_chart_type == RF_TRAIN_TYPE_EMG || current_chart_type == RF_TRAIN_TYPE_YD) {
		templatePath = CPaintManagerUI::GetResourcePath() + _T("traindata_yd.xlsx");


		RFPatientsManager::get()->m_excel.OpenExcelFile(templatePath.c_str());
		CString strSheetName = RFPatientsManager::get()->m_excel.GetSheetName(1);
		RFPatientsManager::get()->m_excel.LoadSheet(strSheetName);
		unknown_mode = false;

		RFPatientsManager::get()->m_excel.SetCellString(row, 1, m_current_traindetail.createtime);
		RFPatientsManager::get()->m_excel.SetCellString(row, 2, m_current_traindetail.traintype);
		RFPatientsManager::get()->m_excel.SetCellString(row, 4, m_current_traindetail.traintime);
	}

	if (!unknown_mode) {
		row = 6;
		std::list<PatientTrainData>::iterator begin = m_current_traindatas.begin();
		for (; begin != m_current_traindatas.end(); begin++) {

			RFPatientsManager::get()->m_excel.SetCellInt(row, 1, begin->id);

			std::wstring train_type = _T("");
			if (begin->traintype == 1) {
				train_type = _T("关节运动角度");
			} else if (begin->traintype == 2) {
				train_type = _T("握力");
			} else if (begin->traintype == 3) {
				train_type = _T("EMG信号");
			} else if (begin->traintype == 4) {
				train_type = _T("肩关节力矩");
			} else if (begin->traintype == 5) {
				train_type = _T("肘关节力矩");
			}
			RFPatientsManager::get()->m_excel.SetCellString(row, 2, train_type);
			RFPatientsManager::get()->m_excel.SetCellInt(row, 4, begin->timetrace);
			RFPatientsManager::get()->m_excel.SetCellInt(row, 5, begin->tracevalue);

			row++;
		}

		RFPatientsManager::get()->m_excel.SaveasXSLFile(savepath.c_str());
		RFPatientsManager::get()->m_excel.CloseExcelFile(FALSE);
	}
}
