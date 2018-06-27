#include "StdAfx.h"
#include "RFLoginFailWindow.h"
#include "RFMainWindow.h"
#include "RFPassiveTrain.h"
#include "RFMySQLThread.h"
#include "RFCharSet.h"

bool RFDialog::m_delete_patient_flag = false;
RFDialog* RFDialog::Dialog = NULL;

bool RFDialog::m_eyemode_ok = false;
double RFDialog::m_eyemode_sudu = .0f;
bool	RFDialog::m_eyemode_close = true;

static UINT_PTR recordTimer = NULL;

RFDialog::RFDialog(std::wstring skin)
{
	m_tickcount = 0;
	m_skin = skin;

	RFDialog::Dialog = this;
}

void RFDialog::Init()
{
	CButtonUI* login = static_cast<CButtonUI*>(m_pm.FindControl(_T("relogin")));
	if (login) {
		login->OnNotify += MakeDelegate(this, &RFDialog::OnReLogin);
	}
	
	CButtonUI* selectnow = static_cast<CButtonUI*>(m_pm.FindControl(_T("selectnow")));
	if (selectnow) {
		selectnow->OnNotify += MakeDelegate(this, &RFDialog::OnSelectNow);
	}

	CButtonUI* remodifypwd = static_cast<CButtonUI*>(m_pm.FindControl(_T("remodifypwd")));
	if (remodifypwd) {
		remodifypwd->OnNotify += MakeDelegate(this, &RFDialog::OnReMofidyPwd);
	}

	CButtonUI *ok = static_cast<CButtonUI*>(m_pm.FindControl(_T("ok")));
	if (ok) {
		ok->OnNotify += MakeDelegate(this, &RFDialog::OnOKClose);
	}

	CButtonUI *cancel = static_cast<CButtonUI*>(m_pm.FindControl(_T("cancel")));
	if (cancel) {
		cancel->OnNotify += MakeDelegate(this, &RFDialog::OnCancelClose);
	}

	CButtonUI *eyemode_ok = static_cast<CButtonUI*>(m_pm.FindControl(_T("eyemode_ok")));
	if (eyemode_ok) {
		eyemode_ok->OnNotify += MakeDelegate(this, &RFDialog::OnEyeModeOKClose);
	}

	CButtonUI *eyemode_cancel = static_cast<CButtonUI*>(m_pm.FindControl(_T("eyemode_cancel")));
	if (eyemode_cancel) {
		eyemode_cancel->OnNotify += MakeDelegate(this, &RFDialog::OnEyeModeCancelClose);
	}


	CButtonUI *eyemode_jian = static_cast<CButtonUI*>(m_pm.FindControl(_T("eyemode_jian")));
	if (eyemode_jian) {
		eyemode_jian->OnNotify += MakeDelegate(this, &RFDialog::OnEyeModeJian);
	}

	CButtonUI *eyemode_jia = static_cast<CButtonUI*>(m_pm.FindControl(_T("eyemode_jia")));
	if (eyemode_jia) {
		eyemode_jia->OnNotify += MakeDelegate(this, &RFDialog::OnEyeModeJia);
	}	

	CCheckBoxUI* eyemode_show = static_cast<CCheckBoxUI*>(m_pm.FindControl(_T("eyemode_show")));
	if (eyemode_show) {
		eyemode_show->SetCheck(m_eyemode_close);
	}

	CButtonUI *delete_patient_ok = static_cast<CButtonUI*>(m_pm.FindControl(_T("delete_patient_ok")));
	if (delete_patient_ok) {
		delete_patient_ok->OnNotify += MakeDelegate(this, &RFDialog::OnDeletePatientOK);
	}	

	CButtonUI *delete_patient_cancel = static_cast<CButtonUI*>(m_pm.FindControl(_T("delete_patient_cancel")));
	if (delete_patient_cancel) {
		delete_patient_cancel->OnNotify += MakeDelegate(this, &RFDialog::OnDeletePatientCancel);
	}	

	CButtonUI *btn_record_action = static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_record_action")));
	if (btn_record_action) {
		btn_record_action->OnNotify += MakeDelegate(this, &RFDialog::OnRecordPasvTrain);
	}

	CButtonUI *btn_bding =  static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_bding")));
	if (btn_bding) {
		btn_bding->OnNotify += MakeDelegate(this, &RFDialog::OnBiaoding);
	}
}

void RFDialog::Notify(TNotifyUI& msg)
{
	
}

LRESULT RFDialog::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	m_pm.AddPreMessageFilter(this);
	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(m_skin.c_str(), (UINT)0, NULL, &m_pm);
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();
	return 0;
}

LRESULT RFDialog::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT RFDialog::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT RFDialog::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT RFDialog::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return HTCLIENT;
}

LRESULT RFDialog::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT RFDialog::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch( uMsg ) {
		case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		default:
			bHandled = FALSE;
	}
	if( bHandled ) return lRes;
	if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

LRESULT RFDialog::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if( uMsg == WM_KEYDOWN ) {
		if( wParam == VK_ESCAPE ) {
			Close();
			return true;
		}

	}
	return false;
}


bool RFDialog::OnReLogin(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;
	
	Close();
}

bool RFDialog::OnSelectNow(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	Close();
	return RFMainWindow::MainWindow->OnSelectPatient(pParam);
}

bool RFDialog::OnReMofidyPwd(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	Close();
	return true;
}

bool RFDialog::OnCancelClose(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	if (RFMainWindow::MainWindow->m_robot.IsPassiveTeaching()) {
		m_tickcount = 0;
		::KillTimer(NULL, recordTimer);
		recordTimer = NULL;
		RFMainWindow::MainWindow->m_robot.stopTeach();
	}

	Close();
	return true;
}


bool RFDialog::OnOKClose(void *pParam) {
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return true;

	//当主动运动没有结束的时候就点击了OK，这个时候我们应该没有反应
	if (RFMainWindow::MainWindow->m_robot.IsPassiveTeaching()) {
		return true;
	}

	std::wstring actionname = _T("");
	CEditUI *pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("addaction_actionname")));
	if (pEdit) {
		actionname = pEdit->GetText();
	}
	if (!actionname.empty()) {
		Teach teach;
		RFMainWindow::MainWindow->m_robot.getCurrentTeach(teach);

		if (teach.Target_Pos[0].size() > 0 && teach.Target_Pos[1].size() > 0 &&
			teach.Target_Vel[0].size() > 0 && teach.Target_Vel[1].size() > 0) {
			RFMainWindow::MainWindow->m_robot.addPasvMove();

			PassiveTrainInfo train;
			train.name = actionname;
			train.target_pos[0] = teach.Target_Pos[0];
			train.target_pos[1] = teach.Target_Pos[1];
			train.target_vel[0] = teach.Target_Vel[0];
			train.target_vel[1] = teach.Target_Vel[1];

			//在这里我们加入一个获取id的方法，首先从数据库中取id这一行
			std::string sql = "select count(*) from passivetrain";
			RFMYSQLStmt stmt;
			if (stmt.Prepare(RFMainWindow::DBThread->m_db, sql.c_str()) > 0) {
				if (stmt.Step() < 0) {
					train.id = std::wstring(_T("1"));
				}
				else {
					wchar_t id[64] = _T("");
					wsprintf(id, _T("%d"), stmt.GetInt(0) + 1);
					train.id = id;
				}
				stmt.Finalize();

				int totalsecond = train.target_pos[0].size();
				int minute = totalsecond / 60;
				int second = totalsecond % 60;
				std::wstring formt = (minute < 10) ? _T("0%d:") : _T("%d:");
				formt += (second < 10) ? _T("0%d") : _T("%d");

				wchar_t timelen[64] = _T("");
				wsprintf(timelen, formt.c_str(), minute, second);
				train.timelen = timelen;
				train.traintype = _T("被动训练");
				RFPassiveTrain::get()->AddPassiveTrainInfo(train);
			}

			std::string text = "现在开始" + TGUTF16ToGBK(actionname) + "动作";
			std::wstring filepathname = (std::wstring)m_pm.GetResourcePath() + _T("/voice/") + actionname + _T(".wav");

			TTSSampleData *pTTSSampleData = new TTSSampleData;
			pTTSSampleData->filepath = TGUTF16ToGBK(filepathname);
			pTTSSampleData->text = text;
			CTask::Assign(CTask::NotWait, Panic(), pTTSSampleData, EventHandle(&RFMySQLThread::TTSSample), RFMainWindow::UIThread, RFMainWindow::DBThread);

		}
		Close();
		return true;
	}
}


bool RFDialog::OnEyeModeCancelClose(void *pParam) {
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	RFDialog::m_eyemode_ok = false;
	Close();
	return true;
}


bool RFDialog::OnEyeModeOKClose(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	CLabelUI* peyemode_value = static_cast<CLabelUI*>(m_pm.FindControl(_T("eyemode_value")));
	if (peyemode_value) {
		std::wstring eyemodevel = peyemode_value->GetText();

		std::map<std::wstring, double> mapvalue;
		mapvalue[_T("0.0")] = 0.0f;
		mapvalue[_T("0.1")] = 0.1f;
		mapvalue[_T("0.2")] = 0.2f;
		mapvalue[_T("0.3")] = 0.3f;
		mapvalue[_T("0.4")] = 0.4f;
		mapvalue[_T("0.5")] = 0.5f;
		mapvalue[_T("0.6")] = 0.6f;
		mapvalue[_T("0.7")] = 0.7f;
		mapvalue[_T("0.8")] = 0.8f;
		mapvalue[_T("0.9")] = 0.9f;
		mapvalue[_T("1.0")] = 1.0f;

		RFDialog::m_eyemode_sudu = mapvalue[eyemodevel];
	}

	CCheckBoxUI* eyemode_show = static_cast<CCheckBoxUI*>(m_pm.FindControl(_T("eyemode_show")));
	if (eyemode_show) {
		RFDialog::m_eyemode_close = eyemode_show->GetCheck();
	}

	RFDialog::m_eyemode_ok = true;

	Close();
	return true;
}


bool RFDialog::OnDeletePatientOK(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	RFDialog::m_delete_patient_flag = true;
	Close();
	return true;
}


bool RFDialog::OnDeletePatientCancel(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	RFDialog::m_delete_patient_flag = false;
	return true;
}

bool RFDialog::OnEyeModeJia(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	CLabelUI* pLabelUI = static_cast<CLabelUI*>(m_pm.FindControl(_T("eyemode_value")));
	std::wstring value = pLabelUI->GetText();

	std::map<std::wstring, std::wstring> mapvalue;
	mapvalue[_T("0.0")] = _T("0.1");
	mapvalue[_T("0.1")] = _T("0.2");
	mapvalue[_T("0.2")] = _T("0.3");
	mapvalue[_T("0.3")] = _T("0.4");
	mapvalue[_T("0.4")] = _T("0.5");
	mapvalue[_T("0.5")] = _T("0.6");
	mapvalue[_T("0.6")] = _T("0.7");
	mapvalue[_T("0.7")] = _T("0.8");
	mapvalue[_T("0.8")] = _T("0.9");
	mapvalue[_T("0.9")] = _T("1.0");
	mapvalue[_T("1.0")] = _T("1.0");
	pLabelUI->SetText(mapvalue[value].c_str());
	return true;
}


void OnRecordTimer(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	if (!RFDialog::Dialog) {
		return;
	}
	RFDialog::Dialog->m_tickcount += 1000;


	CLabelUI* pLabel = static_cast<CLabelUI*>(RFDialog::Dialog->m_pm.FindControl(_T("lbl_record_time")));
	if (pLabel) {
		wchar_t time[32];

		int minute = RFDialog::Dialog->m_tickcount / 60000;
		int second = (RFDialog::Dialog->m_tickcount / 1000) % 60;
		std::wstring formt = (minute < 10) ? _T("0%d:") : _T("%d:");
		formt += (second < 10) ? _T("0%d") : _T("%d");
		wsprintf(time, formt.c_str(), minute, second);

		pLabel->SetText(time);
	}
}

bool RFDialog::OnRecordPasvTrain(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	
	CCheckBoxUI* pStart = static_cast<CCheckBoxUI*>(pMsg->pSender);
	if (!pStart->GetCheck()) {
		//这个是开始录制
		m_tickcount = 0;
		RFMainWindow::MainWindow->m_robot.startTeach();
		if (recordTimer) {
			::KillTimer(NULL, recordTimer);
		}
		recordTimer = ::SetTimer(NULL, 999, 1000U, (TIMERPROC)OnRecordTimer);
	} else {
		//结束录制
		m_tickcount = 0;
		::KillTimer(NULL, recordTimer);
		recordTimer = NULL;
		RFMainWindow::MainWindow->m_robot.stopTeach();
	}
	return true;
}

bool RFDialog::OnBiaoding(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;
	RFMainWindow::MainWindow->m_robot.eyeCalibrate();
	return true;
}

bool RFDialog::OnEyeModeJian(void *pParam)
{
	TNotifyUI *pMsg = static_cast<TNotifyUI*>(pParam);
	if (pMsg->sType != _T("click"))
		return false;

	CLabelUI* pLabelUI = static_cast<CLabelUI*>(m_pm.FindControl(_T("eyemode_value")));
	std::wstring value = pLabelUI->GetText();

	std::map<std::wstring, std::wstring> mapvalue;
	mapvalue[_T("0.0")] = _T("0.0");
	mapvalue[_T("0.1")] = _T("0.0");
	mapvalue[_T("0.2")] = _T("0.1");
	mapvalue[_T("0.3")] = _T("0.2");
	mapvalue[_T("0.4")] = _T("0.3");
	mapvalue[_T("0.5")] = _T("0.4");
	mapvalue[_T("0.6")] = _T("0.5");
	mapvalue[_T("0.7")] = _T("0.6");
	mapvalue[_T("0.8")] = _T("0.7");
	mapvalue[_T("0.9")] = _T("0.8");
	mapvalue[_T("1.0")] = _T("0.9");
	pLabelUI->SetText(mapvalue[value].c_str());

	return true;
}

void RFConnectFailDialog(HWND wnd)
{
	RFDialog* pLoginFrame = new RFDialog(_T("connectfail.xml"));
	pLoginFrame->Create(wnd, _T(""), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pLoginFrame->CenterWindow();
	pLoginFrame->ShowModal();
}


void RFLoginFailDialog(HWND wnd)
{
	RFDialog* pLoginFrame = new RFDialog(_T("loginfail.xml"));
	pLoginFrame->Create(wnd, _T(""), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pLoginFrame->CenterWindow();
	pLoginFrame->ShowModal();
}

void RFModifyPwdFailedDialog(HWND wnd, int reason)
{
	std::wstring xml = _T("modifypwdfailed2.xml");
	if (reason == 1) {
		xml = _T("modifypwdfailed1.xml");
	}
	if (reason = 2) {
		xml = _T("modifypwdok.xml");
	}
	RFDialog* pLoginFrame = new RFDialog(xml);
	pLoginFrame->Create(wnd, _T(""), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pLoginFrame->CenterWindow();
	pLoginFrame->ShowModal();
}

void RFSelectPatientDialog(HWND wnd)
{
	RFDialog* pLoginFrame = new RFDialog(_T("nopatientselect.xml"));
	pLoginFrame->Create(wnd, _T(""), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pLoginFrame->CenterWindow();
	pLoginFrame->ShowModal();
}

bool RFDeletePatientConfirmDialog(HWND wnd)
{
	RFDialog* pLoginFrame = new RFDialog(_T("deletepatientconfirm.xml"));
	pLoginFrame->Create(wnd, _T(""), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pLoginFrame->CenterWindow();
	pLoginFrame->ShowModal();

	return RFDialog::m_delete_patient_flag;
}

void RFBDAddActionDialog(HWND wnd)
{
	RFDialog* pLoginFrame = new RFDialog(_T("addaction.xml"));
	pLoginFrame->Create(wnd, _T(""), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pLoginFrame->CenterWindow();
	pLoginFrame->ShowModal();
}

bool RFEyeModeSettingDialog(HWND wnd, double& sudu, bool& show)
{
	RFDialog* pLoginFrame = new RFDialog(_T("eyemodeset.xml"));
	pLoginFrame->Create(wnd, _T(""), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pLoginFrame->CenterWindow();
	pLoginFrame->ShowModal();

	sudu = RFDialog::m_eyemode_sudu;
	show = RFDialog::m_eyemode_close;

	return RFDialog::m_eyemode_ok;
}
