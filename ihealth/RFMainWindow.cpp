#include "StdAfx.h"
#include "RFMainWindow.h"
#include "RFLoginFailWindow.h"
#include "RFCharSet.h"
#include "RFPatientsManager.h"
#include "RFPatientsTrainInfo.h"
#include "RFPatientsTrainDetails.h"
#include "RFPatientsTrainData.h"
#include "WkeWebkit.h"
#include "UIChart.h"
#include "UIEMGChart.h"
#include "UIProgressEx.h"
#include "RFPassiveTrain.h"
#include "PopupWidget.h"
//#include "AVPlayer.h"
#include "UIAVPlayer.h"
#include <mmsystem.h>
#include "Log.h"
#include "RFEvaluationData.h"



static bool	  s_active_begin_recode = false;
static time_t s_active_game4_start = 0;
static time_t s_active_game4_stop = 0;

static std::vector<double>	s_active_data[2];
static std::vector<double>  s_active_data_wl;

static time_t	s_eyemode_start = 0;
static time_t	s_eyemode_stop = 0;
static std::vector<double>	s_eyemode_data[2];

CUIThread* RFMainWindow::UIThread = NULL;
RFMySQLThread* RFMainWindow::DBThread = NULL;
RFMainWindow* RFMainWindow::MainWindow = NULL;

RFMainWindow::RFMainWindow(void)
{
	m_mysql_connected = false;

	m_current_patient.id = -1;
	m_emgmode_tracetime = 0;
	m_emgmode_timer = NULL;
	m_emg_createtime = 0;
}

RFMainWindow::~RFMainWindow(void)
{
	m_robotEvent.Stop();
	RFPatientsManager::release();
	RFPatientsTrainInfo::release();
	RFPatientsTrainDetails::release();
	RFPassiveTrain::release();
	RFEvaluationData::release();
	RFEvaluationDetailData::release();

	CWorkThread::Dispose();
	RFMySQLThread::Release(RFMainWindow::DBThread);
	CWorkThread::Dispose();

	CWorkThread::Dispose();
	CUIThread::Release(UIThread);
	CWorkThread::Dispose();
}

LPCTSTR RFMainWindow::GetWindowClassName() const 
{ 
	return _T("RFMainWindow");
}

UINT RFMainWindow::GetClassStyle() const 
{ 
	return CS_DBLCLKS; 
}

void RFMainWindow::OnFinalMessage(HWND /*hWnd*/) 
{
}

CControlUI* RFMainWindow::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass, _T("wkeWebkit")) == 0) return  new CWkeWebkitUI;
	else if (_tcsicmp(pstrClass, _T("MusicProgress")) == 0) return new CProgressExUI;
	else if (_tcsicmp(pstrClass, _T("AVCamera")) == 0)	return new CAVPlayerUI;
	else if (_tcscmp(pstrClass, _T("Wave")) == 0) return new CWaveUI;
	else if (_tcscmp(pstrClass, _T("EMGWave")) == 0) return new CEMGWaveUI;
	return NULL;
}

void RFMainWindow::Init() 
{
	Panic panic1 = CUIThread::Create(this->GetHWND());
	UIThread = panic1.GetTag<CUIThread*>();

	DBThread = RFMySQLThread::Create();

	RFPatientsTrainDetails::get();
	RFPatientsTrainInfo::get();
	RFPatientsManager::get();
	RFPassiveTrain::get()->LoadPassiveTrainInfo();
	MainWindow = this;
	m_robot.setWindow(GetHWND());

	ShowLoginPage();
}

void RFMainWindow::OnPrepare() 
{
}

void RFMainWindow::Closing()
{
}

void RFMainWindow::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("windowinit") ) 
	{
		OnPrepare();
	}
	if (msg.sType == DUI_MSGTYPE_SCROLL) {
		CScrollBarUI* pScrollBar = static_cast<CScrollBarUI*>(msg.pSender);
		if (pScrollBar) {
			std::wstring scrollname = pScrollBar->GetName();
			if (scrollname == _T("zd_gjjd_scroll")) {
				CWaveUI* pWave = static_cast<CWaveUI*>(m_pm.FindControl(_T("zd_gjjd_wave")));
				pWave->Invalidate();


				UpdateZDGJJDWaveXNum((double)pWave->GetXStartValue() / 1000.0f);
			} else if (scrollname == _T("zd_wl_scroll")) {
				CWaveUI* pWave = static_cast<CWaveUI*>(m_pm.FindControl(_T("zd_wl_wave")));
				pWave->Invalidate();

				UpdateZDWLWaveXNum((double)pWave->GetXStartValue() / 1000.0f);
			}

			if (scrollname == _T("bd_jgj_scroll")) {
				CWaveUI* pWave = static_cast<CWaveUI*>(m_pm.FindControl(_T("bd_jgj_wave")));
				pWave->Invalidate();


				UpdateJGJWaveXNum((double)pWave->GetXStartValue() / 1000.0f);
			} else if (scrollname == _T("bd_zgj_scroll")) {
				CWaveUI* pWave = static_cast<CWaveUI*>(m_pm.FindControl(_T("bd_zgj_wave")));
				pWave->Invalidate();

				UpdateZGJWaveXNum((double)pWave->GetXStartValue() / 1000.0f);
			} else if (scrollname == _T("yd_gjydjd_scroll")) {
				CWaveUI* pWave = static_cast<CWaveUI*>(m_pm.FindControl(_T("yd_gjydjd_wave")));
				pWave->Invalidate();

				UpdateYDGJJDWaveXNum((double)pWave->GetXStartValue() / 1000.0f);
			} else if (scrollname == _T("emg_scroll")) {
				CWaveUI* pWave = static_cast<CWaveUI*>(m_pm.FindControl(_T("emg_wave")));
				pWave->Invalidate();

				UpdateEMGWaveXNum((double)pWave->GetXStartValue() / 1000.0f);
			} else if (scrollname == _T("emg_gjyd_scroll")) {
				CWaveUI* pWave = static_cast<CWaveUI*>(m_pm.FindControl(_T("emg_gjyd_wave")));
				pWave->Invalidate();

				UpdateEMGGJYDWaveXNum((double)pWave->GetXStartValue() / 1000.0f);
			}
			
		}
	}
	if (msg.sType == DUI_MSGTYPE_VALUECHANGED_MOVE) {
		CSliderUI* pSlider = static_cast<CSliderUI*>(msg.pSender);
		if (pSlider) {
			double v = 0.1 + (double)(pSlider->GetValue() / pSlider->GetMaxValue()) * 0.5;
			m_robot.setDamping(v);
		}
	}
}

void RFMainWindow::BindSelectPatientPageEvent()
{
	CButtonUI* select_patient_return = static_cast<CButtonUI*>(m_pm.FindControl(_T("select_patient_return")));
	select_patient_return->OnNotify += MakeDelegate(this, &RFMainWindow::OnReturnMainPage);

	CButtonUI* prevpage = static_cast<CButtonUI*>(m_pm.FindControl(_T("last_page")));
	prevpage->OnNotify += MakeDelegate(this, &RFMainWindow::OnPrevPage);

	CButtonUI* page1 = static_cast<CButtonUI*>(m_pm.FindControl(_T("page1")));
	page1->OnNotify += MakeDelegate(this, &RFMainWindow::OnPage1);

	CButtonUI* page2 = static_cast<CButtonUI*>(m_pm.FindControl(_T("page2")));
	page2->OnNotify += MakeDelegate(this, &RFMainWindow::OnPage2);

	CButtonUI* page3 = static_cast<CButtonUI*>(m_pm.FindControl(_T("page3")));
	page3->OnNotify += MakeDelegate(this, &RFMainWindow::OnPage3);

	CButtonUI* page4 = static_cast<CButtonUI*>(m_pm.FindControl(_T("page4")));
	page4->OnNotify += MakeDelegate(this, &RFMainWindow::OnPage4);

	CButtonUI* nextpage = static_cast<CButtonUI*>(m_pm.FindControl(_T("next_page")));
	nextpage->OnNotify += MakeDelegate(this, &RFMainWindow::OnNextPage);

	CButtonUI* select_patient_search = static_cast<CButtonUI*>(m_pm.FindControl(_T("select_patient_search")));
	select_patient_search->OnNotify += MakeDelegate(this, &RFMainWindow::OnSearch);
}

void RFMainWindow::BindManagerPatientPageEvent()
{
	CButtonUI* manger_patient_return = static_cast<CButtonUI*>(m_pm.FindControl(_T("manager_patient_return")));
	manger_patient_return->OnNotify += MakeDelegate(this, &RFMainWindow::OnReturnMainPage);

	CButtonUI* personor_info_return = static_cast<CButtonUI*>(m_pm.FindControl(_T("personor_info_return")));
	personor_info_return->OnNotify += MakeDelegate(this, &RFMainWindow::OnReturnMainPage);

	CButtonUI* prevpage = static_cast<CButtonUI*>(m_pm.FindControl(_T("manage_last_page")));
	prevpage->OnNotify += MakeDelegate(this, &RFMainWindow::OnManagerPrevPage);

	CButtonUI* page1 = static_cast<CButtonUI*>(m_pm.FindControl(_T("manage_page1")));
	page1->OnNotify += MakeDelegate(this, &RFMainWindow::OnManagerPage1);

	CButtonUI* page2 = static_cast<CButtonUI*>(m_pm.FindControl(_T("manage_page2")));
	page2->OnNotify += MakeDelegate(this, &RFMainWindow::OnManagerPage2);

	CButtonUI* page3 = static_cast<CButtonUI*>(m_pm.FindControl(_T("manage_page3")));
	page3->OnNotify += MakeDelegate(this, &RFMainWindow::OnManagerPage3);

	CButtonUI* page4 = static_cast<CButtonUI*>(m_pm.FindControl(_T("manage_page4")));
	page4->OnNotify += MakeDelegate(this, &RFMainWindow::OnManagerPage4);

	CButtonUI* nextpage = static_cast<CButtonUI*>(m_pm.FindControl(_T("manage_next_page")));
	nextpage->OnNotify += MakeDelegate(this, &RFMainWindow::OnManagerNextPage);

	CButtonUI* select_patient_search = static_cast<CButtonUI*>(m_pm.FindControl(_T("manager_patient_search")));
	select_patient_search->OnNotify += MakeDelegate(this, &RFMainWindow::OnManagerSearch);

	CButtonUI* management_btn_tianjia = static_cast<CButtonUI*>(m_pm.FindControl(_T("management_btn_tianjia")));
	management_btn_tianjia->OnNotify += MakeDelegate(this, &RFMainWindow::OnManagerPatientAdd);

	CButtonUI* detail_return= static_cast<CButtonUI*>(m_pm.FindControl(_T("manager_patient_detail_return")));
	detail_return->OnNotify += MakeDelegate(this, &RFMainWindow::OnManagerPatient);
	
	CButtonUI* edit_return= static_cast<CButtonUI*>(m_pm.FindControl(_T("manager_patient_bianji_return")));
	edit_return->OnNotify += MakeDelegate(this, &RFMainWindow::OnManagerPatient);

	CButtonUI* add_return= static_cast<CButtonUI*>(m_pm.FindControl(_T("manager_patient_add_return")));
	add_return->OnNotify += MakeDelegate(this, &RFMainWindow::OnManagerPatient);

	CButtonUI* about_return= static_cast<CButtonUI*>(m_pm.FindControl(_T("about_return")));
	about_return->OnNotify += MakeDelegate(this, &RFMainWindow::OnEntry);

	CButtonUI* patient_information_return= static_cast<CButtonUI*>(m_pm.FindControl(_T("patient_information_return")));
	patient_information_return->OnNotify += MakeDelegate(this, &RFMainWindow::OnReturnMainPage);

	CButtonUI* patient_traindetail_return= static_cast<CButtonUI*>(m_pm.FindControl(_T("patient_traindetail_return")));
	patient_traindetail_return->OnNotify += MakeDelegate(this, &RFMainWindow::OnPatientTrainMessage);

	CButtonUI* train_main_page_return= static_cast<CButtonUI*>(m_pm.FindControl(_T("train_main_page_return")));
	train_main_page_return->OnNotify += MakeDelegate(this, &RFMainWindow::OnReturnMainPage);

	CButtonUI* active_train_page_return= static_cast<CButtonUI*>(m_pm.FindControl(_T("active_train_page_return")));
	active_train_page_return->OnNotify += MakeDelegate(this, &RFMainWindow::OnPatientTrainFromActiveTrain);

	CButtonUI* passive_train_page_return= static_cast<CButtonUI*>(m_pm.FindControl(_T("passive_train_page_return")));
	passive_train_page_return->OnNotify += MakeDelegate(this, &RFMainWindow::OnPatientTrainFromPassiveTrain);

	CButtonUI* eye_mode_return= static_cast<CButtonUI*>(m_pm.FindControl(_T("eye_mode_return")));
	eye_mode_return->OnNotify += MakeDelegate(this, &RFMainWindow::OnPatientTrainFromEyeMode);

	CButtonUI* emg_mode_return= static_cast<CButtonUI*>(m_pm.FindControl(_T("emg_mode_return")));
	emg_mode_return->OnNotify += MakeDelegate(this, &RFMainWindow::OnPatientTrain);

	CButtonUI* traindetail_chart_return = static_cast<CButtonUI*>(m_pm.FindControl(_T("traindetail_chart_return")));
	traindetail_chart_return->OnNotify += MakeDelegate(this, &RFMainWindow::OnPatientTrainDetail);

	CButtonUI* active_train_game4_return= static_cast<CButtonUI*>(m_pm.FindControl(_T("active_train_game4_return")));
	active_train_game4_return->OnNotify += MakeDelegate(this, &RFMainWindow::OnActiveTrainFromGame);
	
	CButtonUI* evaluation_page_return= static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation_page_return")));
	evaluation_page_return->OnNotify += MakeDelegate(this, &RFMainWindow::OnReturnMainPage);

	CButtonUI* evaluation_history_return= static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation_history_return")));
	evaluation_history_return->OnNotify += MakeDelegate(this, &RFMainWindow::OnReturnEvaluationPage);

	CButtonUI* evaluation_add_return = static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation_add_return")));
	evaluation_add_return->OnNotify += MakeDelegate(this, &RFMainWindow::OnReturnEvaluationHistoryPage);

	CButtonUI* evaluation_ydgn_add_return = static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation_ydgn_add_return")));
	evaluation_ydgn_add_return->OnNotify += MakeDelegate(this, &RFMainWindow::OnReturnEvaluationHistoryPage);

	CButtonUI* evaluation_ydgn_detail_return = static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation_ydgn_detail_return")));
	evaluation_ydgn_detail_return->OnNotify += MakeDelegate(this, &RFMainWindow::OnReturnEvaluationHistoryPage);

	CButtonUI* evaluation_detail_return = static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation_detail_return")));
	evaluation_detail_return->OnNotify += MakeDelegate(this, &RFMainWindow::OnReturnEvaluationHistoryPage);


	CButtonUI* manager_edit_patient_save= static_cast<CButtonUI*>(m_pm.FindControl(_T("manager_edit_patient_save")));
	manager_edit_patient_save->OnNotify += MakeDelegate(this, &RFMainWindow::OnSavePatient);

	CButtonUI* manager_patient_detail_edit = static_cast<CButtonUI*>(m_pm.FindControl(_T("manager_patient_detail_edit")));
	manager_patient_detail_edit->OnNotify += MakeDelegate(this, &RFMainWindow::OnDetailPageEdit);

	CButtonUI* manager_patient_detail_delete = static_cast<CButtonUI*>(m_pm.FindControl(_T("manager_patient_detail_delete")));
	manager_patient_detail_delete->OnNotify += MakeDelegate(this, &RFMainWindow::OnDetailPageDelete);

	CButtonUI* manager_add_patient_save = static_cast<CButtonUI*>(m_pm.FindControl(_T("manager_add_patient_save")));
	manager_add_patient_save->OnNotify += MakeDelegate(this, &RFMainWindow::OnAddSavePatient);

	CButtonUI* manager_patient_shaixuan = static_cast<CButtonUI*>(m_pm.FindControl(_T("manager_patient_shaixuan")));
	manager_patient_shaixuan->OnNotify += MakeDelegate(this, &RFMainWindow::OnFilter);
	
	CButtonUI* personor_info = static_cast<CButtonUI*>(m_pm.FindControl(_T("begin_edit_person_info")));
	personor_info->OnNotify += MakeDelegate(this, &RFMainWindow::OnChangePersonInfoPage);

	CButtonUI* changepwd_box = static_cast<CButtonUI*>(m_pm.FindControl(_T("begin_change_pwd")));
	changepwd_box->OnNotify += MakeDelegate(this, &RFMainWindow::OnChangePasswordPage);

	CButtonUI* personorinfo_save = static_cast<CButtonUI*>(m_pm.FindControl(_T("personorinfo_save")));
	personorinfo_save->OnNotify += MakeDelegate(this, &RFMainWindow::OnSavePersonInfo);

	CButtonUI* personorinfo_pwd_save = static_cast<CButtonUI*>(m_pm.FindControl(_T("personorinfo_pwd_save")));
	personorinfo_pwd_save->OnNotify += MakeDelegate(this, &RFMainWindow::OnModifyPwdInfo);

	CButtonUI* manager_patient_export = static_cast<CButtonUI*>(m_pm.FindControl(_T("manager_patient_export")));
	manager_patient_export->OnNotify += MakeDelegate(this, &RFMainWindow::OnExportFilterPatient);

	CButtonUI* manager_patient_import = static_cast<CButtonUI*>(m_pm.FindControl(_T("manager_patient_import")));
	manager_patient_import->OnNotify += MakeDelegate(this, &RFMainWindow::OnImportPatient);

	CButtonUI* manager_patient_detail_export = static_cast<CButtonUI*>(m_pm.FindControl(_T("manager_patient_detail_export")));
	manager_patient_detail_export->OnNotify += MakeDelegate(this, &RFMainWindow::OnExportFilterPatientDetail);
	
	CButtonUI* traininfo_export = static_cast<CButtonUI*>(m_pm.FindControl(_T("traininfo_export")));
	traininfo_export->OnNotify += MakeDelegate(this, &RFMainWindow::OnTrainInfoExport);

	CButtonUI* patient_traindetail_export = static_cast<CButtonUI*>(m_pm.FindControl(_T("patient_traindetail_export")));
	patient_traindetail_export->OnNotify += MakeDelegate(this, &RFMainWindow::OnTrainDetailExport);


	CButtonUI* traindetail_chart_export = static_cast<CButtonUI*>(m_pm.FindControl(_T("traindetail_chart_export")));
	traindetail_chart_export->OnNotify += MakeDelegate(this, &RFMainWindow::OnTrainDataExport);


	CButtonUI* train_main_page_active = static_cast<CButtonUI*>(m_pm.FindControl(_T("train_main_page_active")));
	train_main_page_active->OnNotify += MakeDelegate(this, &RFMainWindow::OnActiveTrain);

	CButtonUI* train_main_page_passive = static_cast<CButtonUI*>(m_pm.FindControl(_T("train_main_page_passive")));
	train_main_page_passive->OnNotify += MakeDelegate(this, &RFMainWindow::OnPassiveTrain);

	CButtonUI* pf_last_page = static_cast<CButtonUI*>(m_pm.FindControl(_T("pf_last_page")));
	pf_last_page->OnNotify += MakeDelegate(this, &RFMainWindow::OnPatientTrainInfoPrevPage);

	CButtonUI* pf_page1 = static_cast<CButtonUI*>(m_pm.FindControl(_T("pf_page1")));
	pf_page1->OnNotify += MakeDelegate(this, &RFMainWindow::OnPatientTrainInfoPage1);

	CButtonUI* pf_page2 = static_cast<CButtonUI*>(m_pm.FindControl(_T("pf_page2")));
	pf_page2->OnNotify += MakeDelegate(this, &RFMainWindow::OnPatientTrainInfoPage2);

	CButtonUI* pf_page3 = static_cast<CButtonUI*>(m_pm.FindControl(_T("pf_page3")));
	pf_page3->OnNotify += MakeDelegate(this, &RFMainWindow::OnPatientTrainInfoPage3);

	CButtonUI* pf_page4 = static_cast<CButtonUI*>(m_pm.FindControl(_T("pf_page4")));
	pf_page4->OnNotify += MakeDelegate(this, &RFMainWindow::OnPatientTrainInfoPage4);

	CButtonUI* pf_next_page = static_cast<CButtonUI*>(m_pm.FindControl(_T("pf_next_page")));
	pf_next_page->OnNotify += MakeDelegate(this, &RFMainWindow::OnPatientTrainInfoNextPage);

	CButtonUI* pf_search = static_cast<CButtonUI*>(m_pm.FindControl(_T("pf_search")));
	pf_search->OnNotify += MakeDelegate(this, &RFMainWindow::OnPatientTrainSearch);


	CButtonUI* pd_last_page = static_cast<CButtonUI*>(m_pm.FindControl(_T("pd_last_page")));
	pd_last_page->OnNotify += MakeDelegate(this, &RFMainWindow::OnPatientTrainDetailPrevPage);

	CButtonUI* pd_page1 = static_cast<CButtonUI*>(m_pm.FindControl(_T("pd_page1")));
	pd_page1->OnNotify += MakeDelegate(this, &RFMainWindow::OnPatientTrainDetailPage1);

	CButtonUI* pd_page2 = static_cast<CButtonUI*>(m_pm.FindControl(_T("pd_page2")));
	pd_page2->OnNotify += MakeDelegate(this, &RFMainWindow::OnPatientTrainDetailPage2);

	CButtonUI* pd_page3 = static_cast<CButtonUI*>(m_pm.FindControl(_T("pd_page3")));
	pd_page3->OnNotify += MakeDelegate(this, &RFMainWindow::OnPatientTrainDetailPage3);

	CButtonUI* pd_page4 = static_cast<CButtonUI*>(m_pm.FindControl(_T("pd_page4")));
	pd_page4->OnNotify += MakeDelegate(this, &RFMainWindow::OnPatientTrainDetailPage4);

	CButtonUI* pd_next_page = static_cast<CButtonUI*>(m_pm.FindControl(_T("pd_next_page")));
	pd_next_page->OnNotify += MakeDelegate(this, &RFMainWindow::OnPatientTrainDetailNextPage);

	CButtonUI* traindetail_search = static_cast<CButtonUI*>(m_pm.FindControl(_T("traindetail_search")));
	traindetail_search->OnNotify += MakeDelegate(this, &RFMainWindow::OnPatientTrainDetailSearch);

	CButtonUI* passive_play = static_cast<CButtonUI*>(m_pm.FindControl(_T("passive_play")));
	passive_play->OnNotify += MakeDelegate(this, &RFMainWindow::OnPassiveTrainPlay);

	CButtonUI* passive_play10 = static_cast<CButtonUI*>(m_pm.FindControl(_T("passive_play10")));
	passive_play10->OnNotify += MakeDelegate(this, &RFMainWindow::OnPassiveTrainPlayByAuto);

	CButtonUI* passive_list = static_cast<CButtonUI*>(m_pm.FindControl(_T("passive_list")));
	passive_list->OnNotify += MakeDelegate(this, &RFMainWindow::OnPassiveTrainPlayByOrder);

	CButtonUI* passive_recover = static_cast<CButtonUI*>(m_pm.FindControl(_T("passive_recover")));
	passive_recover->OnNotify += MakeDelegate(this, &RFMainWindow::OnPassiveTrainRecover);

	CButtonUI* passive_volume = static_cast<CButtonUI*>(m_pm.FindControl(_T("passive_volume")));
	passive_volume->OnNotify += MakeDelegate(this, &RFMainWindow::OnPassiveTrainVolumeSet);

	CButtonUI* zd_gjjd_chart_btn = static_cast<CButtonUI*>(m_pm.FindControl(_T("zd_gjjd_chart_btn")));
	zd_gjjd_chart_btn->OnNotify += MakeDelegate(this, &RFMainWindow::OnZDGGJDChart);
	
	CButtonUI* zd_wl_chart_btn = static_cast<CButtonUI*>(m_pm.FindControl(_T("zd_wl_chart_btn")));
	zd_wl_chart_btn->OnNotify += MakeDelegate(this, &RFMainWindow::OnZDWLChart);

	CButtonUI* bd_zgj_chart_btn = static_cast<CButtonUI*>(m_pm.FindControl(_T("bd_zgj_chart_btn")));
	bd_zgj_chart_btn->OnNotify += MakeDelegate(this, &RFMainWindow::OnBDZGJChart);
	
	CButtonUI* bd_jgj_chart_btn = static_cast<CButtonUI*>(m_pm.FindControl(_T("bd_jgj_chart_btn")));
	bd_jgj_chart_btn->OnNotify += MakeDelegate(this, &RFMainWindow::OnBDJGJChart);

	CButtonUI* emg_chart_btn = static_cast<CButtonUI*>(m_pm.FindControl(_T("emg_chart_btn")));
	emg_chart_btn->OnNotify += MakeDelegate(this, &RFMainWindow::OnEMGChart);

	CButtonUI* emg_gjyd_chart_btn = static_cast<CButtonUI*>(m_pm.FindControl(_T("emg_gjyd_chart_btn")));
	emg_gjyd_chart_btn->OnNotify += MakeDelegate(this, &RFMainWindow::OnEMGYDJDChart);

	CButtonUI* passive_train_add = static_cast<CButtonUI*>(m_pm.FindControl(_T("passive_train_add")));
	passive_train_add->OnNotify += MakeDelegate(this, &RFMainWindow::OnAddAction);

	CButtonUI* eye_mode_setting = static_cast<CButtonUI*>(m_pm.FindControl(_T("eye_mode_setting")));
	eye_mode_setting->OnNotify += MakeDelegate(this, &RFMainWindow::OnEyeModeSetting);

	CCheckBoxUI* eye_mode_startstop = static_cast<CCheckBoxUI*>(m_pm.FindControl(_T("eye_mode_startstop")));
	eye_mode_startstop->OnNotify += MakeDelegate(this, &RFMainWindow::OnEyeModeStartStop);

	CButtonUI* eye_mode_recovery = static_cast<CButtonUI*>(m_pm.FindControl(_T("eye_mode_recovery")));
	eye_mode_recovery->OnNotify += MakeDelegate(this, &RFMainWindow::OnEyeModeRecovery);

	CButtonUI* emg_mode_start = static_cast<CButtonUI*>(m_pm.FindControl(_T("emg_mode_start")));
	emg_mode_start->OnNotify += MakeDelegate(this, &RFMainWindow::OnEMGModeStart);

	CButtonUI* emg_mode_recovery = static_cast<CButtonUI*>(m_pm.FindControl(_T("emg_mode_recovery")));
	emg_mode_recovery->OnNotify += MakeDelegate(this, &RFMainWindow::OnEMGModeRecovery);

	CButtonUI* zhudong_game1 = static_cast<CButtonUI*>(m_pm.FindControl(_T("zhudong_game1")));
	if (zhudong_game1 != NULL) zhudong_game1->OnNotify += MakeDelegate(this, &RFMainWindow::OnZhudongFeiji);


	CButtonUI* zhudong_game2 = static_cast<CButtonUI*>(m_pm.FindControl(_T("zhudong_game2")));
	if (zhudong_game2 != NULL) zhudong_game2->OnNotify += MakeDelegate(this, &RFMainWindow::OnZhudongBiaoqiang);


	CButtonUI* zhudong_game3 = static_cast<CButtonUI*>(m_pm.FindControl(_T("zhudong_game3")));
	if (zhudong_game3 != NULL) zhudong_game3->OnNotify += MakeDelegate(this, &RFMainWindow::OnZhudongDuimutou);

	CButtonUI* btn_game4 = static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_game4")));
	btn_game4->OnNotify += MakeDelegate(this, &RFMainWindow::OnGame4);


	CButtonUI* btn_game3 = static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_game3")));
	btn_game3->OnNotify += MakeDelegate(this, &RFMainWindow::OnGame3);


	CButtonUI* btn_game2 = static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_game2")));
	btn_game2->OnNotify += MakeDelegate(this, &RFMainWindow::OnGame2);

	CCheckBoxUI* game4_nandu_select = static_cast<CCheckBoxUI*>(m_pm.FindControl(_T("game4_nandu_select")));
	game4_nandu_select->OnNotify += MakeDelegate(this, &RFMainWindow::OnGame4NanduSetingMenu); 
	
	CButtonUI* game4_start = static_cast<CButtonUI*>(m_pm.FindControl(_T("game4_start")));
	game4_start->OnNotify += MakeDelegate(this, &RFMainWindow::OnGame4Start);

	CButtonUI* game4_recovery = static_cast<CButtonUI*>(m_pm.FindControl(_T("game4_recovery")));
	game4_recovery->OnNotify += MakeDelegate(this, &RFMainWindow::OnGame4Recovery);

	CButtonUI* evaluation1 = static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation1")));
	evaluation1->OnNotify += MakeDelegate(this, &RFMainWindow::OnEvaluation1);

	CButtonUI* evaluation2 = static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation2")));
	evaluation2->OnNotify += MakeDelegate(this, &RFMainWindow::OnEvaluation2);

	CButtonUI* evaluation3 = static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation3")));
	evaluation3->OnNotify += MakeDelegate(this, &RFMainWindow::OnEvaluation3);


	CButtonUI* ev_last_page = static_cast<CButtonUI*>(m_pm.FindControl(_T("ev_last_page")));
	ev_last_page->OnNotify += MakeDelegate(this, &RFMainWindow::OnEVLastPage);

	CButtonUI* ev_page1 = static_cast<CButtonUI*>(m_pm.FindControl(_T("ev_page1")));
	ev_page1->OnNotify += MakeDelegate(this, &RFMainWindow::OnEVPage1);

	CButtonUI* ev_page2 = static_cast<CButtonUI*>(m_pm.FindControl(_T("ev_page2")));
	ev_page2->OnNotify += MakeDelegate(this, &RFMainWindow::OnEVPage2);

	CButtonUI* ev_page3 = static_cast<CButtonUI*>(m_pm.FindControl(_T("ev_page3")));
	ev_page3->OnNotify += MakeDelegate(this, &RFMainWindow::OnEVPage3);

	CButtonUI* ev_page4 = static_cast<CButtonUI*>(m_pm.FindControl(_T("ev_page4")));
	ev_page4->OnNotify += MakeDelegate(this, &RFMainWindow::OnEVPage4);

	CButtonUI* ev_next_page = static_cast<CButtonUI*>(m_pm.FindControl(_T("ev_next_page")));
	ev_next_page->OnNotify += MakeDelegate(this, &RFMainWindow::OnEVNextPage);

	CButtonUI* evaluation_add =  static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation_add")));
	evaluation_add->OnNotify += MakeDelegate(this, &RFMainWindow::OnEVAdd);

	CButtonUI* evaluation_add_prev = static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation_add_prev")));
	evaluation_add_prev->OnNotify += MakeDelegate(this, &RFMainWindow::OnEVAddPrev);

	CButtonUI* evaluation_add_submit = static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation_add_submit")));
	evaluation_add_submit->OnNotify += MakeDelegate(this, &RFMainWindow::OnEVAddSubmit);

	CButtonUI* evaluation_add_next = static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation_add_next")));
	evaluation_add_next->OnNotify += MakeDelegate(this, &RFMainWindow::OnEVAddNext);

	CButtonUI* evd_last_page = static_cast<CButtonUI*>(m_pm.FindControl(_T("evd_last_page")));
	evd_last_page->OnNotify += MakeDelegate(this, &RFMainWindow::OnEVDLastPage);

	CButtonUI* evd_page1 = static_cast<CButtonUI*>(m_pm.FindControl(_T("evd_page1")));
	evd_page1->OnNotify += MakeDelegate(this, &RFMainWindow::OnEVDPage1);

	CButtonUI* evd_page2 = static_cast<CButtonUI*>(m_pm.FindControl(_T("evd_page2")));
	evd_page2->OnNotify += MakeDelegate(this, &RFMainWindow::OnEVDPage2);

	CButtonUI* evd_page3 = static_cast<CButtonUI*>(m_pm.FindControl(_T("evd_page3")));
	evd_page3->OnNotify += MakeDelegate(this, &RFMainWindow::OnEVDPage3);

	CButtonUI* evd_page4 = static_cast<CButtonUI*>(m_pm.FindControl(_T("evd_page4")));
	evd_page4->OnNotify += MakeDelegate(this, &RFMainWindow::OnEVDPage4);

	CButtonUI* evd_next_page = static_cast<CButtonUI*>(m_pm.FindControl(_T("evd_next_page")));
	evd_next_page->OnNotify += MakeDelegate(this, &RFMainWindow::OnEVDNextPage);

	CButtonUI* evaluation_start = static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation_start")));
	evaluation_start->OnNotify += MakeDelegate(this, &RFMainWindow::OnEVStart);
	
	CButtonUI* evaluation_end = static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation_end")));
	evaluation_end->OnNotify += MakeDelegate(this, &RFMainWindow::OnEVStop);

	CButtonUI* evaluation_begin1 = static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation_begin1")));
	evaluation_begin1->OnNotify += MakeDelegate(this, &RFMainWindow::OnEVBegin1);

	CButtonUI* evaluation_begin2 = static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation_begin2")));
	evaluation_begin2->OnNotify += MakeDelegate(this, &RFMainWindow::OnEVBegin2);
	
}

LRESULT RFMainWindow::OnCommunicate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CUIThread *pCurrentThread = (CUIThread*)wParam;
	if (pCurrentThread == NULL)
		return 0;

	CTask *pTask = (CTask*)lParam;
	if (pTask == NULL)
		return 0;

	pCurrentThread->ExecuteTask(pTask);
	return 0;
}

LRESULT RFMainWindow::OnAppClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);


	CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
	if (pControl && _tcsncmp(pControl->GetName(), _T("cell"), 4) == 0) {
		std::string name = TGUTF16ToUTF8((std::wstring)pControl->GetName());
		int row = atoi(name.substr(4, 1).c_str());
		wchar_t cell_name[32] = _T("");
		wsprintf(cell_name, _T("cell%d2"), row);

		CLabelUI* pName = static_cast<CLabelUI*>(m_pm.FindControl(cell_name));
		if (pName) {
			m_current_patient.name = pName->GetText();
			//m_current_patient.id = 1;
		}

		wsprintf(cell_name, _T("cell%d1"), row);
		CLabelUI* pID = static_cast<CLabelUI*>(m_pm.FindControl(cell_name));
		if (pID) {
			m_current_patient.id = _wtoi(((std::wstring)pID->GetText()).c_str());
		}

		ShowMainPage();

		bHandled = TRUE;
	}

	if (pControl && pControl->m_pParent && _tcsncmp(pControl->m_pParent->GetName(), _T("row"), 3) == 0) {
		std::string name = TGUTF16ToUTF8((std::wstring)pControl->m_pParent->GetName());
		int row = atoi(name.substr(3, 2).c_str()) - 1;
		wchar_t cell_name[32] = _T("");
		wsprintf(cell_name, _T("cell%d2"), row);

		CLabelUI* pName = static_cast<CLabelUI*>(m_pm.FindControl(cell_name));
		if (pName) {
			m_current_patient.name = pName->GetText();
			//m_current_patient.id = 1;
		}

		wsprintf(cell_name, _T("cell%d1"), row);
		CLabelUI* pID = static_cast<CLabelUI*>(m_pm.FindControl(cell_name));
		if (pID) {
			m_current_patient.id = _wtoi(((std::wstring)pID->GetText()).c_str());
		}

		ShowMainPage();

		bHandled = TRUE;
	}

	if (pControl && _tcscmp(pControl->GetName(), _T("manager_patient_detail")) == 0) {
		bHandled = TRUE;
		
		
		if (pControl && pControl->GetParent() && pControl->GetParent()->GetParent()) {
			CControlUI* pParent = pControl->GetParent()->GetParent();

			std::wstring name = pParent->GetName();
			name = name.substr(name.length() - 1, 1);
			int row = _wtoi(name.c_str());

			ShowPatientDetail(RFPatientsManager::get()->m_current_page, row - 1);
		}
	} else if (pControl && _tcscmp(pControl->GetName(), _T("manager_patient_modify")) == 0)  {
		bHandled = TRUE;

		if (pControl && pControl->GetParent() && pControl->GetParent()->GetParent()) {
			CControlUI* pParent = pControl->GetParent()->GetParent();

			std::wstring name = pParent->GetName();
			name = name.substr(name.length() - 1, 1);
			int row = _wtoi(name.c_str());

			ShowPatientEdit(RFPatientsManager::get()->m_current_page, row - 1);
		}
	}  else if (pControl && _tcscmp(pControl->GetName(), _T("manager_patient_delete")) == 0)  {
		bHandled = TRUE;
	
		if (pControl && pControl->GetParent() && pControl->GetParent()->GetParent()) {
			CContainerUI* pParent = static_cast<CContainerUI*>(pControl->GetParent()->GetParent());

			if (!RFDeletePatientConfirmDialog(GetHWND())) {
				return 0;
			}

			std::wstring name = pParent->GetName();
			name = name.substr(name.length() - 1, 1);
			int row = _wtoi(name.c_str());
			wchar_t cell_name[128] = _T("");
			wsprintf(cell_name, _T("manage_cell%d1"), row - 1);
			CLabelUI* pLabel = static_cast<CLabelUI*>(pParent->FindSubControl(cell_name));
			if (pLabel) {
				DeletePatient(_wtoi(((std::wstring)pLabel->GetText()).c_str()));
			}
		}
	} else if (pControl && _tcsncmp(pControl->GetName(), _T("patient_information_detail"), _tcslen(_T("patient_information_detail"))) == 0) {
		std::wstring name = pControl->GetName();

		std::wstring cellname = _T("pf_cell") + name.substr(name.size() - 1, 1) + _T("1");
		CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(cellname.c_str()));
		
		ShowPatientTrainDetail((std::wstring)pLabel->GetText());
	} else if (pControl && _tcsncmp(pControl->GetName(), _T("train_detail_detail"), _tcslen(_T("train_detail_detail"))) == 0) {
		int id = pControl->GetTag();

		ShowTrainDataChartPage(id);
	} else if (pControl && _tcsncmp(pControl->GetName(), _T("eval_detail"), _tcslen(_T("eval_detail"))) == 0) {
		std::wstring name = pControl->GetName();
		std::wstring cellname = _T("eval_cell") + name.substr(name.size() - 1, 1) + _T("1");
		CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(cellname.c_str()));
		ShowEVDetail((std::wstring)pLabel->GetText());
	}


	return 0;
}

LRESULT RFMainWindow::OnDuiCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	CDialogBuilder builder;
	CVerticalLayoutUI* pRoot = static_cast<CVerticalLayoutUI*>(builder.Create(_T("skin.xml"), (UINT)0,  NULL, &m_pm));
	
	CDialogBuilder builder1;
	CContainerUI *pSelectPatient = static_cast<CContainerUI*>(builder1.Create(_T("select.xml"), (UINT)0));
	//wchar_t message[1024];
	//builder1.GetLastErrorMessage(message, 1024);
	//builder1.GetLastErrorLocation(message, 1024);
	pRoot->Add(pSelectPatient);

	CDialogBuilder builder2;
	CContainerUI *pManagerPatient = static_cast<CContainerUI*>(builder2.Create(_T("manager.xml"), (UINT)0));
	pRoot->Add(pManagerPatient);

	CDialogBuilder builder3;
	CContainerUI *pManagerPatientDetail = static_cast<CContainerUI*>(builder3.Create(_T("detail.xml"), (UINT)0));
	pRoot->Add(pManagerPatientDetail);

	CDialogBuilder builder4;
	CContainerUI *pManagerPatientEdit = static_cast<CContainerUI*>(builder4.Create(_T("edit.xml"), (UINT)0));
	pRoot->Add(pManagerPatientEdit);

	CDialogBuilder builder5;
	CContainerUI *pAbout = static_cast<CContainerUI*>(builder5.Create(_T("about.xml"), (UINT)0));
	pRoot->Add(pAbout);

	CDialogBuilder builder6;
	CContainerUI *pAdd = static_cast<CContainerUI*>(builder6.Create(_T("add.xml"), (UINT)0));
	pRoot->Add(pAdd);

	CDialogBuilder builder7;
	CContainerUI *pPersonor = static_cast<CContainerUI*>(builder7.Create(_T("personor.xml"), (UINT)0));
	pRoot->Add(pPersonor);

	CDialogBuilder builder8;
	CContainerUI *pTrain = static_cast<CContainerUI*>(builder8.Create(_T("train.xml"), (UINT)0));
	pRoot->Add(pTrain);

	CDialogBuilder builder9;
	CContainerUI *pActiveTrain = static_cast<CContainerUI*>(builder9.Create(_T("active_train.xml"), (UINT)0, this));
	pRoot->Add(pActiveTrain);

	CDialogBuilder builder10;
	CContainerUI *pTrainInfo = static_cast<CContainerUI*>(builder10.Create(_T("traininfo.xml"), (UINT)0));
	pRoot->Add(pTrainInfo);

	CDialogBuilder builder11;
	CContainerUI *pTrainDetail = static_cast<CContainerUI*>(builder11.Create(_T("traindetail.xml"), (UINT)0));
	pRoot->Add(pTrainDetail);

	CDialogBuilder builder12;
	CContainerUI *pPassiveTrain = static_cast<CContainerUI*>(builder12.Create(_T("passive_train.xml"), (UINT)0, this));
	pRoot->Add(pPassiveTrain);

	CDialogBuilder builder13;
	CContainerUI *pEyeMode = static_cast<CContainerUI*>(builder13.Create(_T("eyemode.xml"), (UINT)0, this));
	pRoot->Add(pEyeMode);

	CDialogBuilder builder14;
	CContainerUI *pEMGMode = static_cast<CContainerUI*>(builder14.Create(_T("emgmode.xml"), (UINT)0, this));
	pRoot->Add(pEMGMode);

	CDialogBuilder builder15;
	CContainerUI *pTrainDetailChart = static_cast<CContainerUI*>(builder15.Create(_T("traindetailchart.xml"), (UINT)0, this));
	pRoot->Add(pTrainDetailChart);


	CDialogBuilder builder16;
	CContainerUI *pGame4 = static_cast<CContainerUI*>(builder16.Create(_T("game4.xml"), (UINT)0, this, &m_pm));
	pRoot->Add(pGame4);

	CDialogBuilder builder17;
	CContainerUI *pEvaluation = static_cast<CContainerUI*>(builder17.Create(_T("evaluation_system.xml"), (UINT)0, this, &m_pm));
	pRoot->Add(pEvaluation);

	CDialogBuilder builder18;
	CContainerUI *pEvaluationHistory = static_cast<CContainerUI*>(builder18.Create(_T("evaluation_history.xml"), (UINT)0, this, &m_pm));
	pRoot->Add(pEvaluationHistory);

	CDialogBuilder builder19;
	CContainerUI *pEvaluationAdd = static_cast<CContainerUI*>(builder19.Create(_T("evaluation_add.xml"), (UINT)0, this, &m_pm));
	pRoot->Add(pEvaluationAdd);

	CDialogBuilder builder20;
	CContainerUI *pEvaluationDetail = static_cast<CContainerUI*>(builder20.Create(_T("evaluation_detail.xml"), (UINT)0, this, &m_pm));
	pRoot->Add(pEvaluationDetail);

	CDialogBuilder builder21;
	CContainerUI *pEvaluationYDGNAdd = static_cast<CContainerUI*>(builder21.Create(_T("evaluation_ydgn_add.xml"), (UINT)0, this, &m_pm));
	pRoot->Add(pEvaluationYDGNAdd);

	CDialogBuilder builder22;
	CContainerUI *pEvaluationYDGNADetail = static_cast<CContainerUI*>(builder22.Create(_T("evaluation_ydgn_detail.xml"), (UINT)0, this, &m_pm));
	pRoot->Add(pEvaluationYDGNADetail);
	
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);
	
	m_login_page = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("login_bg")));
	m_login_input_page = static_cast<CContainerUI*>(m_pm.FindControl(_T("input_login")));
	m_login_success_page = static_cast<CContainerUI*>(m_pm.FindControl(_T("success_login")));
	m_main_page = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("main_page")));
	m_patient_select_page = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("select_patient")));
	m_patient_manager_page = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("manager_patient")));
	m_patient_detail_page = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("manager_patient_detail")));
	m_patient_edit_page = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("manager_patient_bianji")));
	m_patient_add_page = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("manager_patient_add")));
	m_about_page = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("about")));
	m_personor_info_edit_page = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("personor_info_edit_page")));
	m_train_main_page = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("train_main_page")));
	m_active_train_page = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("active_train_page")));
	m_patient_train_info = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("patient_information")));
	m_patient_train_detail = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("patient_traindetail")));
	m_passive_train_page = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("passive_train_page")));
	m_eyemode_page = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("eye_mode")));
	m_emgmode_page = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("emg_mode")));
	m_traindetail_chart = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("traindetail_chart")));
	m_active_train_game4_page = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("active_train_game4_page")));
	m_evaluation_page =  static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("evaluation_page")));
	m_evaluation_history_page =  static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("evaluation_history_page")));
	m_evaluation_add_page =   static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("evaluation_add_page")));
	m_evaluation_detail_page =   static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("evaluation_detail_page")));
	m_evaluation_ydgn_add_page =   static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("evaluation_ydgn_add_page")));
	m_evaluation_ydgn_detail_page = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("evaluation_ydgn_detail_page")));
	//m_active_train_page_list = static_cast<CVerticalLayoutUI>(m_pm.FindControl(_T("active_train_page_list")));


	m_welcom_menu = static_cast<CButtonUI*>(m_pm.FindControl(_T("welcom_menu")));
	m_welcom_menu->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);

	CButtonUI* pWelcom = static_cast<CButtonUI*>(m_pm.FindControl(_T("about_welcom")));
	pWelcom->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);

	pWelcom = static_cast<CButtonUI*>(m_pm.FindControl(_T("manager_patient_add_welcom")));
	pWelcom->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);

	pWelcom = static_cast<CButtonUI*>(m_pm.FindControl(_T("manager_patient_detail_welcom")));
	pWelcom->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);

	pWelcom = static_cast<CButtonUI*>(m_pm.FindControl(_T("manager_patient_bianji_welcom")));
	pWelcom->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);

	pWelcom = static_cast<CButtonUI*>(m_pm.FindControl(_T("manager_patient_welcom")));
	pWelcom->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);

	pWelcom = static_cast<CButtonUI*>(m_pm.FindControl(_T("personor_welcom")));
	pWelcom->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);

	pWelcom = static_cast<CButtonUI*>(m_pm.FindControl(_T("select_patient_welcom")));
	pWelcom->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);

	pWelcom = static_cast<CButtonUI*>(m_pm.FindControl(_T("train_main_page_welcom")));
	pWelcom->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);

	pWelcom = static_cast<CButtonUI*>(m_pm.FindControl(_T("active_train_page_welcom")));
	pWelcom->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);

	pWelcom = static_cast<CButtonUI*>(m_pm.FindControl(_T("passive_train_page_welcom")));
	pWelcom->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);

	pWelcom = static_cast<CButtonUI*>(m_pm.FindControl(_T("emg_mode_welcom")));
	pWelcom->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);

	pWelcom = static_cast<CButtonUI*>(m_pm.FindControl(_T("eye_mode_welcom")));
	pWelcom->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);

	pWelcom = static_cast<CButtonUI*>(m_pm.FindControl(_T("patient_information_welcom")));
	pWelcom->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);

	pWelcom = static_cast<CButtonUI*>(m_pm.FindControl(_T("patient_traindetail_welcom")));
	pWelcom->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);


	pWelcom = static_cast<CButtonUI*>(m_pm.FindControl(_T("traindetail_chart_welcom")));
	pWelcom->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);
	
	pWelcom = static_cast<CButtonUI*>(m_pm.FindControl(_T("active_train_game4_welcom")));
	pWelcom->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);

	pWelcom = static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation_page_welcom")));
	pWelcom->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);

	pWelcom = static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation_history_welcom")));
	pWelcom->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);

	pWelcom = static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation_detail_welcom")));
	pWelcom->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);

	pWelcom = static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation_ydgn_add_welcom")));
	pWelcom->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);

	pWelcom = static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation_add_welcom")));
	pWelcom->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);

	pWelcom = static_cast<CButtonUI*>(m_pm.FindControl(_T("evaluation_ydgn_detail_welcom")));
	pWelcom->OnNotify += MakeDelegate(this, &RFMainWindow::OnPersonerCenterMenu);

	m_main_tip = static_cast<CButtonUI*>(m_pm.FindControl(_T("main_tip")));

	CButtonUI* login = static_cast<CButtonUI*>(m_pm.FindControl(_T("login")));
	login->OnNotify += MakeDelegate(this, &RFMainWindow::OnLogin);

	m_welcom = static_cast<CLabelUI*>(m_pm.FindControl(_T("welcom")));
	m_manager_patient_header = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("manager_patient_header")));
	m_manager_patient_filterheader = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("manager_patient_filter_header")));



	CButtonUI* entry = static_cast<CButtonUI*>(m_pm.FindControl(_T("enter")));
	entry->OnNotify += MakeDelegate(this, &RFMainWindow::OnEntry);

	CButtonUI* main_enter = static_cast<CButtonUI*>(m_pm.FindControl(_T("main_enter")));
	main_enter->OnNotify += MakeDelegate(this, &RFMainWindow::OnSelectPatient);

	CButtonUI* main_manager = static_cast<CButtonUI*>(m_pm.FindControl(_T("main_manager")));
	main_manager->OnNotify += MakeDelegate(this, &RFMainWindow::OnManagerPatient);

	CButtonUI* main_train = static_cast<CButtonUI*>(m_pm.FindControl(_T("main_train")));
	main_train->OnNotify += MakeDelegate(this, &RFMainWindow::OnPatientTrain);

	CButtonUI* main_train_info = static_cast<CButtonUI*>(m_pm.FindControl(_T("main_train_info")));
	main_train_info->OnNotify += MakeDelegate(this, &RFMainWindow::OnPatientTrainMessage);

	CButtonUI* main_assessment = static_cast<CButtonUI*>(m_pm.FindControl(_T("main_assessment")));
	main_assessment->OnNotify += MakeDelegate(this, &RFMainWindow::OnPassessment);

	CButtonUI* main_about = static_cast<CButtonUI*>(m_pm.FindControl(_T("main_about")));
	main_about->OnNotify += MakeDelegate(this, &RFMainWindow::OnAbout);

	CButtonUI* train_main_page_eye = static_cast<CButtonUI*>(m_pm.FindControl(_T("train_main_page_eye")));
	train_main_page_eye->OnNotify += MakeDelegate(this, &RFMainWindow::OnEyeModeTrainPage); 

	CButtonUI* train_main_page_sEMG = static_cast<CButtonUI*>(m_pm.FindControl(_T("train_main_page_sEMG")));
	train_main_page_sEMG->OnNotify += MakeDelegate(this, &RFMainWindow::OnEMGModeTrainPage); 
	

	Init();

	BindSelectPatientPageEvent();
	BindManagerPatientPageEvent();
	return 0;
}

LRESULT RFMainWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	return 0;
}

LRESULT RFMainWindow::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT RFMainWindow::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}


LRESULT RFMainWindow::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT RFMainWindow::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT RFMainWindow::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	if( !::IsZoomed(*this) ) {
		RECT rcSizeBox = m_pm.GetSizeBox();
		if( pt.y < rcClient.top + rcSizeBox.top ) {
			if( pt.x < rcClient.left + rcSizeBox.left ) 
				return HTTOPLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) 
				return HTTOPRIGHT;

			return HTTOP;
		}
		else if( pt.y > rcClient.bottom - rcSizeBox.bottom ) {
			if( pt.x < rcClient.left + rcSizeBox.left ) 
				return HTBOTTOMLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) 
				return HTBOTTOMRIGHT;

			return HTBOTTOM;
		}

		if( pt.x < rcClient.left + rcSizeBox.left ) 
			return HTLEFT;

		if( pt.x > rcClient.right - rcSizeBox.right ) 
			return HTRIGHT;
	}

	RECT rcCaption = m_pm.GetCaptionRect();
	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
			if (pControl && _tcsncmp(pControl->GetName(), _T("row"), 3) == 0)
			{
				return HTCLIENT;
			}

			if (pControl && _tcsncmp(pControl->GetName(), _T("cell"), 4) == 0)
			{
				return HTCLIENT;
			}

			if (pControl && pControl->m_pParent && _tcsncmp(pControl->m_pParent->GetName(), _T("row"), 3) == 0) {
				return HTCLIENT;
			}

			if( pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
				_tcscmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
				_tcscmp(pControl->GetClass(), _T("CheckBoxUI")) != 0 &&
				_tcscmp(pControl->GetClass(), _T("TextUI")) != 0 &&
				_tcscmp(pControl->GetClass(), _T("EditUI")) != 0 &&
				_tcscmp(pControl->GetClass(), _T("RichEditUI")) != 0 &&
				_tcscmp(pControl->GetClass(), _T("ComboUI")) != 0 &&
				_tcscmp(pControl->GetClass(), _T("DateTimeUI")) != 0 &&
				_tcscmp(pControl->GetClass(), _T("ScrollBarUI")) != 0
				)
				//return HTCAPTION;
				return HTCLIENT;
	}

	return HTCLIENT;
}


LRESULT RFMainWindow::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SIZE szRoundCorner = m_pm.GetRoundCorner();
	if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
		CDuiRect rcWnd;
		::GetWindowRect(*this, &rcWnd);
		rcWnd.Offset(-rcWnd.left, -rcWnd.top);
		rcWnd.right++; rcWnd.bottom++;
		HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
		::SetWindowRgn(*this, hRgn, TRUE);
		::DeleteObject(hRgn);
	}

	bHandled = FALSE;
	return 0;
}

LRESULT RFMainWindow::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDuiRect rcWork = oMonitor.rcWork;
	rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

	LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
	lpMMI->ptMaxPosition.x	= rcWork.left;
	lpMMI->ptMaxPosition.y	= rcWork.top;
	lpMMI->ptMaxSize.x		= rcWork.right;
	lpMMI->ptMaxSize.y		= rcWork.bottom;

	bHandled = FALSE;
	return 0;
}

LRESULT RFMainWindow::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
	if( wParam == SC_CLOSE ) {
		::PostQuitMessage(0L);
		bHandled = TRUE;
		return 0;
	}
	BOOL bZoomed = ::IsZoomed(*this);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if( ::IsZoomed(*this) != bZoomed ) {
		if( !bZoomed ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
			if( pControl ) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
			if( pControl ) pControl->SetVisible(true);
		}
		else {
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
			if( pControl ) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
			if( pControl ) pControl->SetVisible(false);
		}
	}
	return lRes;
}

LRESULT RFMainWindow::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	if( uMsg == WM_KEYDOWN ) {
		if( wParam == VK_ESCAPE ) {
			::PostQuitMessage(0L);
			return true;
		}

	}
	return 0;
}

LRESULT RFMainWindow::OnMenuClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	CDuiString *name = (CDuiString*)wParam;

	if (*name == _T("menu_personor_center")) {
		ShowPersonorPage();
	}

	if (*name == _T("menu_change_account")) {
		m_current_patient.id = -1;
		RFPatientsManager::release();
		RFPatientsManager::get();
		ShowLoginPage();
	}

	if (*name == _T("menu_exit_system")) {
		::PostQuitMessage(0L);
	}

	if (*name == _T("nandu1")) {
		CWkeWebkitUI* game4 = static_cast<CWkeWebkitUI*>(m_pm.FindControl(_T("game4")));
		if (game4) {
			//CDuiString respath = CPaintManagerUI::GetResourcePath() + _T("/Plane2/index.html");
			//game4->SetFile((std::wstring)respath);

			game4->RunJS(_T("Checkpoint1();"));
		}
		m_robot.setDamping(0.1);
	} 
	if (*name == _T("nandu2")) {
		CWkeWebkitUI* game4 = static_cast<CWkeWebkitUI*>(m_pm.FindControl(_T("game4")));
		if (game4) {
			//CDuiString respath = CPaintManagerUI::GetResourcePath() + _T("/Plane1/index.html");
			//game4->SetFile((std::wstring)respath);
			game4->RunJS(_T("Checkpoint2();"));
		}
		m_robot.setDamping(0.3);
	}
	if (*name == _T("nandu3")) {
		CWkeWebkitUI* game4 = static_cast<CWkeWebkitUI*>(m_pm.FindControl(_T("game4")));
		if (game4) {
			//CDuiString respath = CPaintManagerUI::GetResourcePath() + _T("/Plane/index.html");
			//game4->SetFile((std::wstring)respath);
			game4->RunJS(_T("Checkpoint3();"));
		}
		m_robot.setDamping(0.5);
	}
	return 0;
}

LRESULT RFMainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch( uMsg ) {
		case WM_CREATE:        lRes = OnDuiCreate(uMsg, wParam, lParam, bHandled); break;
		case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
		case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
		case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO: lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
		case WM_SYSCOMMAND:    lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
		case WM_COMMUNICATE:   lRes = OnCommunicate(uMsg, wParam, lParam, bHandled); break;
		case WM_LBUTTONDOWN:   lRes = OnAppClick(uMsg, wParam, lParam, bHandled); break;
		case WM_KEYDOWN:	   lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
		case WM_MENUCLICK:	   lRes = OnMenuClick(uMsg, wParam, lParam, bHandled); break;
		case WM_EMG_DATA_SAMPLE_MSG: lRes = OnEmgSampleData(uMsg, wParam, lParam); break;
		default:
			bHandled = FALSE;
	}

	if( bHandled ) return lRes;
	if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}


LRESULT RFMainWindow::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if( uMsg == WM_KEYDOWN ) {
		if( wParam == VK_ESCAPE ) {
			Close();
			return true;
		}

	}
	return false;
}

LRESULT RFMainWindow::OnEmgSampleData(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	double* pRawData = (double*)lParam;
	


	EMGLineWaveData wavedata;
	wavedata.x = RFMainWindow::MainWindow->m_emgmode_tracetime;
	wavedata.y = pRawData[0];//0+(500-0)*rand()/(RAND_MAX + 1.0); 
	wavedata.y = 500 * wavedata.y;
	m_emgmode_data[0].push_back(wavedata.y);
	CEMGWaveUI* pEMGWave1 = static_cast<CEMGWaveUI*>(RFMainWindow::MainWindow->m_pm.FindControl(_T("emgmode_wave1")));
	pEMGWave1->PushData(wavedata);

	wavedata.y = pRawData[1];
	m_emgmode_data[0].push_back(wavedata.y);
	wavedata.y = 500 * wavedata.y;
	m_emgmode_data[1].push_back(wavedata.y);
	CEMGWaveUI* pEMGWave2 = static_cast<CEMGWaveUI*>(RFMainWindow::MainWindow->m_pm.FindControl(_T("emgmode_wave2")));
	pEMGWave2->PushData(wavedata);

	wavedata.y = pRawData[2];
	m_emgmode_data[0].push_back(wavedata.y);
	wavedata.y = 500 * wavedata.y;
	m_emgmode_data[2].push_back(wavedata.y);
	CEMGWaveUI* pEMGWave3 = static_cast<CEMGWaveUI*>(RFMainWindow::MainWindow->m_pm.FindControl(_T("emgmode_wave3")));
	pEMGWave3->PushData(wavedata);

	wavedata.y = pRawData[3];
	m_emgmode_data[0].push_back(wavedata.y);
	wavedata.y = 500 * wavedata.y;
	m_emgmode_data[3].push_back(wavedata.y);
	CEMGWaveUI* pEMGWave4 = static_cast<CEMGWaveUI*>(RFMainWindow::MainWindow->m_pm.FindControl(_T("emgmode_wave4")));
	pEMGWave4->PushData(wavedata);

	if (RFMainWindow::MainWindow->m_emgmode_tracetime % 200 == 0) {
		m_emgmode_ydjd[0].push_back(pRawData[4]);
		m_emgmode_ydjd[1].push_back(pRawData[5]);
	}
	RFMainWindow::MainWindow->m_emgmode_tracetime += 100;

	delete[] pRawData;
	return true;
}

bool RFMainWindow::OnLogin(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	std::wstring usr = _T(""), pwd = _T("");
	CEditUI* usredit = static_cast<CEditUI*>(m_pm.FindControl(_T("input_login_usr_txt")));
	if (usredit) {
		usr = usredit->GetText();
	}

	CEditUI* pwdedit = static_cast<CEditUI*>(m_pm.FindControl(_T("input_login_pwd_txt")));
	if (pwdedit) {
		pwd = pwdedit->GetText();
	}

	if (!m_mysql_connected) {
		RFConnectFailDialog(GetHWND());

		CTask::Assign(CTask::NotWait, Panic(), NULL, EventHandle(&RFMySQLThread::ReConnect), RFMainWindow::UIThread, RFMainWindow::DBThread);
		return true;
	}

	if (usr.empty() || pwd.empty()) {
		RFLoginFailDialog(GetHWND());
		return true;
	}

	LoginInfo *pLoginInfo = new LoginInfo;
	pLoginInfo->usrname = usr;
	pLoginInfo->usrpwd = pwd;
	pLoginInfo->role = 2;

	CTask::Assign(CTask::NotWait, Panic(), pLoginInfo, EventHandle(&RFMySQLThread::Login), RFMainWindow::UIThread, RFMainWindow::DBThread);

	return true;
}

int RFMainWindow::OnLoginOK(EventArg *pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();

	LoginInfo *pLoginInfo = pTask->GetContext<LoginInfo*>();
	if (pLoginInfo != NULL) {
		if (pLoginInfo->logined == 1) {
			std::wstring welcome = _T("欢迎您，") + pLoginInfo->login_user + _T("!");
			RFMainWindow::MainWindow->m_welcom->SetText(welcome.c_str());
			RFMainWindow::MainWindow->ShowLoginSuccessPage();
			RFMainWindow::MainWindow->m_pm.Invalidate(RFMainWindow::MainWindow->m_welcom->m_rcItem);
		} else {
			RFLoginFailDialog(RFMainWindow::MainWindow->GetHWND());

			delete pLoginInfo;
			pLoginInfo = NULL;
			return 1;
		}

		RFMainWindow::MainWindow->m_login_info = *pLoginInfo;
		RFPatientsManager::get()->SetHospitalID(pLoginInfo->hospitalid);
		RFPatientsManager::get()->setDoctorID(pLoginInfo->doctorid);
		RFPatientsManager::get()->step();
		RFPatientsTrainInfo::get()->setHospitalID(pLoginInfo->hospitalid);
		RFPatientsTrainInfo::get()->setDoctorID(pLoginInfo->doctorid);
		RFPatientsTrainInfo::get()->LoadPatientTrainInfo();
		RFPatientsTrainDetails::get()->SetHospitalID(pLoginInfo->hospitalid);
		RFPatientsTrainDetails::get()->SetDoctorID(pLoginInfo->doctorid);
		RFPatientsTrainDetails::get()->LoadPatientTrainDetails();
		RFEvaluationData::get()->setDoctorID(pLoginInfo->doctorid);

		delete pLoginInfo;
		pLoginInfo = NULL;
	} else {
		RFLoginFailDialog(RFMainWindow::MainWindow->GetHWND());
	}
	

	return 1;
}

int RFMainWindow::OnConnectOK(EventArg* pArg)
{
	RFMainWindow::MainWindow->m_mysql_connected = true;
	return 1;
}

int RFMainWindow::OnConnectFail(EventArg* pArg)
{
	RFMainWindow::MainWindow->m_mysql_connected = false;
	return 1;
}

bool RFMainWindow::OnGame4NanduSetingMenu(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;
	if (!pMsg->pSender) {
		return false;
	}

	POINT point;
	point.x = pMsg->pSender->GetPos().right - (173 * RF_WINDOW_WIDTH) / RF_DESIGN_WINDOW_WIDTH;
	point.y = pMsg->pSender->GetPos().bottom + (8 * RF_WINDOW_HEIGHT) / RF_DESIGN_WINDOW_HEIGHT;

	::ClientToScreen(m_hWnd, &point);

	CMenuWnd* pMenu = CMenuWnd::CreateMenu(NULL, _T("game4nandu.xml"), point, &m_pm);
	//pMenu->ResizeMenu();
}

bool RFMainWindow::OnPersonerCenterMenu(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;
	if (!pMsg->pSender) {
		return false;
	}

	POINT point;
	point.x = pMsg->pSender->GetPos().right - 217;
	point.y = pMsg->pSender->GetPos().bottom + 8;

	::ClientToScreen(m_hWnd, &point);

	CMenuWnd* pMenu = CMenuWnd::CreateMenu(NULL, _T("personer_menu.xml"), point, &m_pm);
}


bool RFMainWindow::OnEntry(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	std::wstring welcom = _T("欢迎您，") + RFMainWindow::MainWindow->m_login_info.login_user + _T("！∨");

	ShowMainPage();
	RFMainWindow::MainWindow->m_welcom_menu->SetText(welcom.c_str());

	return true;
}

bool RFMainWindow::OnSelectPatient(void* pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;
	
	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("select_patient_welcom")));
	pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

	UpdatePatientPage(RFPatientsManager::get()->getPage(RFPatientsManager::get()->m_current_page));
	UpdatePageNumber(RFPatientsManager::get()->m_current_page);
	ShowSelectPatientPage();

	return true;
}

bool RFMainWindow::OnManagerPatient(void* pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;
	
	if (m_current_patient.id < 0) {
		RFSelectPatientDialog(GetHWND());
		return true;
	}

	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("manager_patient_welcom")));
	pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

	UpdateManagePatientPage(RFPatientsManager::get()->getPage(RFPatientsManager::get()->m_current_page));
	UpdateManagePageNumber(RFPatientsManager::get()->m_current_page);
	ShowManagerPatientPage();
	return true;
}


bool RFMainWindow::OnPatientTrain(void* pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	if (m_current_patient.id < 0) {
		RFSelectPatientDialog(GetHWND());
		return true;
	}

	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("train_main_page_welcom")));
	pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

	ShowTrainPage();
	return true;
}


bool RFMainWindow::OnPatientTrainFromActiveTrain(void* pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	if (m_current_patient.id < 0) {
		RFSelectPatientDialog(GetHWND());
		return true;
	}

	CVerticalLayoutUI* active_train_page_list = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("active_train_page_list")));
	if (active_train_page_list->IsVisible()) {
		active_train_page_list->SetVisible(false);
		CVerticalLayoutUI* active_train_page_main = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("active_train_page_main")));
		active_train_page_main->SetVisible(true);

		return true;
	}

	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("train_main_page_welcom")));
	pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

	ShowTrainPage();
	return true;
}

bool RFMainWindow::OnPatientTrainFromPassiveTrain(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	m_passive_train_action.StopPlay();

	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("train_main_page_welcom")));
	pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

	ShowTrainPage();
	return true;
}

bool RFMainWindow::OnPatientTrainMessage(void* pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	if (m_current_patient.id < 0) {
		RFSelectPatientDialog(GetHWND());
		return true;
	}

	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("patient_information_welcom")));
	pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

	UpdateTrainInfoPage(RFPatientsTrainInfo::get()->getPageElement(RFPatientsTrainInfo::get()->m_current_page));
	UpdateTrainInfoPageNumber(RFPatientsTrainInfo::get()->m_current_page);
	ShowPatientTrainInformation();

	return true;
}

bool RFMainWindow::OnPatientTrainDetail(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("patient_traindetail_welcom")));
	pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

	ShowPatientTrainDetail(m_current_patient_id_detail);
	return true;
}

bool RFMainWindow::OnPassessment(void* pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	if (m_current_patient.id < 0) {
		RFSelectPatientDialog(GetHWND());
		return true;
	}

	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_page_welcom")));
	pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

	ShowEvaluationPage();
	return true;
}

bool RFMainWindow::OnAbout(void* pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	ShowAboutPage();

	return true;
}

bool RFMainWindow::OnPatientTrainFromEyeMode(void *pParam)
{
	m_robot.stopEyeMove();
	m_robot.exitEyeMode();
	RFMainWindow::OnPatientTrain(pParam);
	return true;
}

bool RFMainWindow::OnChangePersonInfoPage(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CVerticalLayoutUI *pPersonorInfo = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("personor_info")));
	pPersonorInfo->SetVisible(true);

	CVerticalLayoutUI *pChangePwdPage = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("changepwd_box")));
	pChangePwdPage->SetVisible(false);

	UpdatePersonInfo();
	return true;
}

bool RFMainWindow::OnChangePasswordPage(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;


	CVerticalLayoutUI *pPersonorInfo = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("personor_info")));
	pPersonorInfo->SetVisible(false);

	CVerticalLayoutUI *pChangePwdPage = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("changepwd_box")));
	pChangePwdPage->SetVisible(true);

	return true;
}

bool RFMainWindow::OnPrevPage(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);
	int cur_page = RFPatientsManager::get()->m_current_page;
	if (cur_page < 1) {
		return true;
	}
	
	RFPatientsManager::get()->setPage(cur_page - 1);

	UpdatePageNumber(cur_page - 1);
	UpdatePatientPage(RFPatientsManager::get()->getPage(cur_page - 1));

	return true;
}

bool RFMainWindow::OnPage1(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;
	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);
	
	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;
	RFPatientsManager::get()->setPage(page);

	UpdatePageNumber(page);
	UpdatePatientPage(RFPatientsManager::get()->getPage(page));

	return true;
}

bool RFMainWindow::OnPage2(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;
	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);

	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;
	RFPatientsManager::get()->setPage(page);

	UpdatePageNumber(page);
	UpdatePatientPage(RFPatientsManager::get()->getPage(page));
	return true;
}
bool RFMainWindow::OnPage3(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;
	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);

	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;
	RFPatientsManager::get()->setPage(page);

	UpdatePageNumber(page);
	UpdatePatientPage(RFPatientsManager::get()->getPage(page));

	return true;
}
bool RFMainWindow::OnPage4(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;
	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);

	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;
	RFPatientsManager::get()->setPage(page);

	UpdatePageNumber(page);
	UpdatePatientPage(RFPatientsManager::get()->getPage(page));

	return true;
}
bool RFMainWindow::OnNextPage(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;
	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);

	int page = RFPatientsManager::get()->m_current_page + 1;
	std::list<PatientInfo> patients = RFPatientsManager::get()->getPage(page);
	if (patients.size() < 1) {
		return true;
	}

	RFPatientsManager::get()->setPage(page);

	UpdatePageNumber(page);
	UpdatePatientPage(patients);
	return true;
}

bool RFMainWindow::OnSearch(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;
	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);

	CLabelUI *pLable = static_cast<CLabelUI*>(m_pm.FindControl(_T("select_patient_search_txt")));
	std::wstring patientname = pLable->GetText();

	RFPatientsManager::get()->search(patientname);

	return true;
}


bool RFMainWindow::OnManagerPrevPage(void* pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);
	int cur_page = RFPatientsManager::get()->m_current_page;
	if (cur_page < 1) {
		return true;
	}

	RFPatientsManager::get()->setPage(cur_page - 1);

	UpdateManagePageNumber(cur_page - 1);
	UpdateManagePatientPage(RFPatientsManager::get()->getPage(cur_page - 1));

	return true;
}

bool RFMainWindow::OnManagerPage1(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;
	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);

	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;
	RFPatientsManager::get()->setPage(page);

	UpdateManagePageNumber(page);
	UpdateManagePatientPage(RFPatientsManager::get()->getPage(page));

	return true;
}

bool RFMainWindow::OnManagerPage2(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;
	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);

	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;
	RFPatientsManager::get()->setPage(page);

	UpdateManagePageNumber(page);
	UpdateManagePatientPage(RFPatientsManager::get()->getPage(page));

	return true;
}

bool RFMainWindow::OnManagerPage3(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;
	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);

	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;
	RFPatientsManager::get()->setPage(page);

	UpdateManagePageNumber(page);
	UpdateManagePatientPage(RFPatientsManager::get()->getPage(page));

	return true;
}

bool RFMainWindow::OnManagerPage4(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;
	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);

	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;
	RFPatientsManager::get()->setPage(page);

	UpdateManagePageNumber(page);
	UpdateManagePatientPage(RFPatientsManager::get()->getPage(page));

	return true;
}

bool RFMainWindow::OnManagerNextPage(void *pParam)
{

	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;
	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);

	int page = RFPatientsManager::get()->m_current_page + 1;
	std::list<PatientInfo> patients = RFPatientsManager::get()->getPage(page);
	if (patients.size() < 1) {
		return true;
	}

	RFPatientsManager::get()->setPage(page);

	UpdateManagePageNumber(page);
	UpdateManagePatientPage(patients);
	return true;
}

bool RFMainWindow::OnManagerSearch(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;
	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);

	CLabelUI *pLable = static_cast<CLabelUI*>(m_pm.FindControl(_T("manager_patient_search_txt")));
	std::wstring patientid = pLable->GetText();

	RFPatientsManager::get()->search(patientid);

	return true;
}

bool RFMainWindow::OnManagerPatientAdd(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	RFPatientsManager::get()->nextPatientID();

	return true;
}

bool RFMainWindow::OnPatientTrainInfoPrevPage(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;
	
	int cur_page = RFPatientsTrainInfo::get()->m_current_page;
	if (cur_page < 1) {
		return true;
	}

	RFPatientsTrainInfo::get()->setCurrentPage(cur_page - 1);

	UpdateTrainInfoPageNumber(cur_page - 1);
	UpdateTrainInfoPage(RFPatientsTrainInfo::get()->getPageElement(cur_page - 1));

	return true;
}

bool RFMainWindow::OnPatientTrainInfoPage1(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CControlUI* pControl = pMsg->pSender;
	if (NULL == pControl) {
		return true;
	}
	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;
	RFPatientsTrainInfo::get()->setCurrentPage(page);

	UpdateTrainInfoPageNumber(page);
	UpdateTrainInfoPage(RFPatientsTrainInfo::get()->getPageElement(page));

	return true;
}


bool RFMainWindow::OnPatientTrainInfoPage2(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CControlUI* pControl = pMsg->pSender;
	if (NULL == pControl) {
		return true;
	}
	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;
	RFPatientsTrainInfo::get()->setCurrentPage(page);

	UpdateTrainInfoPageNumber(page);
	UpdateTrainInfoPage(RFPatientsTrainInfo::get()->getPageElement(page));

	return true;
}


bool RFMainWindow::OnPatientTrainInfoPage3(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;


	CControlUI* pControl = pMsg->pSender;
	if (NULL == pControl) {
		return true;
	}
	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;
	RFPatientsTrainInfo::get()->setCurrentPage(page);

	UpdateTrainInfoPageNumber(page);
	UpdateTrainInfoPage(RFPatientsTrainInfo::get()->getPageElement(page));
	return true;
}


bool RFMainWindow::OnPatientTrainInfoPage4(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CControlUI* pControl = pMsg->pSender;
	if (NULL == pControl) {
		return true;
	}
	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;
	RFPatientsTrainInfo::get()->setCurrentPage(page);

	UpdateTrainInfoPageNumber(page);
	UpdateTrainInfoPage(RFPatientsTrainInfo::get()->getPageElement(page));

	return true;
}

bool RFMainWindow::OnPatientTrainInfoNextPage(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;
	
	int page = RFPatientsTrainInfo::get()->m_current_page + 1;
	std::list<PatientTrainInfo> trains = RFPatientsTrainInfo::get()->getPageElement(page);
	if (trains.size() < 1) {
		return true;
	}

	RFPatientsTrainInfo::get()->setCurrentPage(page);

	UpdateTrainInfoPageNumber(page);
	UpdateTrainInfoPage(trains);

	return true;
}

bool RFMainWindow::OnPatientTrainSearch(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CLabelUI *pLable = static_cast<CLabelUI*>(m_pm.FindControl(_T("pf_search_txt")));
	std::wstring patientname = pLable->GetText();

	std::list<PatientTrainInfo> trains = RFPatientsTrainInfo::get()->search(patientname);
	UpdateTrainInfoPage(trains);
	return true;
}

bool RFMainWindow::OnPatientTrainDetailSearch(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CLabelUI *pLable = static_cast<CLabelUI*>(m_pm.FindControl(_T("traindetail_search_txt")));
	std::wstring create_time = pLable->GetText();
	if (!create_time.empty()) {
		std::list<PatientTrainDetails> trains = RFPatientsTrainDetails::get()->search(create_time);
		UpdateTrainDetailPage(trains);
	} else {
		UpdateTrainDetailPageNumber(RFPatientsTrainDetails::get()->m_currentpage);
		UpdateTrainDetailPage(RFPatientsTrainDetails::get()->getPageElement(RFPatientsTrainDetails::get()->m_currentpage));
	}
	
	return true;
}

bool RFMainWindow::OnPatientTrainDetailPrevPage(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	int cur_page = RFPatientsTrainDetails::get()->m_currentpage;
	if (cur_page < 1) {
		return true;
	}

	RFPatientsTrainDetails::get()->SetCurrentPage(cur_page - 1);

	UpdateTrainDetailPageNumber(cur_page - 1);
	UpdateTrainDetailPage(RFPatientsTrainDetails::get()->getPageElement(cur_page - 1));

	return true;
}

bool RFMainWindow::OnPatientTrainDetailPage1(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CControlUI* pControl = pMsg->pSender;
	if (NULL == pControl) {
		return true;
	}
	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;
	RFPatientsTrainDetails::get()->SetCurrentPage(page);

	UpdateTrainDetailPageNumber(page);
	UpdateTrainDetailPage(RFPatientsTrainDetails::get()->getPageElement(page));
	return true;
}

bool RFMainWindow::OnPatientTrainDetailPage2(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CControlUI* pControl = pMsg->pSender;
	if (NULL == pControl) {
		return true;
	}
	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;
	RFPatientsTrainDetails::get()->SetCurrentPage(page);

	UpdateTrainDetailPageNumber(page);
	UpdateTrainDetailPage(RFPatientsTrainDetails::get()->getPageElement(page));
	return true;
}

bool RFMainWindow::OnPatientTrainDetailPage3(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CControlUI* pControl = pMsg->pSender;
	if (NULL == pControl) {
		return true;
	}
	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;
	RFPatientsTrainDetails::get()->SetCurrentPage(page);

	UpdateTrainDetailPageNumber(page);
	UpdateTrainDetailPage(RFPatientsTrainDetails::get()->getPageElement(page));
	return true;
}

bool RFMainWindow::OnPatientTrainDetailPage4(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CControlUI* pControl = pMsg->pSender;
	if (NULL == pControl) {
		return true;
	}
	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;
	RFPatientsTrainDetails::get()->SetCurrentPage(page);

	UpdateTrainDetailPageNumber(page);
	UpdateTrainDetailPage(RFPatientsTrainDetails::get()->getPageElement(page));

	return true;
}

bool RFMainWindow::OnPatientTrainDetailNextPage(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;


	int page = RFPatientsTrainDetails::get()->m_currentpage + 1;
	std::list<PatientTrainDetails> trains = RFPatientsTrainDetails::get()->getPageElement(page);
	if (trains.size() < 1) {
		return true;
	}

	RFPatientsTrainDetails::get()->SetCurrentPage(page);

	UpdateTrainDetailPageNumber(page);
	UpdateTrainDetailPage(trains);

	return true;
}

bool RFMainWindow::OnDetailPageDelete(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);
	CEditUI* pEdit = static_cast<CEditUI*>(pControl->m_pManager->FindControl(_T("detail_patient_id")));
	if (pEdit) {
		DeletePatient(_wtoi(((std::wstring)pEdit->GetText()).c_str()));
	}

	return true;
}

bool RFMainWindow::OnDetailPageEdit(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);
	CEditUI* pEdit = static_cast<CEditUI*>(pControl->m_pManager->FindControl(_T("detail_patient_id")));
	if (pEdit) {
		ShowPatientEdit(_wtoi(((std::wstring)pEdit->GetText()).c_str()));
	}

	return true;
}

bool RFMainWindow::OnSavePatient(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	PatientInfo patient;

	patient.hospitalid = m_login_info.hospitalid;
	patient.doctorid = m_login_info.doctorid;
	patient.flag = 0;

	CEditUI* pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("bianji_patient_createtime")));
	if (pEdit) {
		patient.createtime = pEdit->GetText();
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("bianji_patient_id")));
	if (pEdit) {
		patient.id = _wtoi(((std::wstring)pEdit->GetText()).c_str());
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("bianji_patient_name")));
	if (pEdit) {
		patient.name= pEdit->GetText();
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("bianji_patient_lasttime")));
	if (pEdit) {
		patient.lasttreattime= pEdit->GetText();
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("bianji_patient_sex")));
	if (pEdit) {
		patient.sex= pEdit->GetText();
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("bianji_patient_totaltime")));
	if (pEdit) {
		patient.totaltreattime= pEdit->GetText();
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("bianji_patient_age")));
	if (pEdit) {
		patient.age= _wtoi(((std::wstring)pEdit->GetText()).c_str());
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("bianji_patient_detail")));
	if (pEdit) {
		patient.recoverdetail= pEdit->GetText();
	}

	CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(m_pm.FindControl(_T("bianji_patient_remark")));
	if (pEdit) {
		patient.remarks= pRichEdit->GetText();
	}

	RFPatientsManager::get()->modify(patient);

	return true;
}

bool RFMainWindow::OnSavePersonInfo(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	LoginInfo doctor = GetPersonInfo();
	RFPatientsManager::get()->modify(doctor);
}

bool RFMainWindow::OnModifyPwdInfo(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	std::wstring oldpwd;
	CEditUI *pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_old_pwd")));
	if (pEdit) {
		oldpwd = pEdit->GetText();
	}

	std::wstring newpwd1;
	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_new_pwd")));
	if (pEdit) {
		newpwd1 = pEdit->GetText();
	}

	std::wstring newpwd2;
	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_confirm_new_pwd")));
	if (pEdit) {
		newpwd2 = pEdit->GetText();
	}

	if (m_login_info.usrpwd != oldpwd) {
		RFModifyPwdFailedDialog(m_hWnd, 0);
		return true;
	}
	if (newpwd1 != newpwd2) {
		RFModifyPwdFailedDialog(m_hWnd, 1);
		return true;
	}

	RFModifyPwdFailedDialog(m_hWnd, 2);

	ModifyPWDInfo pwd;
	pwd.loginid = m_login_info.login_id;
	pwd.oldpwd = oldpwd;
	pwd.pwd = newpwd1;

	m_login_info.usrpwd = newpwd1;
	RFPatientsManager::get()->modifyPwd(pwd);
	return true;
}

bool RFMainWindow::OnAddSavePatient(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	PatientInfo patient;

	patient.hospitalid = m_login_info.hospitalid;
	patient.doctorid = m_login_info.doctorid;
	patient.flag = 0;

	//CComboUI* pComboYear = static_cast<CComboUI*>(m_pm.FindControl(_T("add_patient_createtime_year")));
	//CComboUI* pComboMonth = static_cast<CComboUI*>(m_pm.FindControl(_T("add_patient_createtime_month")));
	//CComboUI* pComboDay = static_cast<CComboUI*>(m_pm.FindControl(_T("add_patient_createtime_date")));
	//if (pComboYear && pComboMonth && pComboDay) {
	//	patient.createtime = pComboYear->GetText() + _T("-") + pComboMonth->GetText() + _T("-") + pComboDay->GetText();
	//}

	CEditUI *pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("add_patient_createtime")));
	if (pEdit) {
		patient.createtime = pEdit->GetText();
	}
	
	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("add_patient_id")));
	if (pEdit) {
		patient.id = _wtoi(((std::wstring)pEdit->GetText()).c_str());
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("add_patient_name")));
	if (pEdit) {
		patient.name= pEdit->GetText();
	}

	//pComboYear = static_cast<CComboUI*>(m_pm.FindControl(_T("add_patient_lasttime_year")));
	//pComboMonth = static_cast<CComboUI*>(m_pm.FindControl(_T("add_patient_lasttime_month")));
	//pComboDay = static_cast<CComboUI*>(m_pm.FindControl(_T("add_patient_lasttime_date")));
	//if (pComboYear && pComboMonth && pComboDay) {
	//	patient.lasttreattime = pComboYear->GetText() + _T("-") + pComboMonth->GetText() + _T("-") + pComboDay->GetText();
	//}

	COptionUI* pOption = static_cast<COptionUI*>(m_pm.FindControl(_T("add_patient_sex_man")));
	if (pOption) {
		patient.sex= pOption->IsSelected()?_T("男"):_T("女");
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("add_patient_totaltime")));
	if (pEdit) {
		patient.totaltreattime= pEdit->GetText();
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("add_patient_age")));
	if (pEdit) {
		patient.age= _wtoi(((std::wstring)pEdit->GetText()).c_str());
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("add_patient_detail")));
	if (pEdit) {
		patient.recoverdetail= pEdit->GetText();
	}

	CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(m_pm.FindControl(_T("add_patient_remark")));
	if (pEdit) {
		patient.remarks= pRichEdit->GetText();
	}

	RFPatientsManager::get()->add(patient);

	return true;
}



bool RFMainWindow::OnFilter(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("selectchanged"))
		return true;
	
	COptionUI* pOption = static_cast<COptionUI*>(pMsg->pSender);
	if (pOption->IsSelected()) {
		m_manager_patient_header->SetVisible(false);
		m_manager_patient_filterheader->SetVisible(true);
	} else {
		m_manager_patient_header->SetVisible(true);
		m_manager_patient_filterheader->SetVisible(false);


		PatientFilterParam param;
		param.name = _T("");
		param.sex = _T("");
		param.age_from = _T("");
		param.age_to = _T("");
		param.create_from = _T("");
		param.create_to = _T("");
		param.hospitalid = m_login_info.hospitalid;
		param.doctorid = m_login_info.doctorid;
		
		CEditUI* pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("manager_patient_name_search_txt")));
		if (pEdit) {
			param.name = pEdit->GetText();
		}

		pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("manager_patient_sex_search_txt")));
		if (pEdit) {
			param.sex = pEdit->GetText();
		}

		pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("manager_patient_age_search_from")));
		if (pEdit) {
			param.age_from = pEdit->GetText();
		}

		pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("manager_patient_age_search_to")));
		if (pEdit) {
			param.age_to = pEdit->GetText();
		}

		CDateTimeUI* pDate = static_cast<CDateTimeUI*>(m_pm.FindControl(_T("manager_patient_createtime_from")));
		if (pDate) {
			param.create_from = pDate->GetText();
		}

		pDate = static_cast<CDateTimeUI*>(m_pm.FindControl(_T("manager_patient_createtime_to")));
		if (pDate) {
			param.create_to = pDate->GetText();
		}

		RFPatientsManager::get()->filter(param);
	}

	return true;
}

bool RFMainWindow::OnExportFilterPatient(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	std::wstring savePath = GetSaveFilePath(m_pm.GetPaintWindow());
	if (savePath.empty()) {
		return true;
	}

	ExportPatientParam param;
	std::wstring templatePath = CPaintManagerUI::GetResourcePath() + _T("patientmanager.xlsx");
	param.path = templatePath;
	param.savepath = savePath;
	param.doctorid = m_login_info.doctorid;
	param.hospitalid = m_login_info.hospitalid;
	param.patientid = -1;
	RFPatientsManager::get()->exportPatient(param);

	return true;
}

bool RFMainWindow::OnImportPatient(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	std::vector<std::wstring> files;
	GetFilesFromDialog(GetHWND(), files, _T("*.xlsx|*.xls"), OFN_OVERWRITEPROMPT | OFN_EXPLORER);

	if (files.size() > 0) {
		RFPatientsManager::get()->importPatient(files[0]);
	}
	return true;
}

bool RFMainWindow::OnExportFilterPatientDetail(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	std::wstring savePath = GetSaveFilePath(m_pm.GetPaintWindow());
	if (savePath.empty()) {
		return true;
	}
	std::wstring templatePath = CPaintManagerUI::GetResourcePath() + _T("patientdetail.xlsx");


	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("detail_patient_id")));
	if (NULL == pLabel) {
		return true;
	}
	std::wstring patientid = pLabel->GetText();
	RFPatientsManager::get()->exportPatient(_wtoi(patientid.c_str()), templatePath, savePath);
}

bool RFMainWindow::OnTrainInfoExport(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	std::wstring savePath = GetSaveFilePath(m_pm.GetPaintWindow());
	if (savePath.empty()) {
		return true;
	}
	std::wstring templatePath = CPaintManagerUI::GetResourcePath() + _T("traininfo.xlsx");
	RFPatientsTrainInfo::get()->exportTrainInfo(templatePath, savePath);
}

bool RFMainWindow::OnTrainDetailExport(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	std::wstring savePath = GetSaveFilePath(m_pm.GetPaintWindow());
	if (savePath.empty()) {
		return true;
	}
	std::wstring templatePath = CPaintManagerUI::GetResourcePath() + _T("traindetail.xlsx");
	RFPatientsTrainDetails::get()->exportTrainDetail(templatePath, savePath);
}


bool RFMainWindow::OnActiveTrain(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("train_main_page_welcom")));
	pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

	ShowActiveTrainPage();
	return true;
}

bool RFMainWindow::OnActiveTrainFromGame(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("train_main_page_welcom")));
	pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

	m_robot.stopActiveMove();
	ShowActiveTrainPage();
	return true;
}

bool RFMainWindow::OnPassiveTrain(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("passive_train_page_welcom")));
	pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

	pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("passive_patient_name")));
	pLabel->SetText((_T("患者:") + m_current_patient.name).c_str());

	ShowPassiveTrainPage();
	return true;
}

bool RFMainWindow::OnPassiveTrainPlay(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	
	CCheckBoxUI* pPlay = static_cast<CCheckBoxUI*>(pMsg->pSender);
	if (!pPlay->GetCheck()) {
		bool playbyoder = true;
		COptionUI* pOption = static_cast<COptionUI*>(m_passive_train_page->FindSubControl(_T("passive_play10")));
		if (pOption->IsSelected()) {
			playbyoder = false;
		}
		m_passive_train_action.StartPlay(m_current_passivetraininfos, playbyoder);
	} else {
		m_passive_train_action.StopPlay();
	}
	
	return true;
}

bool RFMainWindow::OnPassiveTrainPlayNext(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;
	
	bool playbyoder = false;
	COptionUI* pOption = static_cast<COptionUI*>(m_passive_train_page->FindSubControl(_T("passive_play10")));
	if (pOption->IsSelected()) {
		playbyoder = true;
	}
	
	m_passive_train_action.PlayNext(playbyoder);
	return true;
}

bool RFMainWindow::OnPassiveTrainPlayPrev(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	bool playbyoder = false;
	COptionUI* pOption = static_cast<COptionUI*>(m_passive_train_page->FindSubControl(_T("passive_play10")));
	if (pOption->IsSelected()) {
		playbyoder = true;
	}

	m_passive_train_action.PlayPrev(playbyoder);
	return true;
}

bool RFMainWindow::OnPassiveTrainRecover(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	m_robot.resetPos();
	return true;
}

bool RFMainWindow::OnPassiveTrainPlayByOrder(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	bool playbyoder = false;
	COptionUI* pOption = static_cast<COptionUI*>(m_passive_train_page->FindSubControl(_T("passive_list")));
	if (pOption->IsSelected()) {
		playbyoder = true;
	}
	m_passive_train_action.SetPlayOrder(playbyoder);

	POINT point;
	point.x = pMsg->pSender->GetPos().left - 170 * RF_WINDOW_WIDTH / RF_DESIGN_WINDOW_WIDTH ;
	point.y = pMsg->pSender->GetPos().top - (280 * RF_WINDOW_HEIGHT) / RF_DESIGN_WINDOW_HEIGHT;

	::ClientToScreen(m_hWnd, &point);

	CMenuWnd* pMenu = CMenuWnd::CreateMenu(NULL, _T("playlistmenu.xml"), point, &m_pm);
	CListUI *pList = static_cast<CListUI*>(pMenu->m_pm.FindControl(_T("musiclist")));
	UpdateMusicList(pList);
	return true;
}

bool RFMainWindow::OnPassiveTrainPlayByAuto(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	bool playbyoder = true;
	COptionUI* pOption = static_cast<COptionUI*>(m_passive_train_page->FindSubControl(_T("passive_play10")));
	if (pOption->IsSelected()) {
		playbyoder = false;
	}
	m_passive_train_action.SetPlayOrder(playbyoder);
	return true;
}

bool RFMainWindow::OnPassiveTrainVolumeSet(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;
	CControlUI* pControl = pMsg->pSender;

	CPopupWidget* pWidget = new CPopupWidget;
	POINT point;
	RECT rc	= pControl->GetPos();
	//pControl->SetTag(m_MusicPlayer.GetVolume());
	point.x = rc.left + (rc.right - rc.left)/2 - 10;
	point.y = rc.top - 10;
	pWidget->Init(_T("Volume.xml"), m_hWnd, &m_pm, point, CHANGE_VOLUME);
	return true;
}

bool RFMainWindow::OnAddAction(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	RFBDAddActionDialog(GetHWND());
	return true;
}

bool RFMainWindow::OnBDAddActionToPlayList(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	if (m_id_passivetrainitem.find(pMsg->pSender->GetId()) != m_id_passivetrainitem.end()) {
		static int media_index = 0;

		MEDIA media = m_id_passivetrainitem[pMsg->pSender->GetId()];
		media.index = media_index++;
		m_current_passivetraininfos.push_back(media);
	}
	return true;
}

bool RFMainWindow::OnBDDeleteAction(void *pParam) {
	TNotifyUI *pMsg = static_cast<TNotifyUI *>(pParam);
	if (pMsg->sType != _T("click"))
		return true;
	if (m_delete_id_passivetrainitem.find(pMsg->pSender->GetId()) != m_delete_id_passivetrainitem.end()) {
		MEDIA media = m_delete_id_passivetrainitem[pMsg->pSender->GetId()];
		PassiveTrainInfo *pParam = new PassiveTrainInfo;
		*pParam = media.train;
		CTask::Assign(CTask::NotWait, Panic(), pParam, EventHandle(&RFMySQLThread::DeletePassiveTrainInfo), RFMainWindow::UIThread, RFMainWindow::DBThread);
	}
}

bool RFMainWindow::OnZDGGJDChart(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CLabelUI* traindetial_chart_zd_name = static_cast<CLabelUI*>(m_pm.FindControl(_T("traindetial_chart_zd_name")));
	traindetial_chart_zd_name->SetText(_T("关节运动角度"));

	CVerticalLayoutUI* p_zd_gjjd_chart_bk = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("zd_gjjd_chart_bk")));
	p_zd_gjjd_chart_bk->SetVisible(true);

	CHorizontalLayoutUI* p_zd_gjjd_chart_tuli = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("zd_gjjd_chart_tuli")));
	p_zd_gjjd_chart_tuli->SetVisible(true);

	CVerticalLayoutUI* zd_wl_chart_bk = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("zd_wl_chart_bk")));
	zd_wl_chart_bk->SetVisible(false);
	return true;
}

bool RFMainWindow::OnZDWLChart(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CLabelUI* traindetial_chart_zd_name = static_cast<CLabelUI*>(m_pm.FindControl(_T("traindetial_chart_zd_name")));
	traindetial_chart_zd_name->SetText(_T("握力"));

	CHorizontalLayoutUI* p_zd_gjjd_chart_tuli = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("zd_gjjd_chart_tuli")));
	p_zd_gjjd_chart_tuli->SetVisible(false);

	CVerticalLayoutUI* p_zd_gjjd_chart_bk = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("zd_gjjd_chart_bk")));
	p_zd_gjjd_chart_bk->SetVisible(false);

	CVerticalLayoutUI* zd_wl_chart_bk = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("zd_wl_chart_bk")));
	zd_wl_chart_bk->SetVisible(true);
	return true;
}

bool RFMainWindow::OnBDZGJChart(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CLabelUI* traindetial_chart_zd_name = static_cast<CLabelUI*>(m_pm.FindControl(_T("traindetial_chart_bd_name")));
	traindetial_chart_zd_name->SetText(_T("肘关节力矩值"));


	CVerticalLayoutUI* bd_jgj_chart_bk = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("bd_jgj_chart_bk")));
	bd_jgj_chart_bk->SetVisible(false);

	CVerticalLayoutUI* bd_zgj_chart_bk = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("bd_zgj_chart_bk")));
	bd_zgj_chart_bk->SetVisible(true);
	return true;
}

bool RFMainWindow::OnBDJGJChart(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CLabelUI* traindetial_chart_zd_name = static_cast<CLabelUI*>(m_pm.FindControl(_T("traindetial_chart_bd_name")));
	traindetial_chart_zd_name->SetText(_T("肩关节力矩值"));


	CVerticalLayoutUI* bd_jgj_chart_bk = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("bd_jgj_chart_bk")));
	bd_jgj_chart_bk->SetVisible(true);

	CVerticalLayoutUI* bd_zgj_chart_bk = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("bd_zgj_chart_bk")));
	bd_zgj_chart_bk->SetVisible(false);
	return true;
}

bool RFMainWindow::OnEMGChart(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CLabelUI* traindetial_chart_zd_name = static_cast<CLabelUI*>(m_pm.FindControl(_T("traindetial_chart_bd_name")));
	traindetial_chart_zd_name->SetText(_T("EMG信号"));

	CHorizontalLayoutUI* emg_chart_tuli = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("emg_chart_tuli")));
	emg_chart_tuli->SetVisible(true);

	CHorizontalLayoutUI* emg_gjjd_chart_tuli = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("emg_gjjd_chart_tuli")));
	emg_gjjd_chart_tuli->SetVisible(false);

	CVerticalLayoutUI* emg_chart_bk = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("emg_chart_bk")));
	emg_chart_bk->SetVisible(true);

	CVerticalLayoutUI* emg_gjyd_chart_bk = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("emg_gjyd_chart_bk")));
	emg_gjyd_chart_bk->SetVisible(false);
	return true;
}

bool RFMainWindow::OnEMGYDJDChart(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CLabelUI* traindetial_chart_zd_name = static_cast<CLabelUI*>(m_pm.FindControl(_T("traindetial_chart_emg_name")));
	traindetial_chart_zd_name->SetText(_T("关节运动角度"));

	CHorizontalLayoutUI* emg_chart_tuli = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("emg_chart_tuli")));
	emg_chart_tuli->SetVisible(false);

	CHorizontalLayoutUI* emg_gjjd_chart_tuli = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("emg_gjjd_chart_tuli")));
	emg_gjjd_chart_tuli->SetVisible(true);

	CVerticalLayoutUI* emg_chart_bk = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("emg_chart_bk")));
	emg_chart_bk->SetVisible(false);

	CVerticalLayoutUI* emg_gjyd_chart_bk = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("emg_gjyd_chart_bk")));
	emg_gjyd_chart_bk->SetVisible(true);
	return true;
}

bool RFMainWindow::OnTrainDataExport(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	std::wstring savePath = GetSaveFilePath(m_pm.GetPaintWindow());
	if (savePath.empty()) {
		return true;
	}
	//std::wstring templatePath = CPaintManagerUI::GetResourcePath() + _T("traindetail.xlsx");
	RFPatientsTrainData::get()->exportTrainData(savePath);
}

bool RFMainWindow::OnEyeModeSetting(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	double sudu = .0f;
	bool showvideo = false;
	if (RFEyeModeSettingDialog(GetHWND(), sudu, showvideo)) {
		CAVPlayerUI* pLeft = static_cast<CAVPlayerUI*>(m_pm.FindControl(_T("vlc_left_camera")));
		if (pLeft) {
			pLeft->m_isPlaying = showvideo;
			pLeft->ShowVideo(showvideo);
			pLeft->Invalidate();
		}

		CAVPlayerUI* pRight = static_cast<CAVPlayerUI*>(m_pm.FindControl(_T("vlc_right_camera")));
		if (pRight) {
			pRight->m_isPlaying = showvideo;
			pRight->ShowVideo(showvideo);
			pRight->Invalidate();
		}

		m_robot.setEyeVel(sudu);
	}
	return true;
}

bool RFMainWindow::OnEyeModeStartStop(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;
	CCheckBoxUI* pStartStop = static_cast<CCheckBoxUI*>(pMsg->pSender);
	if (!pStartStop->GetCheck()) {
		s_eyemode_data[0].clear();
		s_eyemode_data[1].clear();
		s_eyemode_start = time(NULL);
		m_robot.startEyeMove();

		StartEyeModeGameDetect();
	} else {
		StopEyeModeGameDetect();

		m_robot.stopEyeMove();
		s_eyemode_stop = time(NULL);
		SaveEyeModeDetectData();
		s_eyemode_data[0].clear();
		s_eyemode_data[1].clear();
	}

	return true;
}

bool RFMainWindow::OnEyeModeRecovery(void* pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	m_robot.resetPos();
	return true;
}

bool RFMainWindow::OnEyeModeTrainPage(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("eye_mode_welcom")));
	pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

	m_robot.enterEyeMode();
	ShowEyeModeTrainPage();
}

bool RFMainWindow::OnEMGModeTrainPage(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("emg_mode_welcom")));
	pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

	ShowEmgModeTrainPage();

	return true;
}


void OnEMGModeTimer(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	if (!RFMainWindow::MainWindow) {
		return;
	}
	

	EMGLineWaveData wavedata;
	wavedata.x = RFMainWindow::MainWindow->m_emgmode_tracetime;
	wavedata.y = 0+(500-0)*rand()/(RAND_MAX + 1.0); 

	CEMGWaveUI* pEMGWave1 = static_cast<CEMGWaveUI*>(RFMainWindow::MainWindow->m_pm.FindControl(_T("emgmode_wave1")));
	pEMGWave1->PushData(wavedata);

	CEMGWaveUI* pEMGWave2 = static_cast<CEMGWaveUI*>(RFMainWindow::MainWindow->m_pm.FindControl(_T("emgmode_wave2")));
	pEMGWave2->PushData(wavedata);

	CEMGWaveUI* pEMGWave3 = static_cast<CEMGWaveUI*>(RFMainWindow::MainWindow->m_pm.FindControl(_T("emgmode_wave3")));
	pEMGWave3->PushData(wavedata);

	CEMGWaveUI* pEMGWave4 = static_cast<CEMGWaveUI*>(RFMainWindow::MainWindow->m_pm.FindControl(_T("emgmode_wave4")));
	pEMGWave4->PushData(wavedata);


	RFMainWindow::MainWindow->m_emgmode_tracetime += 200;
	
}

bool RFMainWindow::OnEMGModeStart(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CCheckBoxUI *pCheckBox = static_cast<CCheckBoxUI*>(pMsg->pSender);
	if (!pCheckBox->GetCheck()) {
		m_robot.startEMGMove();
		m_emgmode_tracetime = 0;
		m_emg_createtime = time(NULL);
		for (int i = 0; i < 4; i++)
		{
			m_emgmode_data[i].clear();
		}
		m_emgmode_ydjd[0].clear();
		m_emgmode_ydjd[1].clear();

	} else {
		m_robot.stopEMGMove();

		SaveEMGTrainData();
		m_emgmode_tracetime = 0;
		m_emg_createtime = 0;
	}
	return true;
}

bool RFMainWindow::OnEMGModeRecovery(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	if (m_robot.isEMGMove()) {
		m_robot.stopEMGMove();
	}

	for (int i = 0; i < 4; i++)
	{
		m_emgmode_data[i].clear();
	}
	m_emgmode_ydjd[0].clear();
	m_emgmode_ydjd[1].clear();
	m_emgmode_tracetime = 0;
	CEMGWaveUI* pEMGWave1 = static_cast<CEMGWaveUI*>(RFMainWindow::MainWindow->m_pm.FindControl(_T("emgmode_wave1")));
	pEMGWave1->ClearData();

	CEMGWaveUI* pEMGWave2 = static_cast<CEMGWaveUI*>(RFMainWindow::MainWindow->m_pm.FindControl(_T("emgmode_wave2")));
	pEMGWave2->ClearData();

	CEMGWaveUI* pEMGWave3 = static_cast<CEMGWaveUI*>(RFMainWindow::MainWindow->m_pm.FindControl(_T("emgmode_wave3")));
	pEMGWave3->ClearData();

	CEMGWaveUI* pEMGWave4 = static_cast<CEMGWaveUI*>(RFMainWindow::MainWindow->m_pm.FindControl(_T("emgmode_wave4")));
	pEMGWave4->ClearData();

	m_robot.resetPos();
	return true;
}

bool RFMainWindow::OnZhudongFeiji(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CVerticalLayoutUI* active_train_page_main = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("active_train_page_main")));
	active_train_page_main->SetVisible(false);

	CVerticalLayoutUI* active_train_page_list = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("active_train_page_list")));
	active_train_page_list->SetVisible(true);
}

bool RFMainWindow::OnZhudongBiaoqiang(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CWkeWebkitUI* game4 = static_cast<CWkeWebkitUI*>(m_pm.FindControl(_T("game4")));
	if (game4) {
		CDuiString respath = CPaintManagerUI::GetResourcePath() + _T("/biaoqiang/index.html");
		game4->SetFile((std::wstring)respath);
	}

	CButtonUI* game4_start = static_cast<CButtonUI*>(m_pm.FindControl(_T("game4_start")));
	game4_start->SetVisible(false);

	ShowGame4();
	return true;
}

bool RFMainWindow::OnZhudongDuimutou(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CWkeWebkitUI* game4 = static_cast<CWkeWebkitUI*>(m_pm.FindControl(_T("game4")));
	if (game4) {
		CDuiString respath = CPaintManagerUI::GetResourcePath() + _T("/duimutou/index.html");
		game4->SetFile((std::wstring)respath);

		//game4->RunJS("zoom(2.0);");
	}

	CButtonUI* game4_start = static_cast<CButtonUI*>(m_pm.FindControl(_T("game4_start")));
	game4_start->SetVisible(false);
	ShowGame4();
	return true;
}

bool RFMainWindow::OnGame4(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	
	CWkeWebkitUI* game4 = static_cast<CWkeWebkitUI*>(m_pm.FindControl(_T("game4")));
	if (game4) {
		CDuiString respath = CPaintManagerUI::GetResourcePath() + _T("/Plane/index.html");
		game4->SetFile((std::wstring)respath);
	}

	CButtonUI* game4_start = static_cast<CButtonUI*>(m_pm.FindControl(_T("game4_start")));
	game4_start->SetVisible(false);
	ShowGame4();
	

	return true;
}

bool RFMainWindow::OnGame3(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;


	CWkeWebkitUI* game4 = static_cast<CWkeWebkitUI*>(m_pm.FindControl(_T("game4")));
	if (game4) {
		CDuiString respath = CPaintManagerUI::GetResourcePath() + _T("/Plane1/index.html");
		game4->SetFile((std::wstring)respath);
	}

	CButtonUI* game4_start = static_cast<CButtonUI*>(m_pm.FindControl(_T("game4_start")));
	game4_start->SetVisible(true);
	ShowGame4();


	return true;
}

bool RFMainWindow::OnGame2(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;


	CWkeWebkitUI* game4 = static_cast<CWkeWebkitUI*>(m_pm.FindControl(_T("game4")));
	if (game4) {
		CDuiString respath = CPaintManagerUI::GetResourcePath() + _T("/Plane2/index.html");
		game4->SetFile((std::wstring)respath);
	}

	CButtonUI* game4_start = static_cast<CButtonUI*>(m_pm.FindControl(_T("game4_start")));
	game4_start->SetVisible(true);
	ShowGame4();


	return true;
}


bool RFMainWindow::OnGame4Start(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CCheckBoxUI *pCheckBox = static_cast<CCheckBoxUI*>(pMsg->pSender);

	if (!pCheckBox->GetCheck()) {
		//s_active_game4_start = time(NULL);
		//s_active_begin_recode = true;
		//s_active_data[0].clear();
		//s_active_data[1].clear();
		//s_active_data_wl.clear();
		m_robot.startActiveMove();
	} else {
		//s_active_begin_recode = false;
		//s_active_game4_stop = time(NULL);
		m_robot.stopActiveMove();
		//SaveActiveGameDetectData();
		//s_active_data[0].clear();
		//s_active_data[1].clear();
		//s_active_data_wl.clear();
	}

	return true;
}

bool RFMainWindow::OnGame4Recovery(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	m_robot.stopActiveMove();
	m_robot.resetPos();
	return true;
}


bool RFMainWindow::OnMusicItemDelete(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;
	if (!pMsg->pSender || !pMsg->pSender->GetParent() || !pMsg->pSender->GetParent()->GetParent()) {
		return true;
	}

	
	CListContainerElementUI *pListContainerElement = static_cast<CListContainerElementUI*>(pMsg->pSender->GetParent()->GetParent());
	if (pListContainerElement && pListContainerElement) {
		CListUI* pList = static_cast<CListUI*>(pListContainerElement->GetOwner());
		int index = pList->GetItemIndex(pListContainerElement);
		if (index < 0) {
			return true;
		}

		CLabelUI* pLabel = static_cast<CLabelUI*>(pListContainerElement->FindSubControl(_T("music_cell01")));
		int pos = _wtoi(pLabel->GetText().GetData());
		int i = 0;	
		std::list<MEDIA>::iterator begin = m_current_passivetraininfos.begin();
		for (; begin != m_current_passivetraininfos.end(); begin++) {
			if (i == pos) {
				m_current_passivetraininfos.erase(begin);
				break;
			}

			i++;
		}

		pList->Remove(pListContainerElement);
	}
	
	return true;
}

bool RFMainWindow::OnEvaluation1(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_history_welcom")));
	pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());
	
	CControlUI* pImage = static_cast<CControlUI*>(m_pm.FindControl(_T("evaluation_history_image")));
	pImage->SetBkImage(_T("pg_fma_img.png"));

	CLabelUI* pName = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_history_name")));
	pName->SetText(_T("FMA评测"));

	CLabelUI* pScore = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_history_score")));

	m_evalution_type = 1;
	std::list<EvaluationData> datas;
	RFEvaluationData::get()->Load(1);
	RFEvaluationData::get()->setCurPage(0);
	UpdateEvaluationNumber(0);
	UpdateEvaluationPage(datas);
	ShowEvaluationHistoryPage();
	return true;
}

bool RFMainWindow::OnEvaluation2(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;
	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_history_welcom")));
	pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

	CControlUI* pImage = static_cast<CControlUI*>(m_pm.FindControl(_T("evaluation_history_image")));
	pImage->SetBkImage(_T("pg_mas_img.png"));

	CLabelUI* pName = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_history_name")));
	pName->SetText(_T("MAS评测"));

	CLabelUI* pScore = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_history_score")));

	m_evalution_type = 2;
	std::list<EvaluationData> datas;
	RFEvaluationData::get()->Load(2);
	RFEvaluationData::get()->setCurPage(0);
	UpdateEvaluationNumber(0);
	UpdateEvaluationPage(datas);
	ShowEvaluationHistoryPage();
	return true;
}

bool RFMainWindow::OnEvaluation3(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;
	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_history_welcom")));
	pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

	CControlUI* pImage = static_cast<CControlUI*>(m_pm.FindControl(_T("evaluation_history_image")));
	pImage->SetBkImage(_T("pg_yd_img.png"));

	CLabelUI* pName = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_history_name")));
	pName->SetText(_T("运动功能评测"));

	CLabelUI* pScore = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_history_score")));

	m_evalution_type = 3;
	std::list<EvaluationData> datas;
	RFEvaluationData::get()->Load(3);
	RFEvaluationData::get()->setCurPage(0);
	UpdateEvaluationNumber(0);
	UpdateEvaluationPage(datas);
	ShowEvaluationHistoryPage();
	return true;
}

int RFMainWindow::OnSearchOK(EventArg *pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();

	LoadPatientResult *pResult = pTask->GetContext<LoadPatientResult*>();
	if (pResult) {
		if (RFMainWindow::MainWindow->m_patient_select_page->IsVisible()) {
			RFMainWindow::MainWindow->UpdatePatientPage(pResult->patients);
		} else {
			RFMainWindow::MainWindow->UpdateManagePatientPage(pResult->patients);
		}
	}

	if (pResult) {
		delete pResult;
		pResult = NULL;
	}

	return 1;
}

int RFMainWindow::OnFilterOK(EventArg* pArg)
{
	RFMySQLThread *pCurrentThread = pArg->GetSender<RFMySQLThread*>();
	CTask *pTask = pArg->GetAttach<CTask*>();
	if (NULL == pTask)
	{
		return 1;
	}

	LoadPatientResult *pResult = pTask->GetContext<LoadPatientResult*>();
	if (pResult) {
		RFPatientsManager::get()->setPage(0);
		RFPatientsManager::get()->m_patients = pResult->patients;
		if (RFMainWindow::MainWindow->m_patient_select_page->IsVisible()) {
			RFMainWindow::MainWindow->UpdatePageNumber(0);
			RFMainWindow::MainWindow->UpdatePatientPage(pResult->patients);
		} else {
			RFMainWindow::MainWindow->UpdateManagePageNumber(0);
			RFMainWindow::MainWindow->UpdateManagePatientPage(pResult->patients);
		}
	}

	if (pResult) {
		delete pResult;
		pResult = NULL;
	}

	return 1;
}


bool RFMainWindow::OnReturnMainPage(void* pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	ShowMainPage();
	return true;
}

bool RFMainWindow::OnEVLastPage(void* pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);
	int cur_page = RFEvaluationData::get()->m_current_page;
	if (cur_page < 1) {
		return true;
	}

	int page = cur_page - 1;
	//if (RFEvaluationData::get()->GetElementNumber(page) > 0) {
		RFEvaluationData::get()->setCurPage(page);
		UpdateEvaluationNumber(page);
		UpdateEvaluationPage(RFEvaluationData::get()->Get(page));
	//}

	return true;
}

bool RFMainWindow::OnEVPage1(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);

	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;
	
	//if (RFEvaluationData::get()->GetElementNumber(page) > 0) {
		RFEvaluationData::get()->setCurPage(page);
		UpdateEvaluationNumber(page);
		UpdateEvaluationPage(RFEvaluationData::get()->Get(page));
	//}
	return true;
}

bool RFMainWindow::OnEVPage2(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);

	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;
	
	//if (RFEvaluationData::get()->GetElementNumber(page) > 0) {
		RFEvaluationData::get()->setCurPage(page);
		UpdateEvaluationNumber(page);
		UpdateEvaluationPage(RFEvaluationData::get()->Get(page));
	//}
	return true;
}

bool RFMainWindow::OnEVPage3(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);

	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;
	
	//if (RFEvaluationData::get()->GetElementNumber(page) > 0) {
		RFEvaluationData::get()->setCurPage(page);
		UpdateEvaluationNumber(page);
		UpdateEvaluationPage(RFEvaluationData::get()->Get(page));
	//}
	return true;
}

bool RFMainWindow::OnEVPage4(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);

	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;
	
	//if (RFEvaluationData::get()->GetElementNumber(page) > 0) {
		RFEvaluationData::get()->setCurPage(page);
		UpdateEvaluationNumber(page);
		UpdateEvaluationPage(RFEvaluationData::get()->Get(page));
	//}
	return true;
}

bool RFMainWindow::OnEVNextPage(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	int cur_page = RFEvaluationData::get()->m_current_page;

	int page = cur_page + 1;
	//if (RFEvaluationData::get()->GetElementNumber(page) > 0) {
		RFEvaluationData::get()->setCurPage(page);
		UpdateEvaluationNumber(page);
		UpdateEvaluationPage(RFEvaluationData::get()->Get(page));
	//}
	return true;
}


bool RFMainWindow::OnReturnEvaluationPage(void* pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	ShowEvaluationPage();
	return true;
}

bool RFMainWindow::OnReturnEvaluationHistoryPage(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	if (m_evalution_type == 1) {
		OnEvaluation1(pParam);
	} else if (m_evalution_type == 2){
		OnEvaluation2(pParam);
	} else {
		OnEvaluation3(pParam);
		m_robot.stopActiveMove();
		m_robot.setDamping(0.3);
	}
	
	return true;
}

bool RFMainWindow::OnEVAdd(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	if (m_evalution_type == 1) {
		CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_add_welcom")));
		pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

		pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_add_name")));
		pLabel->SetText(_T("建立FMA评测"));

		m_fma.Reset();
		m_mas.Reset();
		UpdateEVIndex();
		UpdateEVQuestion();
		UpdateEVAnswer();
		ShowEvaluationAddPage();
	} else if (m_evalution_type == 2) {
		CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_add_welcom")));
		pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

		pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_add_name")));
		pLabel->SetText(_T("建立MAS评测"));
		m_fma.Reset();
		m_mas.Reset();
		UpdateEVIndex();
		UpdateEVQuestion();
		UpdateEVAnswer();
		ShowEvaluationAddPage();
	} else if (m_evalution_type == 3) {
		CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_ydgn_add_welcom")));
		pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

		m_fma.Reset();
		m_mas.Reset();
		ShowEvaluationYDGNAddPage();
	}

	return true;
}

bool RFMainWindow::OnEVAddPrev(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	UpdateEVAnswerToData();
	if (m_evalution_type == 1) {
		m_fma.Prev();
	} else if (m_evalution_type == 2) {
		m_mas.Prev();
	}

	UpdateEVIndex();
	UpdateEVQuestion();
	UpdateEVAnswer();
	UpdateEVAnswerToUI();
	return true;
}

bool RFMainWindow::OnEVAddSubmit(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	UpdateEVAnswerToData();
	SaveEVAnserToDB();

	OnReturnEvaluationHistoryPage(pParam);
	return true;
}

bool RFMainWindow::OnEVAddNext(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	UpdateEVAnswerToData();
	if (m_evalution_type == 1) {
		m_fma.Next();
	} else if (m_evalution_type == 2) {
		m_mas.Next();
	}

	UpdateEVIndex();
	UpdateEVQuestion();
	UpdateEVAnswer();
	UpdateEVAnswerToUI();
	return true;
}

void RFMainWindow::ShowEVDetail(std::wstring evid)
{
	if (m_evalution_type == 1 || m_evalution_type == 2) {
		int id = _wtoi(evid.c_str());

		CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_detail_welcom")));
		pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

		RFEvaluationDetailData::get()->Load(id);

		ShowEvaluationDetailPage();

		RFEvaluationDetailData::get()->setCurPage(0);
		UpdateEvaluationDetailNumber(0);
		UpdateEvaluationDetailPage(RFEvaluationDetailData::get()->Get(0));
		UpdateEvaluationDetailScore(RFEvaluationDetailData::get()->m_score);
	} else {
		int id = _wtoi(evid.c_str());

		CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_ydgn_detail_welcom")));
		pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

		RFEvaluationDetailData::get()->Load(id);
		ShowEvaluationYDGNDetailPage();

		int i = 0;
		std::list<EvaluationRecordData>::iterator begin = RFEvaluationDetailData::get()->m_datas.begin();
		for (; begin != RFEvaluationDetailData::get()->m_datas.end(); begin++) {
			wchar_t name[128] = _T("");
			wsprintf(name, _T("evd_ydgn_cell%d1"), i);
			CTextUI* pTxt = static_cast<CTextUI*>(m_pm.FindControl(name));
			if (pTxt) {
				pTxt->SetText(begin->item.c_str());
			}
			
			wsprintf(name, _T("evd_ydgn_cell%d2"), i);
			pTxt = static_cast<CTextUI*>(m_pm.FindControl(name));
			if (pTxt) {
				pTxt->SetText(begin->result.c_str());
			}

			i++;
		}

		wchar_t score[128];

		ZeroMemory(score,sizeof(TCHAR) * 20);
		_stprintf_s(score,128,_T("综合得分：%.2f°"), m_evydgn.score);

		pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_ydgn_detail_score")));
		pLabel->SetText(score);
	}
}


bool RFMainWindow::OnEVDLastPage(void* pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);
	int cur_page = RFEvaluationDetailData::get()->m_current_page;
	if (cur_page < 1) {
		return true;
	}

	int page = cur_page - 1;
	RFEvaluationDetailData::get()->setCurPage(page);
	UpdateEvaluationDetailNumber(page);
	UpdateEvaluationDetailPage(RFEvaluationDetailData::get()->Get(page));
	return true;
}

bool RFMainWindow::OnEVDPage1(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);

	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;

	//if (RFEvaluationData::get()->GetElementNumber(page) > 0) {
	RFEvaluationDetailData::get()->setCurPage(page);
	UpdateEvaluationDetailNumber(page);
	UpdateEvaluationDetailPage(RFEvaluationDetailData::get()->Get(page));
	//}
	return true;
}

bool RFMainWindow::OnEVDPage2(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);

	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;

	//if (RFEvaluationData::get()->GetElementNumber(page) > 0) {
	RFEvaluationDetailData::get()->setCurPage(page);
	UpdateEvaluationDetailNumber(page);
	UpdateEvaluationDetailPage(RFEvaluationDetailData::get()->Get(page));
	//}
	return true;
}

bool RFMainWindow::OnEVDPage3(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);

	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;

	//if (RFEvaluationData::get()->GetElementNumber(page) > 0) {
	RFEvaluationDetailData::get()->setCurPage(page);
	UpdateEvaluationDetailNumber(page);
	UpdateEvaluationDetailPage(RFEvaluationDetailData::get()->Get(page));
	//}
	return true;
}

bool RFMainWindow::OnEVDPage4(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	CButtonUI* pControl = static_cast<CButtonUI*>(pMsg->pSender);

	int page = _wtoi(((std::wstring)pControl->GetText()).c_str()) - 1;

	//if (RFEvaluationData::get()->GetElementNumber(page) > 0) {
	RFEvaluationDetailData::get()->setCurPage(page);
	UpdateEvaluationDetailNumber(page);
	UpdateEvaluationDetailPage(RFEvaluationDetailData::get()->Get(page));
	//}
	return true;
}

bool RFMainWindow::OnEVDNextPage(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	int cur_page = RFEvaluationDetailData::get()->m_current_page;

	int page = cur_page + 1;
	//if (RFEvaluationData::get()->GetElementNumber(page) > 0) {
	RFEvaluationDetailData::get()->setCurPage(page);
	UpdateEvaluationDetailNumber(page);
	UpdateEvaluationDetailPage(RFEvaluationDetailData::get()->Get(page));
	//}
	return true;
}

bool RFMainWindow::OnEVStart(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	m_robot.stopActiveMove();
	m_robot.startActiveMove();

	StartEVDetect();
	return true;
}

bool RFMainWindow::OnEVStop(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	StopEVDetect();
	m_robot.stopActiveMove();

	wchar_t temp[128];
	ZeroMemory(temp,sizeof(TCHAR) * 20);
	_stprintf_s(temp,128,_T("得分：%.2f"), m_evydgn.score);

	CLabelUI* plbl = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_ydgn_score")));
	plbl->SetText(temp);
	return true;
}

bool RFMainWindow::OnEVBegin1(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	CCheckBoxUI* pCb = static_cast<CCheckBoxUI*>(pMsg->pSender);
	if (!pCb) {return true;}
	if (!pCb->GetCheck()) {
		double angle[2];
		m_robot.getAngle(angle);
		m_evydgn.zb1 = angle[0];

		TCHAR szStartAngle[20];
		ZeroMemory(szStartAngle,sizeof(TCHAR) * 20);
		_stprintf_s(szStartAngle,20,_T("%.2f°"),angle[0]);
		
		CLabelUI * plbl = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_angle01")));
		plbl->SetText(szStartAngle);
	} else {
		double angle[2];
		m_robot.getAngle(angle);
		m_evydgn.zb2 = angle[0];
		m_evydgn.zb = m_evydgn.zb2 - m_evydgn.zb1;
		
		TCHAR szStartAngle[20];
		ZeroMemory(szStartAngle,sizeof(TCHAR) * 20);
		_stprintf_s(szStartAngle,20,_T("%.2f°"),angle[0]);

		CLabelUI * plbl = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_angle02")));
		plbl->SetText(szStartAngle);
	}

	return true;
}

bool RFMainWindow::OnEVBegin2(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;


	CCheckBoxUI* pCb = static_cast<CCheckBoxUI*>(pMsg->pSender);
	if (!pCb) {return true;}

	if (!pCb->GetCheck()) {
		double angle[2];
		m_robot.getAngle(angle);
		m_evydgn.jb1 = angle[1];

		TCHAR szStartAngle[20];
		ZeroMemory(szStartAngle,sizeof(TCHAR) * 20);
		_stprintf_s(szStartAngle,20,_T("%.2f°"),angle[1]);

		CLabelUI * plbl = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_angle11")));
		plbl->SetText(szStartAngle);
	} else {
		double angle[2];
		m_robot.getAngle(angle);
		m_evydgn.jb2 = angle[1];
		m_evydgn.jb = m_evydgn.jb2 - m_evydgn.jb1;

		TCHAR szStartAngle[20];
		ZeroMemory(szStartAngle,sizeof(TCHAR) * 20);
		_stprintf_s(szStartAngle,20,_T("%.2f°"),angle[1]);

		CLabelUI * plbl = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_angle12")));
		plbl->SetText(szStartAngle);
	}

	return true;
}


void RFMainWindow::ShowLoginPage()
{
	m_main_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(true);
	m_login_page->SetVisible(true);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);
	m_current_passivetraininfos.clear();

	StopActiveGameDetect();
}

void RFMainWindow::ShowLoginSuccessPage()
{
	m_main_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_login_success_page->SetVisible(true);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(true);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);

	m_current_passivetraininfos.clear();


	StopActiveGameDetect();
}

void RFMainWindow::ShowMainPage()
{
	m_patient_select_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_main_page->SetVisible(true);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);

	StopActiveGameDetect();

	m_current_passivetraininfos.clear();

	if (m_current_patient.id > 0) {
		std::wstring current_patient = _T("已选择患者 【") + m_current_patient.name + _T("】");
		m_main_tip->SetText(current_patient.c_str());
	} else {
		std::wstring current_patient = _T("请先选择一名患者之后，再继续其他操作");
		m_main_tip->SetText(current_patient.c_str());
	}
}

void RFMainWindow::ShowSelectPatientPage()
{
	m_main_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_patient_select_page->SetVisible(true);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);

	StopActiveGameDetect();

	m_current_passivetraininfos.clear();
}

void RFMainWindow::ShowManagerPatientPage()
{
	m_main_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_patient_manager_page->SetVisible(true);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);

	StopActiveGameDetect();

	m_current_passivetraininfos.clear();
}

void RFMainWindow::ShowTrainPage()
{
	m_main_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(true);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);

	StopActiveGameDetect();

	m_current_passivetraininfos.clear();
}

void RFMainWindow::ShowActiveTrainPage()
{
	m_main_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(true);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);

	StopActiveGameDetect();

	m_current_passivetraininfos.clear();
}

void RFMainWindow::ShowPassiveTrainPage()
{
	CListUI* pListUI = static_cast<CListUI*>(m_passive_train_page->FindSubControl(_T("passive_train_content_list")));
	pListUI->RemoveAll();
	m_id_media.clear();
	m_id_passivetrainitem.clear();
	m_delete_id_passivetrainitem.clear();
	std::list<PassiveTrainInfo>::iterator begin = RFPassiveTrain::get()->m_passivetraininfos.begin();
	for (; begin != RFPassiveTrain::get()->m_passivetraininfos.end(); begin++) {
		AddPassiveTrainItem(pListUI, *begin);
	}

	m_main_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(true);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);

	StopActiveGameDetect();

	m_current_passivetraininfos.clear();
}

void RFMainWindow::ShowPatientDetail(int page, int index)
{
	m_main_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_detail_page->SetVisible(true);
	m_patient_manager_page->SetVisible(false);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);

	StopActiveGameDetect();

	m_current_passivetraininfos.clear();

	CLabelUI* pWelecome = static_cast<CLabelUI*>(m_pm.FindControl(_T("manager_patient_detail_welcom")));
	pWelecome->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

	PatientInfo patient = RFPatientsManager::get()->getPatient(page, index);
	
	wchar_t field[64] = _T("");
	CEditUI* pLabel = static_cast<CEditUI*>(m_patient_detail_page->FindSubControl(_T("detail_patient_id")));
	if (pLabel) {
		memset(field, 0, 64);
		wsprintf(field, _T("%d"), patient.id);
		pLabel->SetText(field);
	}

	pLabel = static_cast<CEditUI*>(m_patient_detail_page->FindSubControl(_T("detail_patient_createtime")));
	if (pLabel) {
		pLabel->SetText(patient.createtime.c_str());
	}

	pLabel = static_cast<CEditUI*>(m_patient_detail_page->FindSubControl(_T("detail_patient_name")));
	if (pLabel) {
		pLabel->SetText(patient.name.c_str());
	}

	pLabel = static_cast<CEditUI*>(m_patient_detail_page->FindSubControl(_T("detail_patient_lasttime")));
	if (pLabel) {
		pLabel->SetText(patient.lasttreattime.c_str());
	}

	pLabel = static_cast<CEditUI*>(m_patient_detail_page->FindSubControl(_T("detail_patient_sex")));
	if (pLabel) {
		pLabel->SetText(patient.sex.c_str());
	}

	pLabel = static_cast<CEditUI*>(m_patient_detail_page->FindSubControl(_T("detail_patient_totaltime")));
	if (pLabel) {
		pLabel->SetText(patient.totaltreattime.c_str());
	}

	pLabel = static_cast<CEditUI*>(m_patient_detail_page->FindSubControl(_T("detail_patient_age")));
	if (pLabel) {
		memset(field, 0, 64);
		wsprintf(field, _T("%d"), patient.age);
		pLabel->SetText(field);
	}

	pLabel = static_cast<CEditUI*>(m_patient_detail_page->FindSubControl(_T("detail_patient_detail")));
	if (pLabel) {
		pLabel->SetText(patient.recoverdetail.c_str());
	}

	CRichEditUI* pEdit = static_cast<CRichEditUI*>(m_patient_detail_page->FindSubControl(_T("detail_patient_remark")));
	if (pEdit) {
		pEdit->SetText(patient.remarks.c_str());
	}
}

void RFMainWindow::ShowPatientEdit(int patientid)
{
	m_main_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_edit_page->SetVisible(true);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);

	StopActiveGameDetect();

	m_current_passivetraininfos.clear();

	CLabelUI* pWelecome = static_cast<CLabelUI*>(m_pm.FindControl(_T("manager_patient_bianji_welcom")));
	pWelecome->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

	PatientInfo patient = RFPatientsManager::get()->getPatient(patientid);

	wchar_t field[64] = _T("");
	CEditUI* pLabel = static_cast<CEditUI*>(m_patient_edit_page->FindSubControl(_T("bianji_patient_id")));
	if (pLabel) {
		memset(field, 0, 64);
		wsprintf(field, _T("%d"), patient.id);
		pLabel->SetText(field);
	}

	pLabel = static_cast<CEditUI*>(m_patient_edit_page->FindSubControl(_T("bianji_patient_createtime")));
	if (pLabel) {
		pLabel->SetText(patient.createtime.c_str());
	}

	pLabel = static_cast<CEditUI*>(m_patient_edit_page->FindSubControl(_T("bianji_patient_name")));
	if (pLabel) {
		pLabel->SetText(patient.name.c_str());
	}

	pLabel = static_cast<CEditUI*>(m_patient_edit_page->FindSubControl(_T("bianji_patient_lasttime")));
	if (pLabel) {
		pLabel->SetText(patient.lasttreattime.c_str());
	}

	pLabel = static_cast<CEditUI*>(m_patient_edit_page->FindSubControl(_T("bianji_patient_sex")));
	if (pLabel) {
		pLabel->SetText(patient.sex.c_str());
	}

	pLabel = static_cast<CEditUI*>(m_patient_edit_page->FindSubControl(_T("bianji_patient_totaltime")));
	if (pLabel) {
		pLabel->SetText(patient.totaltreattime.c_str());
	}

	pLabel = static_cast<CEditUI*>(m_patient_edit_page->FindSubControl(_T("bianji_patient_age")));
	if (pLabel) {
		memset(field, 0, 64);
		wsprintf(field, _T("%d"), patient.age);
		pLabel->SetText(field);
	}

	pLabel = static_cast<CEditUI*>(m_patient_edit_page->FindSubControl(_T("bianji_patient_detail")));
	if (pLabel) {
		pLabel->SetText(patient.recoverdetail.c_str());
	}

	CRichEditUI* pEdit = static_cast<CRichEditUI*>(m_patient_edit_page->FindSubControl(_T("bianji_patient_remark")));
	if (pEdit) {
		pEdit->SetText(patient.remarks.c_str());
	}
}

void RFMainWindow::ShowPatientEdit(int page, int index)
{
	m_main_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_edit_page->SetVisible(true);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);

	StopActiveGameDetect();

	m_current_passivetraininfos.clear();

	CLabelUI* pWelecome = static_cast<CLabelUI*>(m_pm.FindControl(_T("manager_patient_bianji_welcom")));
	pWelecome->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

	PatientInfo patient = RFPatientsManager::get()->getPatient(page, index);

	wchar_t field[64] = _T("");
	CEditUI* pLabel = static_cast<CEditUI*>(m_patient_edit_page->FindSubControl(_T("bianji_patient_id")));
	if (pLabel) {
		memset(field, 0, 64);
		wsprintf(field, _T("%d"), patient.id);
		pLabel->SetText(field);
	}

	pLabel = static_cast<CEditUI*>(m_patient_edit_page->FindSubControl(_T("bianji_patient_createtime")));
	if (pLabel) {
		pLabel->SetText(patient.createtime.c_str());
	}

	pLabel = static_cast<CEditUI*>(m_patient_edit_page->FindSubControl(_T("bianji_patient_name")));
	if (pLabel) {
		pLabel->SetText(patient.name.c_str());
	}

	pLabel = static_cast<CEditUI*>(m_patient_edit_page->FindSubControl(_T("bianji_patient_lasttime")));
	if (pLabel) {
		pLabel->SetText(patient.lasttreattime.c_str());
	}

	pLabel = static_cast<CEditUI*>(m_patient_edit_page->FindSubControl(_T("bianji_patient_sex")));
	if (pLabel) {
		pLabel->SetText(patient.sex.c_str());
	}

	pLabel = static_cast<CEditUI*>(m_patient_edit_page->FindSubControl(_T("bianji_patient_totaltime")));
	if (pLabel) {
		pLabel->SetText(patient.totaltreattime.c_str());
	}

	pLabel = static_cast<CEditUI*>(m_patient_edit_page->FindSubControl(_T("bianji_patient_age")));
	if (pLabel) {
		memset(field, 0, 64);
		wsprintf(field, _T("%d"), patient.age);
		pLabel->SetText(field);
	}

	pLabel = static_cast<CEditUI*>(m_patient_edit_page->FindSubControl(_T("bianji_patient_detail")));
	if (pLabel) {
		pLabel->SetText(patient.recoverdetail.c_str());
	}

	CRichEditUI* pEdit = static_cast<CRichEditUI*>(m_patient_edit_page->FindSubControl(_T("bianji_patient_remark")));
	if (pEdit) {
		pEdit->SetText(patient.remarks.c_str());
	}
}


void RFMainWindow::ShowPatientAdd(std::wstring patientid)
{
	m_main_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_edit_page->SetVisible(false);
	m_patient_add_page->SetVisible(true);
	m_about_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);

	StopActiveGameDetect();

	m_current_passivetraininfos.clear();

	CLabelUI* pWelecome = static_cast<CLabelUI*>(m_pm.FindControl(_T("manager_patient_add_welcom")));
	pWelecome->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

	wchar_t field[64] = _T("");
	CEditUI* pLabel = static_cast<CEditUI*>(m_patient_add_page->FindSubControl(_T("add_patient_id")));
	if (pLabel) {

		pLabel->SetText(patientid.c_str());
	}

	pLabel = static_cast<CEditUI*>(m_patient_add_page->FindSubControl(_T("add_patient_name")));
	if (pLabel) {
		pLabel->SetText(_T(""));
	}

	pLabel = static_cast<CEditUI*>(m_patient_add_page->FindSubControl(_T("add_patient_createtime")));
	if (pLabel) {
		pLabel->SetText(RFToDateString(time(NULL)).c_str());
	}

	pLabel = static_cast<CEditUI*>(m_patient_add_page->FindSubControl(_T("add_patient_totaltime")));
	if (pLabel) {
		pLabel->SetText(_T(""));
	}

	pLabel = static_cast<CEditUI*>(m_patient_add_page->FindSubControl(_T("add_patient_age")));
	if (pLabel) {
		pLabel->SetText(_T(""));
	}

	pLabel = static_cast<CEditUI*>(m_patient_add_page->FindSubControl(_T("add_patient_detail")));
	if (pLabel) {
		pLabel->SetText(_T(""));
	}

	CRichEditUI* pEdit = static_cast<CRichEditUI*>(m_patient_add_page->FindSubControl(_T("add_patient_remark")));
	if (pEdit) {
		pEdit->SetText(_T(""));
	}
}

void RFMainWindow::ShowAboutPage()
{
	m_main_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(true);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);

	StopActiveGameDetect();

	m_current_passivetraininfos.clear();

	CLabelUI* pWelecome = static_cast<CLabelUI*>(m_pm.FindControl(_T("about_welcom")));
	pWelecome->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

}

void RFMainWindow::ShowPersonorPage()
{
	m_main_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(true);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);

	StopActiveGameDetect();

	m_current_passivetraininfos.clear();

	CLabelUI* pWelecome = static_cast<CLabelUI*>(m_pm.FindControl(_T("personor_welcom")));
	pWelecome->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

	UpdatePersonInfo();
}

void RFMainWindow::ShowPatientTrainInformation()
{
	m_main_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(true);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);

	StopActiveGameDetect();

	m_current_passivetraininfos.clear();
}

void RFMainWindow::ShowPatientTrainDetail(std::wstring patientid)
{
	m_current_patient_id_detail = patientid;

	m_main_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(true);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);

	StopActiveGameDetect();

	m_current_passivetraininfos.clear();

	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("patient_traindetail_welcom")));
	pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

	RFPatientsTrainDetails::get()->SetCurrentPatientID(_wtoi(patientid.c_str()));

	UpdateTrainDetailPage(RFPatientsTrainInfo::get()->getTrainInfo(patientid));
	UpdateTrainDetailPage(RFPatientsTrainDetails::get()->getPageElement(RFPatientsTrainDetails::get()->m_currentpage));
	UpdateTrainDetailPageNumber(RFPatientsTrainDetails::get()->m_currentpage);
}

void RFMainWindow::ShowEyeModeTrainPage()
{
	m_main_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(true);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);

	RFDialog::m_eyemode_close = true;
	StopActiveGameDetect();

	m_current_passivetraininfos.clear();


	//CAVPlayerUI* pAVPlayer = static_cast<CAVPlayerUI*>(m_pm.FindControl(_T("vlc_left_camera")));
	//if (pAVPlayer) {
	//	std::list<std::wstring> cameras = pAVPlayer->EnumCameras();
	//	if (cameras.size() > 0) {
	//		pAVPlayer->Play(0);
	//	}
	//}

	//pAVPlayer = static_cast<CAVPlayerUI*>(m_pm.FindControl(_T("vlc_right_camera")));
	//if (pAVPlayer) {
	//	std::list<std::wstring> cameras = pAVPlayer->EnumCameras();
	//	if (cameras.size() > 1) {
	//		pAVPlayer->Play(1);
	//	}
	//}
}

void RFMainWindow::ShowEmgModeTrainPage()
{
	m_main_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(true);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);

	StopActiveGameDetect();

	m_current_passivetraininfos.clear();
}

void RFMainWindow::ShowTrainDataChartPage(int id)
{
	m_main_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(true);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);

	StopActiveGameDetect();

	m_current_passivetraininfos.clear();

	PatientTrainDetails trainDetails = RFPatientsTrainDetails::get()->getTrainDetail(id);
	UpdateZDChart(trainDetails);
	UpdateBDChart(trainDetails);
	UpdateYDChart(trainDetails);
	UpdateEMGChart(trainDetails);
}

void RFMainWindow::ShowEvaluationHistoryPage()
{
	m_main_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(true);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);
}

void RFMainWindow::ShowEvaluationDetailPage()
{
	m_main_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(true);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);
}

void RFMainWindow::ShowEvaluationYDGNDetailPage()
{
	m_main_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(true);
}


void RFMainWindow::ShowEvaluationYDGNAddPage()
{
	m_main_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(true);
	m_evaluation_ydgn_detail_page->SetVisible(false);
}

void RFMainWindow::ShowEvaluationAddPage()
{
	m_main_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(true);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);
}

void RFMainWindow::ShowEvaluationPage()
{
	m_main_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(false);
	m_evaluation_page->SetVisible(true);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_add_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);
}

void RFMainWindow::ShowGame4()
{
	m_main_page->SetVisible(false);
	m_patient_select_page->SetVisible(false);
	m_patient_manager_page->SetVisible(false);
	m_patient_detail_page->SetVisible(false);
	m_login_success_page->SetVisible(false);
	m_login_input_page->SetVisible(false);
	m_login_page->SetVisible(false);
	m_patient_edit_page->SetVisible(false);
	m_about_page->SetVisible(false);
	m_patient_add_page->SetVisible(false);
	m_personor_info_edit_page->SetVisible(false);
	m_train_main_page->SetVisible(false);
	m_active_train_page->SetVisible(false);
	m_patient_train_info->SetVisible(false);
	m_patient_train_detail->SetVisible(false);
	m_passive_train_page->SetVisible(false);
	m_eyemode_page->SetVisible(false);
	m_emgmode_page->SetVisible(false);
	m_traindetail_chart->SetVisible(false);
	m_active_train_game4_page->SetVisible(true);
	m_evaluation_page->SetVisible(false);
	m_evaluation_history_page->SetVisible(false);
	m_evaluation_detail_page->SetVisible(false);
	m_evaluation_ydgn_add_page->SetVisible(false);
	m_evaluation_ydgn_detail_page->SetVisible(false);

	StartActiveGameDetect();
}

void RFMainWindow::DeletePatient(int patientid)
{
	if (patientid > 0) {
		RFPatientsManager::get()->remove(patientid);
	}

	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("manager_patient_welcom")));
	pLabel->SetText((_T("欢迎您，") + m_login_info.login_user + _T("!∨")).c_str());

	UpdateManagePatientPage(RFPatientsManager::get()->getPage(RFPatientsManager::get()->m_current_page));
	UpdateManagePageNumber(RFPatientsManager::get()->m_current_page);
	ShowManagerPatientPage();
}


void RFMainWindow::UpdatePageNumber(int page)
{
	wchar_t pageid[16] = _T("");
	
	for(int i = 0; i < 4; i++) {
		wchar_t pagevalue[16] = _T("");

		wsprintf(pageid, _T("page%d"), i+1);
		CButtonUI* pButton = static_cast<CButtonUI*>(m_pm.FindControl(pageid));
		
		if (page % 4 == i) {
			wsprintf(pagevalue, _T("%d"), page + 1);

			pButton->SetBkColor(0xFF184199);
			pButton->SetText(pagevalue);
			pButton->SetTextColor(0xffffffff);
		} else {
			wsprintf(pagevalue, _T("%d"), (page/4)*4 + i + 1);

			pButton->SetTextColor(0xff000000);
			pButton->SetBkColor(0xFFFFFFFF);
			pButton->SetText(pagevalue);
		}
	}
}

void RFMainWindow::UpdateManagePageNumber(int page)
{
	wchar_t pageid[16] = _T("");

	for(int i = 0; i < 4; i++) {
		wchar_t pagevalue[16] = _T("");

		wsprintf(pageid, _T("manage_page%d"), i+1);
		CButtonUI* pButton = static_cast<CButtonUI*>(m_pm.FindControl(pageid));

		if (page % 4 == i) {
			wsprintf(pagevalue, _T("%d"), page + 1);

			pButton->SetTextColor(0xffffffff);
			pButton->SetBkColor(0xFF184199);
			pButton->SetText(pagevalue);
		} else {
			wsprintf(pagevalue, _T("%d"), (page/4)*4 + i + 1);

			pButton->SetTextColor(0xff000000);
			pButton->SetBkColor(0xFFFFFFFF);
			pButton->SetText(pagevalue);
		}
	}
}

void RFMainWindow::UpdatePatientPage(std::list<PatientInfo>& patients)
{
	std::list<PatientInfo>::iterator iter = patients.begin();
	for (int i = 1; i < 9; i++)
	{
		wchar_t rowid[16] = _T("");
		wsprintf(rowid, _T("row0%d"), i);

		CHorizontalLayoutUI *pRow = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(rowid));
	
		for (int j = 0; j < pRow->GetCount(); j++) {
			pRow->GetItemAt(j)->SetVisible(true);
		}

		if (iter != patients.end()) {
			//pRow->SetVisible(true);
			wchar_t cellvalue[16] = _T("");
			
			wchar_t cellid[16] = _T("");
			wsprintf(cellid, _T("cell%d1"), i-1);
			CLabelUI* pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			wsprintf(cellvalue, _T("%d"), iter->id);
			pLabel->SetText(cellvalue);

			memset(cellid, 0, 16);
			wsprintf(cellid, _T("cell%d2"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			pLabel->SetText(iter->name.c_str());

			memset(cellid, 0, 16);
			wsprintf(cellid, _T("cell%d3"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			pLabel->SetText(iter->sex.c_str());

			memset(cellid, 0, 16);
			wsprintf(cellid, _T("cell%d4"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			wsprintf(cellvalue, _T("%d"), iter->age);
			pLabel->SetText(cellvalue);

			memset(cellid, 0, 16);
			wsprintf(cellid, _T("cell%d5"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			pLabel->SetText(iter->createtime.c_str());

			memset(cellid, 0, 16);
			wsprintf(cellid, _T("cell%d6"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			pLabel->SetText(iter->lasttreattime.c_str());

			memset(cellid, 0, 16);
			wsprintf(cellid, _T("cell%d7"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			pLabel->SetText(iter->totaltreattime.c_str());

			memset(cellid, 0, 16);
			wsprintf(cellid, _T("cell%d8"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			pLabel->SetText(iter->recoverdetail.c_str());

			memset(cellid, 0, 16);
			wsprintf(cellid, _T("cell%d9"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			pLabel->SetText(iter->remarks.c_str());

			iter++;
		} else {
			for (int k = 0; k < pRow->GetCount(); k++) {
				pRow->GetItemAt(k)->SetVisible(false);
			}
		}
	}
}


void RFMainWindow::UpdateManagePatientPage(std::list<PatientInfo>& patients)
{
	std::list<PatientInfo>::iterator iter = patients.begin();
	for (int i = 1; i < 9; i++)
	{
		wchar_t rowid[16] = _T("");
		wsprintf(rowid, _T("manage_row0%d"), i);

		CHorizontalLayoutUI *pRow = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(rowid));

		for (int j = 0; j < pRow->GetCount(); j++) {
			pRow->GetItemAt(j)->SetVisible(true);
		}

		if (iter != patients.end()) {
			//pRow->SetVisible(true);
			wchar_t cellvalue[16] = _T("");

			wchar_t cellid[16] = _T("");
			wsprintf(cellid, _T("manage_cell%d1"), i-1);
			CLabelUI* pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			wsprintf(cellvalue, _T("%d"), iter->id);
			pLabel->SetText(cellvalue);

			memset(cellid, 0, 16);
			wsprintf(cellid, _T("manage_cell%d2"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			pLabel->SetText(iter->name.c_str());

			memset(cellid, 0, 16);
			wsprintf(cellid, _T("manage_cell%d3"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			pLabel->SetText(iter->sex.c_str());

			memset(cellid, 0, 16);
			wsprintf(cellid, _T("manage_cell%d4"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			wsprintf(cellvalue, _T("%d"), iter->age);
			pLabel->SetText(cellvalue);

			memset(cellid, 0, 16);
			wsprintf(cellid, _T("manage_cell%d5"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			pLabel->SetText(iter->createtime.c_str());

			iter++;
		} else {
			for (int k = 0; k < pRow->GetCount(); k++) {
				pRow->GetItemAt(k)->SetVisible(false);
			}
		}
	}
}

void RFMainWindow::UpdatePersonInfo()
{

	wchar_t intString[64] = _T(""); 
	CEditUI *pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_id")));
	if (pEdit) {
		pEdit->SetText(m_login_info.employeenumber.c_str());
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_education")));
	if (pEdit) {
		pEdit->SetText(m_login_info.education.c_str());
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_name")));
	if (pEdit) {
		pEdit->SetText(m_login_info.name.c_str());
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_nation")));
	if (pEdit) {
		pEdit->SetText(m_login_info.nation.c_str());
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_sex")));
	if (pEdit) {
		pEdit->SetText(m_login_info.sex.c_str());
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_originplace")));
	if (pEdit) {
		pEdit->SetText(m_login_info.birthplace.c_str());
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_birthday")));
	if (pEdit) {
		pEdit->SetText(m_login_info.birthday.c_str());
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_householdprop")));
	if (pEdit) {
		pEdit->SetText(m_login_info.householdprop.c_str());
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_certid")));
	if (pEdit) {
		pEdit->SetText(m_login_info.certid.c_str());
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_department")));
	if (pEdit) {
		pEdit->SetText(m_login_info.department.c_str());
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_tel")));
	if (pEdit) {
		pEdit->SetText(m_login_info.telephone.c_str());
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_keshi")));
	if (pEdit) {
		pEdit->SetText(m_login_info.group.c_str());
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_bloodtype")));
	if (pEdit) {
		if (m_login_info.bloodtype == BLOOD_A) {
			pEdit->SetText(BLOOD_A_STRING);
		} 
		switch(m_login_info.bloodtype) {
			case BLOOD_A:
				pEdit->SetText(BLOOD_A_STRING);
				break;
			case BLOOD_B:
				pEdit->SetText(BLOOD_B_STRING);
				break;
			case BLOOD_AB:
				pEdit->SetText(BLOOD_AB_STRING);
				break;
			case BLOOD_O:
				pEdit->SetText(BLOOD_O_STRING);
				break;
			default:
				break;
		}
	}


	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_position")));
	if (pEdit) {
		pEdit->SetText(m_login_info.positon.c_str());
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_address")));
	if (pEdit) {
		pEdit->SetText(m_login_info.address.c_str());
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_entrydate")));
	if (pEdit) {
		pEdit->SetText(m_login_info.entrydate.c_str());
	}
}

LoginInfo RFMainWindow::GetPersonInfo()
{
	LoginInfo doctor = m_login_info;

	std::wstring txt = _T("");
	wchar_t intString[64] = _T(""); 
	CEditUI *pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_id")));
	if (pEdit) {
		txt = pEdit->GetText();
		doctor.employeenumber = txt;
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_education")));
	if (pEdit) {
		txt = pEdit->GetText();
		doctor.education = txt;
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_name")));
	if (pEdit) {
		txt = pEdit->GetText();
		doctor.name = txt;
		doctor.login_user = txt;
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_nation")));
	if (pEdit) {
		txt = pEdit->GetText();
		doctor.nation = txt;
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_sex")));
	if (pEdit) {
		txt = pEdit->GetText();
		doctor.sex = txt;
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_originplace")));
	if (pEdit) {
		txt = pEdit->GetText();
		doctor.birthplace = txt;
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_birthday")));
	if (pEdit) {
		txt = pEdit->GetText();
		doctor.birthday = txt;
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_householdprop")));
	if (pEdit) {
		txt = pEdit->GetText();
		doctor.householdprop = txt;
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_certid")));
	if (pEdit) {
		txt = pEdit->GetText();
		doctor.certid = txt;
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_department")));
	if (pEdit) {
		txt = pEdit->GetText();
		doctor.department = txt;
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_tel")));
	if (pEdit) {
		txt = pEdit->GetText();
		doctor.telephone = txt;
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_keshi")));
	if (pEdit) {
		txt = pEdit->GetText();
		doctor.group = txt;
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_bloodtype")));
	if (pEdit) {
		txt = pEdit->GetText();
		
		if (txt == BLOOD_A_STRING) {
			doctor.bloodtype = BLOOD_A;
		} else if (txt == BLOOD_B_STRING) {
			doctor.bloodtype = BLOOD_B;
		} else if (txt == BLOOD_AB_STRING) {
			doctor.bloodtype = BLOOD_AB;
		} else if (txt == BLOOD_O_STRING) {
			doctor.bloodtype = BLOOD_O;
		} 
	}


	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_position")));
	if (pEdit) {
		txt = pEdit->GetText();
		doctor.positon = txt;
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_address")));
	if (pEdit) {
		txt = pEdit->GetText();
		doctor.address = txt;
	}

	pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("personor_entrydate")));
	if (pEdit) {
		txt = pEdit->GetText();
		doctor.entrydate = txt;
	}

	return doctor;
}

void RFMainWindow::UpdateEvaluationScore(std::wstring score)
{
	std::wstring text = _T("最近得分：") + score;
	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_history_score")));
	pLabel->SetText(text.c_str());
}

void RFMainWindow::UpdateEvaluationDetailScore(std::wstring score)
{
	std::wstring text = _T("综合得分：") + score;
	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_detail_score")));
	pLabel->SetText(text.c_str());
}

void RFMainWindow::UpdateEvaluationNumber(int page)
{
	wchar_t pageid[16] = _T("");

	for(int i = 0; i < 4; i++) {
		wchar_t pagevalue[16] = _T("");

		wsprintf(pageid, _T("ev_page%d"), i+1);
		CButtonUI* pButton = static_cast<CButtonUI*>(m_pm.FindControl(pageid));

		if (page % 4 == i) {
			wsprintf(pagevalue, _T("%d"), page + 1);

			pButton->SetTextColor(0xffffffff);
			pButton->SetBkColor(0xFF184199);
			pButton->SetText(pagevalue);
		} else {
			wsprintf(pagevalue, _T("%d"), (page/4)*4 + i + 1);

			pButton->SetTextColor(0xff000000);
			pButton->SetBkColor(0xFFFFFFFF);
			pButton->SetText(pagevalue);
		}
	}
}

void RFMainWindow::UpdateEvaluationDetailNumber(int page)
{
	wchar_t pageid[16] = _T("");

	for(int i = 0; i < 4; i++) {
		wchar_t pagevalue[16] = _T("");

		wsprintf(pageid, _T("evd_page%d"), i+1);
		CButtonUI* pButton = static_cast<CButtonUI*>(m_pm.FindControl(pageid));

		if (page % 4 == i) {
			wsprintf(pagevalue, _T("%d"), page + 1);

			pButton->SetTextColor(0xffffffff);
			pButton->SetBkColor(0xFF184199);
			pButton->SetText(pagevalue);
		} else {
			wsprintf(pagevalue, _T("%d"), (page/4)*4 + i + 1);

			pButton->SetTextColor(0xff000000);
			pButton->SetBkColor(0xFFFFFFFF);
			pButton->SetText(pagevalue);
		}
	}
}

void RFMainWindow::UpdateEvaluationDetailPage(std::list<EvaluationRecordData> datas)
{
	std::list<EvaluationRecordData>::iterator iter = datas.begin();
	for (int i = 1; i < 5; i++)
	{
		wchar_t rowid[16] = _T("");
		wsprintf(rowid, _T("evd_row0%d"), i);

		CHorizontalLayoutUI *pRow = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(rowid));

		for (int j = 0; j < pRow->GetCount(); j++) {
			pRow->GetItemAt(j)->SetVisible(true);
		}

		if (iter != datas.end()) {
			wchar_t cellvalue[16] = _T("");

			wchar_t cellid[16] = _T("");
			wsprintf(cellid, _T("evd_cell%d1"), i-1);
			CLabelUI* pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			pLabel->SetText(iter->item.c_str());

			memset(cellid, 0, 16);
			wsprintf(cellid, _T("evd_cell%d2"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			pLabel->SetText(iter->result.c_str());

			iter++;
		} else {
			for (int k = 0; k < pRow->GetCount(); k++) {
				pRow->GetItemAt(k)->SetVisible(false);
			}
		}
	}
}

void RFMainWindow::UpdateEvaluationPage(std::list<EvaluationData> datas)
{
	std::list<EvaluationData>::iterator iter = datas.begin();
	for (int i = 1; i < 6; i++)
	{
		wchar_t rowid[16] = _T("");
		wsprintf(rowid, _T("eval_row0%d"), i);

		CHorizontalLayoutUI *pRow = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(rowid));

		for (int j = 0; j < pRow->GetCount(); j++) {
			pRow->GetItemAt(j)->SetVisible(true);
		}

		if (iter != datas.end()) {
			wchar_t cellvalue[16] = _T("");

			wchar_t cellid[16] = _T("");
			wsprintf(cellid, _T("eval_cell%d1"), i-1);
			CLabelUI* pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			wsprintf(cellvalue, _T("%d"), iter->id);
			pLabel->SetText(cellvalue);

			memset(cellid, 0, 16);
			wsprintf(cellid, _T("eval_cell%d2"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			pLabel->SetText(iter->date.c_str());

			memset(cellid, 0, 16);
			wsprintf(cellid, _T("eval_cell%d3"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			pLabel->SetText(iter->name.c_str());

			memset(cellid, 0, 16);
			wsprintf(cellid, _T("eval_cell%d4"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			pLabel->SetText(iter->score.c_str());

			iter++;
		} else {
			for (int k = 0; k < pRow->GetCount(); k++) {
				pRow->GetItemAt(k)->SetVisible(false);
			}
		}
	}
}

void RFMainWindow::SaveEVAnserToDB()
{
	wchar_t score[64] = _T("");
	std::vector<std::wstring> questions;
	std::vector<std::wstring> answers;
	if (m_evalution_type == 1) {
		wsprintf(score, _T("%d"), m_fma.getScore());
		m_fma.getQuestionAndAnswer(questions, answers);
	} else {
		wsprintf(score, _T("%d"), m_mas.getScore());
		m_mas.getQuestionAndAnswer(questions, answers);
	}
	if (questions.size() != answers.size() || questions.size() == 0) {
		return;
	}

	EvaluationData data;

	DWORD createTime = time(NULL);
	data.doctorid = m_login_info.doctorid;
	data.name = m_current_patient.name;
	data.patientid = m_current_patient.id;
	data.score = score;
	data.type = m_evalution_type;
	data.date = RFToTimeString(createTime);
	data.id = RFEvaluationData::get()->m_nextid;
	

	for (int i = 0; i < questions.size(); i++) {
		EvaluationRecordData record;

		record.item = questions.at(i);
		record.result = answers.at(i);
		record.evalid = data.id;
		data.datas.push_back(record);
	}


	RFEvaluationData::get()->Add(data);
}

bool RFMainWindow::UpdateEVAnswerToData()
{
	bool someSelected = false;

	for (int i = 1; i < 7; i++)
	{
		wchar_t name[128] = _T("");
		wsprintf(name, _T("question%d"), i);
		COptionUI* pOption = static_cast<COptionUI*>(m_pm.FindControl(name));
		if (pOption->IsSelected()) {
			
			if (m_evalution_type == 1) {
				m_fma.saveAnswer(m_fma.m_current_index, (i-1));
			} else {
				m_mas.saveAnswer(m_mas.m_current_index, (i-1));
			}
			someSelected = true;
			break;
		}
	}

	return someSelected;
}

void RFMainWindow::UpdateEVAnswerToUI()
{
	int cur = 1;
	int answer = 1;
	if (m_evalution_type == 1) {
		cur = m_fma.m_current_index;
		if (m_fma.m_result.find(cur) == m_fma.m_result.end()) {
			return;
		}
		answer = m_fma.m_result[cur] + 1;
	} else {
		cur = m_mas.m_current_index;
		if (m_mas.m_result.find(cur) == m_mas.m_result.end()) {
			return;
		}
		answer = m_mas.m_result[cur] + 1;
	}

	wchar_t name[128] = _T("");
	wsprintf(name, _T("question%d"), answer);
	COptionUI* pOption = static_cast<COptionUI*>(m_pm.FindControl(name));
	if (pOption) {
		pOption->Selected(true);
	}
}

void RFMainWindow::UpdateEVIndex()
{
	int cur = 0;
	int total = 0;

	if (m_evalution_type == 1) {
		cur = m_fma.m_current_index+1;
		total = m_fma.m_questions.size();
	} else {
		cur = m_mas.m_current_index + 1;
		total = m_mas.m_questions.size();
	}

	wchar_t index[128];
	wsprintf(index, _T("%d/%d"), cur, total);
	CLabelUI* pLbl = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_question_index")));
	pLbl->SetText(index);

	if (cur < total) {
		CHorizontalLayoutUI* pHori = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("evaluation_continue")));
		pHori->SetVisible(true);


		pHori = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("evaluation_submit")));
		pHori->SetVisible(false);
	} else {
		CHorizontalLayoutUI* pHori = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("evaluation_continue")));
		pHori->SetVisible(false);


		pHori = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("evaluation_submit")));
		pHori->SetVisible(true);
	}
}

void RFMainWindow::UpdateEVQuestion()
{
	std::wstring question;
	if (m_evalution_type == 1) {
		question = m_fma.getQuestion(m_fma.m_current_index);
	} else if (m_evalution_type == 2) {
		question = m_mas.getQuestion(m_mas.m_current_index);
	}

	CLabelUI* pLbl = static_cast<CLabelUI*>(m_pm.FindControl(_T("evaluation_question")));
	pLbl->SetText(question.c_str());
}

void RFMainWindow::UpdateEVAnswer()
{
	std::vector<std::wstring> answers;
	if (m_evalution_type == 1) {
		answers = m_fma.getAnswers(m_fma.m_current_index);
	} else {
		answers = m_mas.getAnswers(m_mas.m_current_index);
	}

	for (int i = 0; i < 6; i++) {
		wchar_t name[128] = _T("");
		wsprintf(name, _T("answer%d"), i+1);
		
		CHorizontalLayoutUI* pHori = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(name));
		pHori->SetVisible(false);
		if (i < answers.size()) {
			wsprintf(name, _T("evaluation_text%d"), i+1);
			CLabelUI* pLbl = static_cast<CLabelUI*>(m_pm.FindControl(name));
			pLbl->SetText(answers.at(i).c_str());

			wsprintf(name, _T("question%d"), i+1);
			COptionUI* pOption = static_cast<COptionUI*>(m_pm.FindControl(name));
			if (answers.at(i) == _T("/")) {
				pOption->SetNormalImage(_T("radio_3.png"));
				pOption->SetHotImage(_T("radio_3.png"));
				pOption->SetHotForeImage(_T("radio_3.png"));
				pOption->SetPushedImage(_T("radio_3.png"));
				pOption->SetSelectedImage(_T("radio_3.png"));
				pOption->SetSelectedForedImage(_T("radio_3.png"));
				pOption->Selected(false);
			} else {
				pOption->SetNormalImage(_T("radio_0.png"));
				pOption->SetHotImage(_T("radio_0.png"));
				pOption->SetHotForeImage(_T("radio_0.png"));
				pOption->SetPushedImage(_T("radio_0.png"));
				pOption->SetSelectedImage(_T("radio_1.png"));
				pOption->SetSelectedForedImage(_T("radio_1.png"));
				pOption->Selected(false);
			}

			pHori->SetVisible(true);
		} 
	}
}

void RFMainWindow::UpdateTrainInfoPageNumber(int page)
{
	wchar_t pageid[16] = _T("");

	for(int i = 0; i < 4; i++) {
		wchar_t pagevalue[16] = _T("");

		wsprintf(pageid, _T("pf_page%d"), i+1);
		CButtonUI* pButton = static_cast<CButtonUI*>(m_pm.FindControl(pageid));

		if (page % 4 == i) {
			wsprintf(pagevalue, _T("%d"), page + 1);

			pButton->SetTextColor(0xffffffff);
			pButton->SetBkColor(0xFF184199);
			pButton->SetText(pagevalue);
		} else {
			wsprintf(pagevalue, _T("%d"), (page/4)*4 + i + 1);

			pButton->SetTextColor(0xff000000);
			pButton->SetBkColor(0xFFFFFFFF);
			pButton->SetText(pagevalue);
		}
	}
}

void RFMainWindow::UpdateTrainInfoPage(std::list<PatientTrainInfo> trains)
{
	std::list<PatientTrainInfo>::iterator iter = trains.begin();
	for (int i = 1; i < 9; i++)
	{
		wchar_t rowid[16] = _T("");
		wsprintf(rowid, _T("pf_row0%d"), i);

		CHorizontalLayoutUI *pRow = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(rowid));

		for (int j = 0; j < pRow->GetCount(); j++) {
			pRow->GetItemAt(j)->SetVisible(true);
		}

		if (iter != trains.end()) {
			//pRow->SetVisible(true);
			wchar_t cellvalue[16] = _T("");

			wchar_t cellid[16] = _T("");
			wsprintf(cellid, _T("pf_cell%d1"), i-1);
			CLabelUI* pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			wsprintf(cellvalue, _T("%d"), iter->id);
			pLabel->SetText(cellvalue);

			memset(cellid, 0, 16);
			wsprintf(cellid, _T("pf_cell%d2"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			pLabel->SetText(iter->name.c_str());

			memset(cellid, 0, 16);
			wsprintf(cellid, _T("pf_cell%d3"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			pLabel->SetText(iter->sex.c_str());

			memset(cellid, 0, 16);
			wsprintf(cellid, _T("pf_cell%d4"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			pLabel->SetText(iter->age.c_str());

			memset(cellid, 0, 16);
			wsprintf(cellid, _T("pf_cell%d5"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			pLabel->SetText(iter->createtime.c_str());

			iter++;
		} else {
			for (int k = 0; k < pRow->GetCount(); k++) {
				pRow->GetItemAt(k)->SetVisible(false);
			}
		}
	}
}


void RFMainWindow::UpdateTrainDetailPage(const PatientTrainInfo& train)
{
	wchar_t cellvalue[64] = _T("");
	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("pfv_cell01")));
	wsprintf(cellvalue, _T("%d"), train.id);
	pLabel->SetText(cellvalue);

	pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("pfv_cell02")));
	pLabel->SetText(train.name.c_str());

	pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("pfv_cell03")));
	pLabel->SetText(train.sex.c_str());

	pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("pfv_cell04")));
	pLabel->SetText(train.age.c_str());

	pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("pfv_cell05")));
	pLabel->SetText(train.createtime.c_str());
}

void RFMainWindow::UpdateBDChart(const PatientTrainDetails& trainDetails)
{
	if (trainDetails.traintype != RF_TRAIN_TYPE_BD) {
		CVerticalLayoutUI* traindetail_chart_bd_title = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("traindetail_chart_bd_title")));
		traindetail_chart_bd_title->SetVisible(false);

		CVerticalLayoutUI* traindetail_chart_bd_view = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("traindetail_chart_bd_view")));
		traindetail_chart_bd_view->SetVisible(false);
		return;
	}


	m_current_chart = RF_TRAIN_TYPE_BD;
	CVerticalLayoutUI* traindetail_chart_bd_title = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("traindetail_chart_bd_title")));
	traindetail_chart_bd_title->SetVisible(true);

	CVerticalLayoutUI* traindetail_chart_bd_view = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("traindetail_chart_bd_view")));
	traindetail_chart_bd_view->SetVisible(true);

	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("bd_cell01")));
	pLabel->SetText(trainDetails.createtime.c_str());

	pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("bd_cell02")));
	pLabel->SetText(trainDetails.traintype.c_str());

	pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("bd_cell03")));
	pLabel->SetText(trainDetails.game.c_str());

	pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("bd_cell04")));
	pLabel->SetText(trainDetails.traintime.c_str());

	RFPatientsTrainData::get()->LoadTrainData(trainDetails);
}

void RFMainWindow::UpdateJGJWaveData(std::list<LineWaveData>& trainDatas)
{
	CWaveUI* pWave = static_cast<CWaveUI*>(m_pm.FindControl(_T("bd_jgj_wave")));
	if (!pWave) {
		return;
	}

	pWave->m_lstDatas = trainDatas;
	pWave->SetStartX(0);
}

void RFMainWindow::UpdateJGJWaveXNum(double fStart)
{
	char x[32] = "";
	CLabelUI* zd_chart_x0 = static_cast<CLabelUI*>(m_pm.FindControl(_T("bd_chart_x0")));
	if (zd_chart_x0) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		zd_chart_x0->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* zd_chart_x1 = static_cast<CLabelUI*>(m_pm.FindControl(_T("bd_chart_x1")));
	if (zd_chart_x1) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		zd_chart_x1->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* zd_chart_x2 = static_cast<CLabelUI*>(m_pm.FindControl(_T("bd_chart_x2")));
	if (zd_chart_x2) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		zd_chart_x2->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* zd_chart_x3 = static_cast<CLabelUI*>(m_pm.FindControl(_T("bd_chart_x3")));
	if (zd_chart_x3) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		zd_chart_x3->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* zd_chart_x4 = static_cast<CLabelUI*>(m_pm.FindControl(_T("bd_chart_x4")));
	if (zd_chart_x4) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		zd_chart_x4->SetText(TGUTF8ToUTF16(x).c_str());
	}
}

void RFMainWindow::UpdateZGJWaveData(std::list<LineWaveData>& trainDatas)
{
	CWaveUI* pWave = static_cast<CWaveUI*>(m_pm.FindControl(_T("bd_zgj_wave")));
	if (!pWave) {
		return;
	}

	pWave->m_lstDatas = trainDatas;
	pWave->SetStartX(0);
}

void RFMainWindow::UpdateZGJWaveXNum(double fStart)
{
	char x[32] = "";
	CLabelUI* zd_chart_x0 = static_cast<CLabelUI*>(m_pm.FindControl(_T("bd_zgj_chart_x0")));
	if (zd_chart_x0) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		zd_chart_x0->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* zd_chart_x1 = static_cast<CLabelUI*>(m_pm.FindControl(_T("bd_zgj_chart_x1")));
	if (zd_chart_x1) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		zd_chart_x1->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* zd_chart_x2 = static_cast<CLabelUI*>(m_pm.FindControl(_T("bd_zgj_chart_x2")));
	if (zd_chart_x2) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		zd_chart_x2->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* zd_chart_x3 = static_cast<CLabelUI*>(m_pm.FindControl(_T("bd_zgj_chart_x3")));
	if (zd_chart_x3) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		zd_chart_x3->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* zd_chart_x4 = static_cast<CLabelUI*>(m_pm.FindControl(_T("bd_zgj_chart_x4")));
	if (zd_chart_x4) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		zd_chart_x4->SetText(TGUTF8ToUTF16(x).c_str());
	}

}

void RFMainWindow::UpdateYDChart(const PatientTrainDetails& trainDetails)
{
	if (trainDetails.traintype != RF_TRAIN_TYPE_YD) {
		CVerticalLayoutUI* traindetail_chart_yd_title = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("traindetail_chart_yd_title")));
		traindetail_chart_yd_title->SetVisible(false);

		CVerticalLayoutUI* traindetail_chart_yd_view = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("traindetail_chart_yd_view")));
		traindetail_chart_yd_view->SetVisible(false);
		return;
	}

	m_current_chart = RF_TRAIN_TYPE_YD;
	CVerticalLayoutUI* traindetail_chart_yd_title = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("traindetail_chart_yd_title")));
	traindetail_chart_yd_title->SetVisible(true);

	CVerticalLayoutUI* traindetail_chart_yd_view = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("traindetail_chart_yd_view")));
	traindetail_chart_yd_view->SetVisible(true);

	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("yd_cell01")));
	pLabel->SetText(trainDetails.createtime.c_str());

	pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("yd_cell02")));
	pLabel->SetText(trainDetails.traintype.c_str());

	pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("yd_cell03")));
	pLabel->SetText(trainDetails.traintime.c_str());

	RFPatientsTrainData::get()->LoadTrainData(trainDetails);
}

void RFMainWindow::UpdateYDGJJDWaveData(std::list<LineWaveData>& trainDatas)
{
	CWaveUI* pWave = static_cast<CWaveUI*>(m_pm.FindControl(_T("yd_gjydjd_wave")));
	if (!pWave) {
		return;
	}

	pWave->m_lstDatas = trainDatas;
	pWave->SetStartX(0);
}

void RFMainWindow::UpdateYDGJJDWaveXNum(double fStart)
{
	char x[32] = "";
	CLabelUI* yd_chart_x0 = static_cast<CLabelUI*>(m_pm.FindControl(_T("yd_chart_x0")));
	if (yd_chart_x0) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		yd_chart_x0->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* yd_chart_x1 = static_cast<CLabelUI*>(m_pm.FindControl(_T("yd_chart_x1")));
	if (yd_chart_x1) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		yd_chart_x1->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* yd_chart_x2 = static_cast<CLabelUI*>(m_pm.FindControl(_T("yd_chart_x2")));
	if (yd_chart_x2) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		yd_chart_x2->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* yd_chart_x3 = static_cast<CLabelUI*>(m_pm.FindControl(_T("yd_chart_x3")));
	if (yd_chart_x3) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		yd_chart_x3->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* yd_chart_x4 = static_cast<CLabelUI*>(m_pm.FindControl(_T("yd_chart_x4")));
	if (yd_chart_x4) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		yd_chart_x4->SetText(TGUTF8ToUTF16(x).c_str());
	}
}


void RFMainWindow::UpdateEMGChart(const PatientTrainDetails& trainDetails)
{
	if (trainDetails.traintype != RF_TRAIN_TYPE_EMG) {
		CVerticalLayoutUI* traindetail_chart_emg_title = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("traindetail_chart_emg_title")));
		traindetail_chart_emg_title->SetVisible(false);

		CVerticalLayoutUI* traindetail_chart_emg_view = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("traindetail_chart_emg_view")));
		traindetail_chart_emg_view->SetVisible(false);
		return;
	}


	m_current_chart = RF_TRAIN_TYPE_EMG;
	CVerticalLayoutUI* traindetail_chart_emg_title = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("traindetail_chart_emg_title")));
	traindetail_chart_emg_title->SetVisible(true);

	CVerticalLayoutUI* traindetail_chart_emg_view = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("traindetail_chart_emg_view")));
	traindetail_chart_emg_view->SetVisible(true);

	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("emg_cell01")));
	pLabel->SetText(trainDetails.createtime.c_str());

	pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("emg_cell02")));
	pLabel->SetText(trainDetails.traintype.c_str());

	pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("emg_cell03")));
	pLabel->SetText(trainDetails.traintime.c_str());

	RFPatientsTrainData::get()->LoadTrainData(trainDetails);
}

void RFMainWindow::UpdateEMGWaveData(std::list<LineWaveData>& trainDatas)
{
	CWaveUI* pWave = static_cast<CWaveUI*>(m_pm.FindControl(_T("emg_wave")));
	if (!pWave) {
		return;
	}

	pWave->m_lstDatas = trainDatas;
	pWave->SetStartX(0);
}


void RFMainWindow::UpdateEMGWaveXNum(double fStart)
{
	char x[32] = "";
	CLabelUI* emg_chart_x0 = static_cast<CLabelUI*>(m_pm.FindControl(_T("emg_chart_x0")));
	if (emg_chart_x0) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		emg_chart_x0->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* emg_chart_x1 = static_cast<CLabelUI*>(m_pm.FindControl(_T("emg_chart_x1")));
	if (emg_chart_x1) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		emg_chart_x1->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* emg_chart_x2 = static_cast<CLabelUI*>(m_pm.FindControl(_T("emg_chart_x2")));
	if (emg_chart_x2) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		emg_chart_x2->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* emg_chart_x3 = static_cast<CLabelUI*>(m_pm.FindControl(_T("emg_chart_x3")));
	if (emg_chart_x3) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		emg_chart_x3->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* emg_chart_x4 = static_cast<CLabelUI*>(m_pm.FindControl(_T("emg_chart_x4")));
	if (emg_chart_x4) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		emg_chart_x4->SetText(TGUTF8ToUTF16(x).c_str());
	}
}

void RFMainWindow::UpdateEMGGJYDWaveData(std::list<LineWaveData>& trainDatas)
{
	CWaveUI* pWave = static_cast<CWaveUI*>(m_pm.FindControl(_T("emg_gjyd_wave")));
	if (!pWave) {
		return;
	}

	pWave->m_lstDatas = trainDatas;
	pWave->SetStartX(0);
}

void RFMainWindow::UpdateEMGGJYDWaveXNum(double fStart)
{
	char x[32] = "";
	CLabelUI* emg_gjyd_chart_x0 = static_cast<CLabelUI*>(m_pm.FindControl(_T("emg_gjyd_chart_x0")));
	if (emg_gjyd_chart_x0) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		emg_gjyd_chart_x0->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* emg_gjyd_chart_x1 = static_cast<CLabelUI*>(m_pm.FindControl(_T("emg_gjyd_chart_x1")));
	if (emg_gjyd_chart_x1) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		emg_gjyd_chart_x1->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* emg_gjyd_chart_x2 = static_cast<CLabelUI*>(m_pm.FindControl(_T("emg_gjyd_chart_x2")));
	if (emg_gjyd_chart_x2) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		emg_gjyd_chart_x2->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* emg_gjyd_chart_x3 = static_cast<CLabelUI*>(m_pm.FindControl(_T("emg_gjyd_chart_x3")));
	if (emg_gjyd_chart_x3) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		emg_gjyd_chart_x3->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* emg_gjyd_chart_x4 = static_cast<CLabelUI*>(m_pm.FindControl(_T("emg_gjyd_chart_x4")));
	if (emg_gjyd_chart_x4) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		emg_gjyd_chart_x4->SetText(TGUTF8ToUTF16(x).c_str());
	}
}

void RFMainWindow::SaveEMGTrainData()
{
	time_t finish_time = time(NULL);

	PatientTrainDetails details;

	details.patientid = m_current_patient.id;
	details.traindate = RFToDateString(m_emg_createtime);
	details.content = RF_TRAINTYPE_STRING_EMG;
	details.duration = RFToTimeString(m_emg_createtime) + _T("-") + RFToTimeString(finish_time);
	details.traintime = RFToTimeLenth(m_emg_createtime, finish_time);
	details.createtime = RFToDateTimeString(m_emg_createtime);
	details.traintype = RF_TRAINTYPE_STRING_EMG;

	details.lasttreattime = RFToDateString(finish_time);
	details.totaltreattime = details.traintime;
	details.recoverdetail = details.content;

	details.emg_signal[0] = m_emgmode_data[0];
	details.emg_signal[1] = m_emgmode_data[1];
	details.emg_signal[2] = m_emgmode_data[2];
	details.emg_signal[3] = m_emgmode_data[3];
	details.emg_angle[0] = m_emgmode_ydjd[0];
	details.emg_angle[1] = m_emgmode_ydjd[1];
	RFPatientsTrainDetails::get()->AddPatientTrainDetails(details);
}

void RFMainWindow::UpdateZDChart(const PatientTrainDetails& trainDetails)
{
	if (trainDetails.traintype != RF_TRAIN_TYPE_ZD) {
		CVerticalLayoutUI* traindetail_chart_zd_title = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("traindetail_chart_zd_title")));
		traindetail_chart_zd_title->SetVisible(false);

		CVerticalLayoutUI* traindetail_chart_zd_view = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("traindetail_chart_zd_view")));
		traindetail_chart_zd_view->SetVisible(false);
		return;
	}

	m_current_chart = RF_TRAIN_TYPE_ZD;
	CVerticalLayoutUI* traindetail_chart_zd_title = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("traindetail_chart_zd_title")));
	traindetail_chart_zd_title->SetVisible(true);

	CVerticalLayoutUI* traindetail_chart_zd_view = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("traindetail_chart_zd_view")));
	traindetail_chart_zd_view->SetVisible(true);

	CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("zd_cell01")));
	pLabel->SetText(trainDetails.createtime.c_str());

	pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("zd_cell02")));
	pLabel->SetText(trainDetails.traintype.c_str());

	pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("zd_cell03")));
	pLabel->SetText(trainDetails.game.c_str());

	pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("zd_cell04")));
	pLabel->SetText(trainDetails.nandu.c_str());

	pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("zd_cell05")));
	pLabel->SetText(trainDetails.defen.c_str());

	RFPatientsTrainData::get()->LoadTrainData(trainDetails);
}

void RFMainWindow::UpdateZDGJJDWaveData(std::list<LineWaveData>& trainDatas)
{
	CWaveUI* pWave = static_cast<CWaveUI*>(m_pm.FindControl(_T("zd_gjjd_wave")));
	if (!pWave) {
		return;
	}

	pWave->m_lstDatas = trainDatas;
	pWave->SetStartX(0);
}

void RFMainWindow::UpdateZDWLWaveData(std::list<LineWaveData>& trainDatas)
{
	CWaveUI* pWave = static_cast<CWaveUI*>(m_pm.FindControl(_T("zd_wl_wave")));
	if (!pWave) {
		return;
	}

	pWave->m_lstDatas = trainDatas;
	pWave->SetStartX(0);
}

void RFMainWindow::UpdateZDGJJDWaveXNum(double fStart)
{
	char x[32] = "";
	CLabelUI* zd_chart_x0 = static_cast<CLabelUI*>(m_pm.FindControl(_T("zd_chart_x0")));
	if (zd_chart_x0) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		zd_chart_x0->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* zd_chart_x1 = static_cast<CLabelUI*>(m_pm.FindControl(_T("zd_chart_x1")));
	if (zd_chart_x1) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		zd_chart_x1->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* zd_chart_x2 = static_cast<CLabelUI*>(m_pm.FindControl(_T("zd_chart_x2")));
	if (zd_chart_x2) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		zd_chart_x2->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* zd_chart_x3 = static_cast<CLabelUI*>(m_pm.FindControl(_T("zd_chart_x3")));
	if (zd_chart_x3) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		zd_chart_x3->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* zd_chart_x4 = static_cast<CLabelUI*>(m_pm.FindControl(_T("zd_chart_x4")));
	if (zd_chart_x4) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		zd_chart_x4->SetText(TGUTF8ToUTF16(x).c_str());
	}
}

void RFMainWindow::UpdateZDWLWaveXNum(double fStart)
{
	char x[32] = "";
	CLabelUI* zd_wl_chart_x0 = static_cast<CLabelUI*>(m_pm.FindControl(_T("zd_wl_chart_x0")));
	if (zd_wl_chart_x0) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		zd_wl_chart_x0->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* zd_wl_chart_x1 = static_cast<CLabelUI*>(m_pm.FindControl(_T("zd_wl_chart_x1")));
	if (zd_wl_chart_x1) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		zd_wl_chart_x1->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* zd_wl_chart_x2 = static_cast<CLabelUI*>(m_pm.FindControl(_T("zd_wl_chart_x2")));
	if (zd_wl_chart_x2) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		zd_wl_chart_x2->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* zd_wl_chart_x3 = static_cast<CLabelUI*>(m_pm.FindControl(_T("zd_wl_chart_x3")));
	if (zd_wl_chart_x3) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		zd_wl_chart_x3->SetText(TGUTF8ToUTF16(x).c_str());
	}

	CLabelUI* zd_wl_chart_x4 = static_cast<CLabelUI*>(m_pm.FindControl(_T("zd_wl_chart_x4")));
	if (zd_wl_chart_x4) {
		fStart += 2;
		sprintf(x, "%3.2f", fStart);
		zd_wl_chart_x4->SetText(TGUTF8ToUTF16(x).c_str());
	}
}

void RFMainWindow::UpdateTrainDetailPageNumber(int page)
{
	wchar_t pageid[16] = _T("");

	for(int i = 0; i < 4; i++) {
		wchar_t pagevalue[16] = _T("");

		wsprintf(pageid, _T("pd_page%d"), i+1);
		CButtonUI* pButton = static_cast<CButtonUI*>(m_pm.FindControl(pageid));

		if (page % 4 == i) {
			wsprintf(pagevalue, _T("%d"), page + 1);

			pButton->SetTextColor(0xffffffff);
			pButton->SetBkColor(0xFF184199);
			pButton->SetText(pagevalue);
		} else {
			wsprintf(pagevalue, _T("%d"), (page/4)*4 + i + 1);
	
			pButton->SetTextColor(0xff000000);
			pButton->SetBkColor(0xFFFFFFFF);
			pButton->SetText(pagevalue);
		}
	}
}

void RFMainWindow::UpdateTrainDetailPage( std::list<PatientTrainDetails>& details)
{
	std::list<PatientTrainDetails>::iterator iter = details.begin();
	for (int i = 1; i < 6; i++)
	{
		wchar_t rowid[16] = _T("");
		wsprintf(rowid, _T("pd_row0%d"), i);

		CHorizontalLayoutUI *pRow = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(rowid));

		for (int j = 0; j < pRow->GetCount(); j++) {
			pRow->GetItemAt(j)->SetVisible(true);
		}

		if (iter != details.end()) {
			//pRow->SetVisible(true);
			wchar_t cellvalue[16] = _T("");

			wchar_t cellid[32] = _T("");
			wsprintf(cellid, _T("pd_cell%d1"), i-1);
			CLabelUI* pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			pLabel->SetText(iter->traindate.c_str());

			memset(cellid, 0, 32);
			wsprintf(cellid, _T("pd_cell%d2"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			pLabel->SetText(iter->content.c_str());

			memset(cellid, 0, 32);
			wsprintf(cellid, _T("pd_cell%d3"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			pLabel->SetText(iter->duration.c_str());

			memset(cellid, 0, 32);
			wsprintf(cellid, _T("pd_cell%d4"), i-1);
			pLabel = static_cast<CLabelUI*>(pRow->FindSubControl(cellid));
			//wsprintf(cellvalue, _T("%d"), iter->score);
			pLabel->SetText(iter->traintime.c_str());

			memset(cellid, 0, 32);
			wsprintf(cellid, _T("train_detail_detail%d"), i);
			CButtonUI* pButton = static_cast<CButtonUI*>(pRow->FindSubControl(cellid));
			pButton->SetTag(iter->id);

			iter++;
		} else {
			for (int k = 0; k < pRow->GetCount(); k++) {
				pRow->GetItemAt(k)->SetVisible(false);
			}
		}
	}
}

void RFMainWindow::SetPassiveTrainProgress(int time, int total, bool playing)
{
	CProgressExUI *pProgress = static_cast<CProgressExUI*>(m_pm.FindControl(_T("passive_train_progress")));
	if (pProgress) {
		pProgress->SetProgressTotal(total);
		pProgress->SetProgressCurrent(time);
	}

	CLabelUI *pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("passsive_train_progress_text")));
	if (pLabel) {
		int second1 = time / 1000 % 60;
		int minute1 = time / 60000;

		std::wstring format1 = std::wstring(minute1 > 9 ? _T("%d:") : _T("0%d:")) + std::wstring(second1 > 9 ? _T("%d") : _T("0%d"));

		int second2 = total / 1000 % 60;
		int minute2 = total / 60000;
		std::wstring format2 = std::wstring(minute2 > 9 ? _T("%d:") : _T("0%d:")) + std::wstring(second2 > 9 ? _T("%d") : _T("0%d"));

		std::wstring format = format1 + _T("/") + format2;
		wchar_t szValue[64] = _T("");
		wsprintf(szValue, format.c_str(), minute1, second1, minute2, second2);
		pLabel->SetText(szValue);
	}

	CCheckBoxUI* pCheckBox = static_cast<CCheckBoxUI*>(m_pm.FindControl(_T("passive_play")));
	if (!playing) {
		pCheckBox->SetCheck(playing);
	}
}

void RFMainWindow::AddPassiveTrainItem(CListUI* pList, const PassiveTrainInfo& train)
{
	CDialogBuilder builder;
	CListContainerElementUI *pItem = static_cast<CListContainerElementUI*>(builder.Create(_T("passivetrainitem.xml"), NULL));
	
	MEDIA media;
	media.train = train;
	media.name = train.name;
	media.exName = _T(".wav");
	media.pathFileName = CPaintManagerUI::GetResourcePath().GetData();
	media.pathFileName += _T("voice\\");
	media.pathFileName += train.name;
	media.pathFileName += _T(".wav");
	media.playNum = 0;
	media.size = FileSizeToString(GetFileSize(media.pathFileName.c_str()));
	media.totalTime = train.timelen;
	//media.totalTime = m_player.GetMusicLenght(m_zplay, media.pathFileName);

	CLabelUI* pLable = static_cast<CLabelUI*>(pItem->FindSubControl(_T("passivetrainitem_sort")));
	if (pLable) {
		pLable->SetText(train.id.c_str());
	}
	pLable = static_cast<CLabelUI*>(pItem->FindSubControl(_T("passivetrainitem_name")));
	if (pLable) {
		pLable->SetText(train.name.c_str());
	}

	pLable = static_cast<CLabelUI*>(pItem->FindSubControl(_T("passivetrainitem_type")));
	if (pLable) {
		pLable->SetText(train.traintype.c_str());
	}

	pLable = static_cast<CLabelUI*>(pItem->FindSubControl(_T("passivetrainitem_timelen")));
	if (pLable) {
		pLable->SetText(train.timelen.c_str());
	}

	CButtonUI *pButton = static_cast<CButtonUI*>(pItem->FindSubControl(_T("bd_add_action")));
	pButton->OnNotify += MakeDelegate(this, &RFMainWindow::OnBDAddActionToPlayList);

	CButtonUI *pButton1 = static_cast<CButtonUI*>(pItem->FindSubControl(_T("bd_delete_action")));
	pButton1->OnNotify += MakeDelegate(this, &RFMainWindow::OnBDDeleteAction);

	m_id_passivetrainitem[pButton->GetId()] = media;
	m_delete_id_passivetrainitem[pButton1->GetId()] = media;
	m_id_media[pItem->GetId()] = media;
	pList->Add(pItem);
}

void RFMainWindow::UpdateMusicList(CListUI* pList)
{
	if (m_current_passivetraininfos.size() < 1) {
		return;
	}
	
	pList->RemoveAll();
	int i = 0;
	std::list<MEDIA>::iterator begin = m_current_passivetraininfos.begin();
	for (; begin!=m_current_passivetraininfos.end(); begin++) {
		CDialogBuilder builder;
		CListContainerElementUI *pItem = static_cast<CListContainerElementUI*>(builder.Create(_T("musiclistitem.xml"), NULL));

		wchar_t cell_value[256] = _T("");
		CLabelUI* pLabel = static_cast<CLabelUI*>(pItem->FindSubControl(_T("music_cell01")));
		if (pLabel) {
			wsprintf(cell_value, _T("%d"), i);
			pLabel->SetText(cell_value);
		}
		if (begin->index == m_passive_train_action.m_curmedia.index) {
			pLabel->SetTextColor(0xFF114391);
		} else {
			pLabel->SetTextColor(0xff3d3d3d);
		}

		pLabel = static_cast<CLabelUI*>(pItem->FindSubControl(_T("music_cell02")));
		if (pLabel) {
			pLabel->SetText(begin->name.c_str());
		}
		if (begin->index == m_passive_train_action.m_curmedia.index) {
			pLabel->SetTextColor(0xFF114391);
		} else {
			pLabel->SetTextColor(0xff3d3d3d);
		}

		pLabel = static_cast<CLabelUI*>(pItem->FindSubControl(_T("music_cell03")));
		if (pLabel) {
			pLabel->SetText(begin->totalTime.c_str());
		}
		if (begin->index == m_passive_train_action.m_curmedia.index) {
			pLabel->SetTextColor(0xFF114391);
		} else {
			pLabel->SetTextColor(0xff3d3d3d);
		}

		CButtonUI* pButton = static_cast<CButtonUI*>(pItem->FindSubControl(_T("music_item_delete")));
		pButton->OnNotify += MakeDelegate(this, &RFMainWindow::OnMusicItemDelete);

		if (begin->index == m_passive_train_action.m_curmedia.index) {
			pItem->SetBkColor(0xFFf0f0f0);
		} else {
			pItem->SetBkColor(0);
		}

		pList->Add(pItem);
		i++;
	}
}

void RFMainWindow::StartGameRecord()
{
	s_active_game4_start = time(NULL);
	s_active_begin_recode = true;
	s_active_data[0].clear();
	s_active_data[1].clear();
	s_active_data_wl.clear();
	//m_robot.startActiveMove();
}

void RFMainWindow::StopGameRecord()
{
	s_active_begin_recode = false;
	s_active_game4_stop = time(NULL);
	//m_robot.stopActiveMove();
	SaveActiveGameDetectData();
	s_active_data[0].clear();
	s_active_data[1].clear();
	s_active_data_wl.clear();
}

void OnActiveGameDetectTimer(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	if (!RFMainWindow::MainWindow) {
		return;
	}

	CWkeWebkitUI* game4 = static_cast<CWkeWebkitUI*>(RFMainWindow::MainWindow->m_pm.FindControl(_T("game4")));
	if (!game4) {
		return;
	}

	std::wstring gameState = game4->RunJS(_T("getGameOver();"));
	if (gameState == _T("0") && !s_active_begin_recode) {
		RFMainWindow::MainWindow->StartGameRecord();
	}
	if (gameState == _T("1") && s_active_begin_recode) {
		RFMainWindow::MainWindow->StopGameRecord();
	}
	if (gameState != _T("0")) {
		return;
	}

	std::wstring gameType = game4->RunJS(_T("getGameType();"));
	if (gameType == RF_GAME_NAME_PLANE_GAOJI || gameType == RF_GAME_NAME_PLANE_ZHONGJI) {
		if (!RFMainWindow::MainWindow->m_robot.m_isActiveModeStart) {
			return;
		}
	}

	bool fire = false;
	int X = 0, Y = 0, width = 0, height = 0;
	RFMainWindow::MainWindow->m_robotEvent.GetValue(fire, X, Y);
	std::wstring width_str = game4->RunJS(_T("getWidth();"));
	std::wstring height_str = game4->RunJS(_T("getHeight();"));
	width = _wtoi(width_str.c_str());
	height = _wtoi(height_str.c_str());
	//width = RFMainWindow::MainWindow->m_robotEvent.m_nWidth;
	//height = RFMainWindow::MainWindow->m_robotEvent.m_nHeight;
	if (gameType == RF_GAME_NAME_DUIMUTOU || gameType == RF_GAME_NAME_PLANE_CHUJI) {
		if (fire) {
			game4->RunJS(_T("fire();"));
		}
	} else if (gameType == RF_GAME_NAME_BIAOQIANG) {
		int wirstForce = (int)RFMainWindow::MainWindow->m_robot.getWirstForce();
		if (wirstForce > 0 && wirstForce < 200) {
			wchar_t v[128];
			wsprintf(v, _T("(%d);"), wirstForce);
			std::wstring jsAction = _T("fire");
			jsAction += v;
			game4->RunJS(jsAction);
		}
	} else {
		if (fire) {
			game4->RunJS(_T("fire();"));
		}
		
		char message_tracing[1024];
		sprintf(message_tracing, "move to (%d, %d)", X, Y);
		LOG1(message_tracing);

		X = max(0, X);
		Y = max(0, Y);
		X = min(width, X);
		Y = min(height, Y);
		wchar_t xy[128];
		wsprintf(xy, _T("(%d,%d);"), X, Y);
		std::wstring planeMove = _T("selfmove");
		planeMove += xy;
		game4->RunJS(planeMove);
	}

	if (s_active_begin_recode) {		
		double angles[2];
		RFMainWindow::MainWindow->m_robot.getAngle(angles);

		s_active_data[0].push_back(angles[0]);
		s_active_data[1].push_back(angles[1]);

		double wl = RFMainWindow::MainWindow->m_robot.getWirstForce();
		s_active_data_wl.push_back(wl);
	}
}

std::vector<double> s_ev_data_wl;
void OnEvDetectTimer(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	if (!RFMainWindow::MainWindow) {
		return;
	}

	double wl = RFMainWindow::MainWindow->m_robot.getWirstForce();
	s_ev_data_wl.push_back(wl);
}

static UINT_PTR s_evDetectTimer = NULL;
void RFMainWindow::StartEVDetect()
{
	s_ev_data_wl.clear();
	s_evDetectTimer = ::SetTimer(NULL, 999, 200U, (TIMERPROC)OnEvDetectTimer);
}

void RFMainWindow::StopEVDetect()
{
	if (s_evDetectTimer != NULL) {
		::KillTimer(NULL, s_evDetectTimer);
		s_evDetectTimer = NULL;
	}

	SaveEVDataToDB();
}

void RFMainWindow::SaveEVDataToDB()
{
	double totalWL = .0f;
	for (int i = 0; i < s_ev_data_wl.size(); i++) {
		totalWL += s_ev_data_wl.at(i);
	}
	m_evydgn.wl = totalWL / s_ev_data_wl.size(); 
	
	CSliderUI* pSlider = static_cast<CSliderUI*>(m_pm.FindControl(_T("passive_train_progress")));
	m_evydgn.cs = 0.1 + 0.4 * pSlider->GetValue() / pSlider->GetMaxValue();
	m_evydgn.score = 2 * (m_evydgn.jb + m_evydgn.zb) / (35 * m_evydgn.cs) + 0.1*m_evydgn.wl;

	EvaluationData data;

	wchar_t score[20];
	ZeroMemory(score,sizeof(TCHAR) * 20);
	_stprintf_s(score,20,_T("%.2f"),m_evydgn.score);

	DWORD createTime = time(NULL);
	data.doctorid = m_login_info.doctorid;
	data.name = m_current_patient.name;
	data.patientid = m_current_patient.id;
	data.score = score;
	data.type = m_evalution_type;
	data.date = RFToTimeString(createTime);
	data.id = RFEvaluationData::get()->m_nextid;

	std::vector<std::wstring> questions;
	std::vector<double> answers;

	questions.push_back(_T("肘部起始角/°"));
	answers.push_back(m_evydgn.zb1);

	questions.push_back(_T("肘部终止角/°"));
	answers.push_back(m_evydgn.zb2);

	questions.push_back(_T("肘部关节活动/°"));
	answers.push_back(m_evydgn.zb);

	questions.push_back(_T("肩部起始角/°"));
	answers.push_back(m_evydgn.jb1);

	questions.push_back(_T("肩部终止角/°"));
	answers.push_back(m_evydgn.jb2);

	questions.push_back(_T("肩部关节活动/°"));
	answers.push_back(m_evydgn.jb);

	questions.push_back(_T("助力参数"));
	answers.push_back(m_evydgn.cs);

	questions.push_back(_T("握力/N"));
	answers.push_back(m_evydgn.wl);

	for (int i = 0; i < questions.size(); i++) {
		EvaluationRecordData record;

		wchar_t temp[20];
		ZeroMemory(temp,sizeof(TCHAR) * 20);
		_stprintf_s(temp,20,_T("%.2f"),answers.at(i));

		record.item = questions.at(i);
		record.result = temp;
		record.evalid = data.id;
		data.datas.push_back(record);
	}

	RFEvaluationData::get()->Add(data);
	
}

static UINT_PTR s_activeGameDetectTimer = NULL;
void RFMainWindow::StartActiveGameDetect()
{
	CWkeWebkitUI* game4 = static_cast<CWkeWebkitUI*>(RFMainWindow::MainWindow->m_pm.FindControl(_T("game4")));
	if (!game4) {
		return;
	}

	s_activeGameDetectTimer = ::SetTimer(NULL, 999, 200U, (TIMERPROC)OnActiveGameDetectTimer);

	std::wstring width = game4->RunJS(_T("getWidth();"));
	std::wstring height = game4->RunJS(_T("getHeight();"));
	int w = _wtoi(width.c_str());
	int h = _wtoi(height.c_str());
	m_robotEvent.Start(w, h);
}

void RFMainWindow::StopActiveGameDetect()
{
	if (s_activeGameDetectTimer != NULL) {
		::KillTimer(NULL, s_activeGameDetectTimer);
		s_activeGameDetectTimer = NULL;
	}
	m_robotEvent.Stop();
}

void OnEyeModeDetectTimer(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	if (RFMainWindow::MainWindow) {
		double angles[2];
		RFMainWindow::MainWindow->m_robot.getAngle(angles);
	
		s_eyemode_data[0].push_back(angles[0]);
		s_eyemode_data[1].push_back(angles[1]);
	}
}

static UINT_PTR s_eyeModeDetectTimer = NULL;
void RFMainWindow::StartEyeModeGameDetect()
{
	s_eyeModeDetectTimer = ::SetTimer(NULL, 998, 200U, (TIMERPROC)OnEyeModeDetectTimer);
}

void RFMainWindow::StopEyeModeGameDetect()
{
	if (s_eyeModeDetectTimer != NULL) {
		::KillTimer(NULL, s_eyeModeDetectTimer);
		s_eyeModeDetectTimer = NULL;
	}
}


void RFMainWindow::SaveEyeModeDetectData()
{
	PatientTrainDetails details;

	details.patientid = m_current_patient.id;
	details.traindate = RFToDateString(s_eyemode_start);
	details.traintype = RF_TRAINTYPE_STRING_YD;
	details.content = RF_TRAINTYPE_STRING_YD;
	details.duration = RFToTimeString(s_eyemode_start) + _T("-") + RFToTimeString(s_eyemode_stop);
	details.traintime = RFToTimeLenth(s_eyemode_start, s_eyemode_stop);
	details.createtime = RFToDateTimeString(s_eyemode_start);

	details.lasttreattime = RFToDateString(s_eyemode_stop);
	details.totaltreattime = details.traintime;
	details.recoverdetail = details.content;

	for (int i = 0; i < s_active_data[0].size(); i++) {
		details.target_angle.push_back(s_eyemode_data[0].at(i));
		details.target_angle.push_back(s_eyemode_data[1].at(i));
	}
	
	RFPatientsTrainDetails::get()->AddPatientTrainDetails(details);
}

void RFMainWindow::SaveActiveGameDetectData()
{
	CWkeWebkitUI* game4 = static_cast<CWkeWebkitUI*>(RFMainWindow::MainWindow->m_pm.FindControl(_T("game4")));
	if (!game4) {
		return;
	}
	std::wstring gamename = game4->RunJS(_T("getGameType();"));
	std::wstring nandu = game4->RunJS(_T("getNandu();"));
	std::wstring score = game4->RunJS(_T("getScore();"));

	PatientTrainDetails details;

	details.patientid = m_current_patient.id;
	details.traindate = RFToDateString(s_active_game4_start);
	details.traintype = RF_TRAINTYPE_STRING_ZD;
	details.content = RF_TRAINTYPE_STRING_ZD;
	details.content += _T("-");
	details.content += gamename;
	details.content += _T("-");
	details.content += nandu;
	details.content += _T("-");
	details.content += score;
	details.duration = RFToTimeString(s_active_game4_start) + _T("-") + RFToTimeString(s_active_game4_stop);
	details.traintime = RFToTimeLenth(s_active_game4_start, s_active_game4_stop);
	details.createtime = RFToDateTimeString(s_active_game4_start);

	for (int i = 0; i < s_active_data[0].size(); i++) {
		details.target_angle.push_back(s_active_data[0].at(i));
		details.target_angle.push_back(s_active_data[1].at(i));
	}
	details.target_wl = s_active_data_wl;

	details.lasttreattime = RFToDateString(s_active_game4_stop);
	details.totaltreattime = details.traintime;
	details.recoverdetail = details.content;

	RFPatientsTrainDetails::get()->AddPatientTrainDetails(details);
}

DWORD GetFileSize(LPCTSTR fileName)
{
	HANDLE hFile = ::CreateFile(fileName, 0, 0, NULL,OPEN_EXISTING, 0, NULL);
	DWORD dwSize = ::GetFileSize(hFile,0);
	if (dwSize != INVALID_FILE_SIZE)
	{
		return dwSize;
	}
	else
		return 0;

	CloseHandle(hFile);

}

LPCTSTR FileSizeToString(DWORD dwSize)
{
	TCHAR* strSize = new TCHAR[20];
	ZeroMemory(strSize,sizeof(TCHAR) * 20);
	_stprintf_s(strSize,20,_T("%.2f M"),float(dwSize) / 1024 /1024);
	return strSize;
}


std::wstring GetSaveFilePath(HWND hWnd)
{
	TCHAR  szFilter[] = L"Report(*.xlsx)\0*.xlsx\0";

	OPENFILENAME ofn;
	int buffer_size = 256*MAX_PATH;
	wchar_t *buffer = new wchar_t[buffer_size];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = szFilter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = buffer;
	ofn.lpstrFile[0] = L'\0';
	ofn.nMaxFile = buffer_size;
	ofn.lpstrFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = L"xlsx";
	ofn.lpstrTitle = L"保存文件";
	ofn.Flags = OFN_OVERWRITEPROMPT;

	GetSaveFileName(&ofn);

	return buffer;
}

int GetFilesFromDialog(HWND wnd, std::vector<std::wstring>& files, const wchar_t *filter, int flags)
{
	int r = 0;
	OPENFILENAME ofn;
	int buffer_size = 256*MAX_PATH;
	wchar_t *buffer = new wchar_t[buffer_size];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = wnd;
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = buffer;
	ofn.lpstrFile[0] = L'\0';
	ofn.nMaxFile = buffer_size;
	ofn.lpstrFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = L"请选择文件";
	ofn.Flags = flags;

	std::wstring dir;
	int n = 0;
	if (::GetOpenFileName(&ofn)) {
		n = lstrlen(buffer);
		dir = std::wstring(buffer, buffer+n);
		if (buffer[ofn.nFileOffset-1] == L'\\') {
			files.push_back(dir);
		} else {
			const wchar_t* dir_p = (wchar_t*)dir.c_str();
			int dir_len = dir.size();
			if (dir_p[dir_len - 1] != L'\\') {
				dir += L"\\";
			}

			wchar_t *p = buffer + ofn.nFileOffset;
			while (*p) {
				n = lstrlen(p);
				files.push_back(dir + std::wstring(p, p+n));
				p += n+1;
			}
		}
	} else {
		r = -1;
	}

	delete [] buffer;

	return r;
}


std::wstring RFToTimeString(time_t in_t)
{
	time_t t = in_t;
	if (t < 0 || t > 0x7fffd27f) {
		t = 0;
	}
	wchar_t buffer[BUFSIZ];
	wcsftime(buffer, BUFSIZ, L"%H:%M:%S", localtime(&t));
	return buffer;
}

std::wstring RFToDateString(time_t in_t)
{
	time_t t = in_t;
	if (t < 0 || t > 0x7fffd27f) {
		t = 0;
	}
	wchar_t buffer[BUFSIZ];
	wcsftime(buffer, BUFSIZ, L"%Y-%m-%d", localtime(&t));
	return buffer;
}

std::wstring RFToTimeLenth(time_t start, time_t end)
{
	wchar_t buffer[BUFSIZ];

	DWORD dwDiff = end - start;

	std::wstring buffer_format = _T("");
	
	if (dwDiff / 3600 < 10) {
		buffer_format += _T("0%d:");
	} else {
		buffer_format += _T("%d:");
	} 

	if ((dwDiff % 3600) / 60 < 10) {
		buffer_format += _T("0%d:");
	}  else {
		buffer_format += _T("%d:");
	} 

	if ((dwDiff % 3600) % 60 < 10) {
		buffer_format += _T("0%d");
	}  else {
		buffer_format += _T("%d");
	} 

	wsprintf(buffer, buffer_format.c_str(), dwDiff / 3600, (dwDiff % 3600) / 60 , (dwDiff % 3600) % 60);

	return buffer;
}

std::wstring RFToDateTimeString(time_t in_t)
{
	time_t t = in_t;
	if (t < 0 || t > 0x7fffd27f) {
		t = 0;
	}
	wchar_t buffer[BUFSIZ];
	wcsftime(buffer, BUFSIZ, L"%Y-%m-%d %H:%M:%S", localtime(&t));
	return buffer;
}