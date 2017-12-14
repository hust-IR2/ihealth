#pragma once

class RFDialog : public CWindowWnd, public INotifyUI, public IMessageFilterUI
{
public:
	RFDialog(std::wstring skin);

	LPCTSTR GetWindowClassName() const { return _T("UILoginFrame"); };
	UINT GetClassStyle()  { return UI_CLASSSTYLE_DIALOG; };
	void OnFinalMessage(HWND /*hWnd*/) {m_pm.RemovePreMessageFilter(this);delete this; };
	void Init();
	void Notify(TNotifyUI& msg);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	bool OnReLogin(void *pParam);
	bool OnSelectNow(void *pParam);
	bool OnReMofidyPwd(void *pParam);
	bool OnCancelClose(void *pParam);
	bool OnOKClose(void *pParam);

	bool OnEyeModeCancelClose(void *pParam);
	bool OnEyeModeOKClose(void *pParam);
	

	bool OnDeletePatientOK(void *pParam);
	bool OnDeletePatientCancel(void *pParam);

	bool OnEyeModeJian(void *pParam);
	bool OnEyeModeJia(void *pParam);

	bool OnRecordPasvTrain(void *pParam);
	bool OnBiaoding(void *pParam);

	static bool	m_delete_patient_flag;
	CPaintManagerUI m_pm;
	std::wstring m_skin;

	static bool	  m_eyemode_ok;
	static double m_eyemode_sudu;
	static bool	  m_eyemode_close;

	static RFDialog* Dialog;
	int			 m_tickcount;
};

void RFConnectFailDialog(HWND wnd);
void RFLoginFailDialog(HWND wnd);
void RFModifyPwdFailedDialog(HWND wnd, int reason);
void RFSelectPatientDialog(HWND wnd);

bool RFDeletePatientConfirmDialog(HWND wnd);

void RFBDAddActionDialog(HWND wnd);
bool RFEyeModeSettingDialog(HWND wnd, double& sudu, bool& show);