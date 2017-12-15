#pragma once
#include "RFMySQLThread.h"
#include "MusicPlayer.h"
#include "RFPassiveTrainAction.h"
#include "UIChart.h"
#include "robot.h"
#include "RFRobotEventControl.h"
#include "RFMAS.h"

#define WM_EMG_DATA_SAMPLE_MSG 2050

#define RF_TRAIN_TYPE_ZD _T("主动训练")
#define RF_TRAIN_TYPE_BD _T("被动训练")
#define RF_TRAIN_TYPE_YD _T("眼动训练")
#define RF_TRAIN_TYPE_EMG _T("EMG")

#define RF_GAME_NAME_PLANE_CHUJI _T("飞机大战初级")
#define RF_GAME_NAME_PLANE_ZHONGJI _T("飞机大战中级")
#define RF_GAME_NAME_PLANE_GAOJI _T("飞机大战高级")
#define RF_GAME_NAME_BIAOQIANG _T("标枪")
#define RF_GAME_NAME_DUIMUTOU _T("堆木头")

#define RF_DESIGN_WINDOW_WIDTH 1200
#define RF_DESIGN_WINDOW_HEIGHT 675

#define RF_WINDOW_WIDTH 1920
#define RF_WINDOW_HEIGHT 1080

class RFMainWindow : public CWindowWnd, public INotifyUI, public IMessageFilterUI, public IDialogBuilderCallback
{
public:
	RFMainWindow();
	~RFMainWindow();
	LPCTSTR GetWindowClassName() const;
	UINT GetClassStyle() const;

	void OnFinalMessage(HWND /*hWnd*/);
	CControlUI* CreateControl(LPCTSTR pstrClass);

	void Init();
	void Closing();
	void OnPrepare();
	void Notify(TNotifyUI& msg);

	void BindSelectPatientPageEvent();
	void BindManagerPatientPageEvent();

	LRESULT OnCommunicate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnAppClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDuiCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMenuClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);	
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnEmgSampleData(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	bool		OnLogin(void *pParam);

	static int	OnLoginOK(EventArg *pArg);
	static int	OnConnectOK(EventArg *pArg);
	static int	OnConnectFail(EventArg *pArg);

	bool		OnGame4NanduSetingMenu(void *pParam);

	bool		OnPersonerCenterMenu(void *pParam);
	bool		OnEntry(void *pParam);
	bool		OnSelectPatient(void *pParam);
	bool		OnManagerPatient(void *pParam);
	bool		OnPatientTrain(void *pParam);
	bool		OnPatientTrainFromActiveTrain(void *pParam);
	bool		OnPatientTrainFromPassiveTrain(void *pParam);
	bool		OnPatientTrainMessage(void *pParam);
	bool		OnPatientTrainDetail(void *pParam);
	bool		OnPassessment(void *pParam);
	bool		OnAbout(void *pParam);
	bool		OnEvaluation1(void *pParam);
	bool		OnEvaluation2(void *pParam);
	bool		OnEvaluation3(void *pParam);

	bool		OnPatientTrainFromEyeMode(void *pParam);

	bool		OnChangePersonInfoPage(void *pParam);
	bool		OnChangePasswordPage(void *pParam);


	bool		OnPrevPage(void *pParam);
	bool		OnPage1(void *pParam);
	bool		OnPage2(void *pParam);
	bool		OnPage3(void *pParam);
	bool		OnPage4(void *pParam);
	bool		OnNextPage(void *pParam);
	bool		OnReturnMainPage(void *pParam);
	bool		OnSearch(void *pParam);
	
	bool		OnEVLastPage(void* pParam);
	bool		OnEVPage1(void *pParam);
	bool		OnEVPage2(void *pParam);
	bool		OnEVPage3(void *pParam);
	bool		OnEVPage4(void *pParam);
	bool		OnEVNextPage(void *pParam);
	bool		OnReturnEvaluationPage(void* pParam);
	bool		OnReturnEvaluationHistoryPage(void *pParam);
	bool		OnEVAdd(void *pParam);
	bool		OnEVAddPrev(void *pParam);
	bool		OnEVAddSubmit(void *pParam);
	bool		OnEVAddNext(void *pParam);
	void		ShowEVDetail(std::wstring evid);
	bool		OnEVDLastPage(void* pParam);
	bool		OnEVDPage1(void *pParam);
	bool		OnEVDPage2(void *pParam);
	bool		OnEVDPage3(void *pParam);
	bool		OnEVDPage4(void *pParam);
	bool		OnEVDNextPage(void *pParam);
	bool		OnEVStart(void *pParam);
	bool		OnEVStop(void *pParam);
	bool		OnEVBegin1(void *pParam);
	bool		OnEVBegin2(void *pParam);

	bool		OnManagerPrevPage(void *pParam);
	bool		OnManagerPage1(void *pParam);
	bool		OnManagerPage2(void *pParam);
	bool		OnManagerPage3(void *pParam);
	bool		OnManagerPage4(void *pParam);
	bool		OnManagerNextPage(void *pParam);
	bool		OnManagerSearch(void *pParam);
	bool		OnManagerPatientAdd(void *pParam);

	bool		OnPatientTrainInfoPrevPage(void *pParam);
	bool		OnPatientTrainInfoPage1(void *pParam);
	bool		OnPatientTrainInfoPage2(void *pParam);
	bool		OnPatientTrainInfoPage3(void *pParam);
	bool		OnPatientTrainInfoPage4(void *pParam);
	bool		OnPatientTrainInfoNextPage(void *pParam);
	bool		OnPatientTrainSearch(void *pParam);

	bool		OnPatientTrainDetailSearch(void *pParam);
	bool		OnPatientTrainDetailPrevPage(void *pParam);
	bool		OnPatientTrainDetailPage1(void *pParam);
	bool		OnPatientTrainDetailPage2(void *pParam);
	bool		OnPatientTrainDetailPage3(void *pParam);
	bool		OnPatientTrainDetailPage4(void *pParam);
	bool		OnPatientTrainDetailNextPage(void *pParam);

	bool		OnDetailPageDelete(void *pParam);
	bool		OnDetailPageEdit(void *pParam);
	bool		OnSavePatient(void *pParam);
	bool		OnSavePersonInfo(void *pParam);
	bool		OnAddSavePatient(void *pParam);
	bool		OnModifyPwdInfo(void *pParam);
	bool		OnFilter(void *pParam);
	bool		OnExportFilterPatient(void *pParam);
	bool		OnImportPatient(void *pParam);
	bool		OnExportFilterPatientDetail(void *pParam);
	bool		OnTrainInfoExport(void *pParam);
	bool		OnTrainDetailExport(void *pParam);

	bool		OnActiveTrain(void *pParam);
	bool		OnActiveTrainFromGame(void* pParam);
	bool		OnPassiveTrain(void *pParam);
	bool		OnPassiveTrainPlay(void *pParam);
	bool		OnPassiveTrainPlayNext(void *pParam);
	bool		OnPassiveTrainPlayPrev(void *pParam);
	bool		OnPassiveTrainRecover(void *pParam);
	bool		OnPassiveTrainPlayByOrder(void *pParam);
	bool		OnPassiveTrainPlayByAuto(void *pParam);
	bool		OnPassiveTrainVolumeSet(void *pParam);
	bool		OnAddAction(void *pParam);
	bool		OnBDAddActionToPlayList(void *pParam);
	bool        OnBDDeleteAction(void *pParam);

	bool		OnZDGGJDChart(void *pParam);
	bool		OnZDWLChart(void *pParam);
	bool		OnBDZGJChart(void *pParam);
	bool		OnBDJGJChart(void *pParam);
	bool		OnEMGChart(void *pParam);
	bool		OnEMGYDJDChart(void *pParam);	
	bool		OnTrainDataExport(void *pParam);

	bool		OnEyeModeSetting(void *pParam);
	bool		OnEyeModeTrainPage(void *pParam);
	bool		OnEyeModeStartStop(void *pParam);
	bool		OnEyeModeRecovery(void *pParam);
	bool		OnEMGModeTrainPage(void *pParam);
	
	bool		OnEMGModeStart(void *pParam);
	bool		OnEMGModeRecovery(void *pParam);
	bool		OnZhudongFeiji(void *pParam);	
	bool		OnZhudongBiaoqiang(void *pParam);
	bool		OnZhudongDuimutou(void *pParam);
	bool		OnGame4(void *pParam);
	bool		OnGame3(void *pParam);
	bool		OnGame2(void *pParam);
	bool		OnGame4Start(void *pParam);
	bool		OnGame4Recovery(void *pParam);
	bool		OnMusicItemDelete(void *pParam);

	static int	OnSearchOK(EventArg *pArg);
	static int  OnFilterOK(EventArg* pArg);

	void		ShowLoginPage();
	void		ShowLoginSuccessPage();
	void		ShowMainPage();
	void		ShowSelectPatientPage();
	void		ShowManagerPatientPage();
	void		ShowTrainPage();
	void		ShowActiveTrainPage();
	void		ShowPassiveTrainPage();
	void		ShowPatientDetail(int page, int index);
	void		ShowPatientEdit(int page, int index);
	void		ShowPatientEdit(int patientid);
	void		ShowPatientAdd(std::wstring patientid);
	void		ShowAboutPage();
	void		ShowPersonorPage();
	void		ShowPatientTrainInformation();
	void		ShowPatientTrainDetail(std::wstring patientid);
	void		ShowEyeModeTrainPage();
	void		ShowEmgModeTrainPage();
	void		ShowTrainDataChartPage(int id);
	void		ShowGame4();
	void		ShowEvaluationPage();
	void		ShowEvaluationHistoryPage();
	void		ShowEvaluationAddPage();
	void		ShowEvaluationYDGNAddPage();
	void		ShowEvaluationDetailPage();
	void		ShowEvaluationYDGNDetailPage();
	
	
	void		DeletePatient(int patientid);

	void		UpdatePageNumber(int page);
	void		UpdatePatientPage(std::list<PatientInfo>& patients);

	void		UpdateManagePageNumber(int page);
	void		UpdateManagePatientPage(std::list<PatientInfo>& patients);
	void		UpdatePersonInfo();
	LoginInfo	GetPersonInfo();

	void		UpdateEvaluationScore(std::wstring score);
	void		UpdateEvaluationNumber(int page);
	void		UpdateEvaluationPage(std::list<EvaluationData> datas);

	void		UpdateEvaluationDetailScore(std::wstring score);
	void		UpdateEvaluationDetailNumber(int page);
	void		UpdateEvaluationDetailPage(std::list<EvaluationRecordData> datas);

	void		SaveEVAnserToDB();
	bool		UpdateEVAnswerToData();
	void		UpdateEVAnswerToUI();
	void		UpdateEVIndex();
	void		UpdateEVQuestion();
	void		UpdateEVAnswer();

	void		UpdateTrainInfoPageNumber(int page);
	void		UpdateTrainInfoPage(std::list<PatientTrainInfo> trains);

	void		UpdateTrainDetailPage(const PatientTrainInfo& train);
	void		UpdateTrainDetailPageNumber(int page);
	void		UpdateTrainDetailPage( std::list<PatientTrainDetails>& details);

	void		UpdateZDChart(const PatientTrainDetails& trainDetails);
	void		UpdateZDGJJDWaveData(std::list<LineWaveData>& trainDatas);
	void		UpdateZDGJJDWaveXNum(double fStart);
	void		UpdateZDWLWaveData(std::list<LineWaveData>& trainDatas);
	void		UpdateZDWLWaveXNum(double fStart);

	void		UpdateBDChart(const PatientTrainDetails& trainDetails);
	void		UpdateJGJWaveData(std::list<LineWaveData>& trainDatas);
	void		UpdateJGJWaveXNum(double fStart);
	void		UpdateZGJWaveData(std::list<LineWaveData>& trainDatas);
	void		UpdateZGJWaveXNum(double fStart);

	void		UpdateYDChart(const PatientTrainDetails& trainDetails);
	void		UpdateYDGJJDWaveData(std::list<LineWaveData>& trainDatas);
	void		UpdateYDGJJDWaveXNum(double fStart);

	void		UpdateEMGChart(const PatientTrainDetails& trainDetails);
	void		UpdateEMGWaveData(std::list<LineWaveData>& trainDatas);
	void		UpdateEMGWaveXNum(double fStart);
	void		UpdateEMGGJYDWaveData(std::list<LineWaveData>& trainDatas);
	void		UpdateEMGGJYDWaveXNum(double fStart);
	void		SaveEMGTrainData();

	void		SetPassiveTrainProgress(int time, int total, bool playing);
	void		AddPassiveTrainItem(CListUI* pList, const PassiveTrainInfo& train);
	void		UpdateMusicList(CListUI* pList);
	
	void		StartEyeModeGameDetect();
	void		StopEyeModeGameDetect();
	void		SaveEyeModeDetectData();

	void		StartActiveGameDetect();
	void		StopActiveGameDetect();
	void		SaveActiveGameDetectData();
	void		StartGameRecord();
	void		StopGameRecord();

	void		StartEVDetect();
	void		StopEVDetect();
	void		SaveEVDataToDB();


	CPaintManagerUI m_pm;
	static RFMySQLThread*	DBThread;
	static CUIThread*		UIThread;
	static RFMainWindow*	MainWindow;

	CLabelUI*				m_welcom;
	CVerticalLayoutUI*		m_login_page;
	CContainerUI*			m_login_input_page;
	CContainerUI*			m_login_success_page;
	CVerticalLayoutUI*		m_main_page;
	CHorizontalLayoutUI*	m_patient_select_page;
	CHorizontalLayoutUI*	m_patient_manager_page;
	CHorizontalLayoutUI*	m_patient_detail_page;
	CHorizontalLayoutUI*	m_patient_edit_page;
	CHorizontalLayoutUI*	m_patient_add_page;
	CHorizontalLayoutUI*	m_about_page;
	CHorizontalLayoutUI*	m_personor_info_edit_page;
	CHorizontalLayoutUI*	m_train_main_page;
	CHorizontalLayoutUI*	m_active_train_page;
	CHorizontalLayoutUI*	m_patient_train_info;
	CHorizontalLayoutUI*	m_patient_train_detail;
	CHorizontalLayoutUI*	m_passive_train_page;
	CHorizontalLayoutUI*	m_eyemode_page;
	CHorizontalLayoutUI*	m_emgmode_page;
	CHorizontalLayoutUI*	m_traindetail_chart;
	CHorizontalLayoutUI*	m_active_train_game4_page;
	CHorizontalLayoutUI*	m_evaluation_page;
	CHorizontalLayoutUI*	m_evaluation_history_page;
	CHorizontalLayoutUI*	m_evaluation_add_page;
	CHorizontalLayoutUI*	m_evaluation_detail_page;
	CHorizontalLayoutUI*	m_evaluation_ydgn_add_page;
	CHorizontalLayoutUI*	m_evaluation_ydgn_detail_page;

	CHorizontalLayoutUI*	m_manager_patient_header;
	CHorizontalLayoutUI*	m_manager_patient_filterheader;

	CButtonUI*				m_welcom_menu;
	CButtonUI*				m_main_tip;
	bool					m_mysql_connected;

	LoginInfo				m_login_info;
	PatientInfo				m_current_patient;
	std::wstring			m_current_patient_id_detail;

	RFPassiveTrainAction			m_passive_train_action;
	std::map<int, MEDIA>			m_id_media;
	std::map<int, MEDIA>			m_id_passivetrainitem;
	std::map<int, MEDIA>			m_delete_id_passivetrainitem;
	std::list<MEDIA>				m_current_passivetraininfos;

	std::wstring					m_current_chart;

	UINT_PTR						m_emgmode_timer;

	time_t							m_emg_createtime;
	DWORD							m_emgmode_tracetime;
	std::vector<double>				m_emgmode_data[4];
	std::vector<double>				m_emgmode_ydjd[2];

	RFRobotEventControl				m_robotEvent;
	robot							m_robot;

	RFMAS							m_mas;
	RFFMA							m_fma;
	int								m_evalution_type;
	EvaluationYDGN					m_evydgn;
};


int				GetFilesFromDialog(HWND wnd, std::vector<std::wstring>& files, const wchar_t *filter, int flags);
std::wstring	GetSaveFilePath(HWND hWnd);
LPCTSTR			FileSizeToString(DWORD dwSize);
DWORD			GetFileSize(LPCTSTR fileName);
std::wstring	RFToTimeString(time_t in_t);
std::wstring	RFToDateString(time_t in_t);
std::wstring	RFToTimeLenth(time_t start, time_t end);
std::wstring	RFToDateTimeString(time_t in_t);

