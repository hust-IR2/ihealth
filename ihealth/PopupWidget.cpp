#include "StdAfx.h"
#include "PopupWidget.h"
#include <MMSystem.h>
#include <Endpointvolume.h>
#include <Mmdeviceapi.h>

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);  
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator); 
const IID IID_IAudioEndpointVolume = __uuidof(IAudioEndpointVolume);

CPopupWidget::CPopupWidget() :m_MainWndpm(NULL),m_WidgetType(UNKNOWN),m_pBtnParent(NULL)
{
}

CPopupWidget::~CPopupWidget()
{
}


LPCTSTR CPopupWidget::GetWindowClassName() const 
{ 
	return _T("PopupWdiget");
}

UINT CPopupWidget::GetClassStyle() const 
{ 
	return CS_DBLCLKS;
}

void CPopupWidget::OnFinalMessage(HWND /*hWnd*/) 
{ 
	 delete this;
}

void CPopupWidget::Init( LPCTSTR pszXMLPath, HWND hWndParent, CPaintManagerUI* pMainPaintManager, POINT ptPos, WidgetType type)
{  
	m_strXMLPath = pszXMLPath;
	m_MainWndpm = pMainPaintManager;
	m_WidgetType = type;
	//m_pBtnParent = pBtnParent;
	Create(hWndParent, _T("PopupWdiget"), UI_WNDSTYLE_DIALOG, WS_EX_TOOLWINDOW);
	::ClientToScreen(hWndParent, &ptPos);
	//这里会计算应该显示的位置，好让自己出现在组合框上方
	::SetWindowPos(*this, NULL, ptPos.x, ptPos.y - m_pm.GetClientSize().cy , 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void CPopupWidget::InitWindow()
{
	m_pSlider = static_cast<CSliderUI*>(m_pm.FindControl(kWidgetSlider));

	if (m_pSlider)
	{
		if ( m_WidgetType == CHANGE_LIST_TRANS )
		{
			m_pSlider->SetMinValue(0);
			m_pSlider->SetMaxValue(255);
		//	m_pSlider->SetValue(m_pBtnParent->GetTag()* 100 / 255);
		}
		else if ( m_WidgetType == CHANGE_MAINWND_TRANS )
		{
			m_pSlider->SetMinValue(0);
			m_pSlider->SetMaxValue(150);
		//	m_pSlider->SetValue(m_pBtnParent->GetTag()* 100 / 150);
		}
		else if (m_WidgetType == CHANGE_VOLUME)
		{
			int nVolumeValue = GetSystemVolumeWin7();
			m_pSlider->SetMinValue(0);
			m_pSlider->SetMaxValue(100);
			m_pSlider->SetValue(nVolumeValue);
		//	m_pSlider->SetValue(m_pBtnParent->GetTag());
		}
		//m_pSlider->SetValue(m_pBtnParent->GetTag());
	}
}

void CPopupWidget::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("click") ) 
	{
		if( msg.pSender->GetName() == _T("closebtn")) {
			Close(IDOK);
			return; 
		}
	}
	else if ( msg.sType == _T("valuechanged") ||  msg.sType == _T("movevaluechanged"))
	{
		if ( msg.pSender == m_pSlider)
		{
			//修改皮肤设置窗体的组合框的文本显示值，把实际的值通过SetTag函数来保存
			CDuiString szValue;
			if ( m_WidgetType == CHANGE_LIST_TRANS )
			{
				SetSystemVolumeWin7(m_pSlider->GetValue());
			}
			else if ( m_WidgetType == CHANGE_MAINWND_TRANS )
			{
				SetSystemVolumeWin7(m_pSlider->GetValue());
			}
			else if ( m_WidgetType == CHANGE_VOLUME )
			{
				SetSystemVolumeWin7(m_pSlider->GetValue());
			}
			//设置SetTag函数好让主窗体知道是什么类型的小挂件窗体
			msg.pSender->SetTag((UINT_PTR)m_WidgetType);
			//通过主窗体的CPaintManager指针来发送消息到主窗体
			m_MainWndpm->SendNotify(msg, false);
		}
	}

}

LRESULT CPopupWidget::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	CDialogBuilder builder;

	CControlUI *pRoot = builder.Create(m_strXMLPath.GetData(), (UINT)0,  0, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	InitWindow();
	return 0;
}

LRESULT CPopupWidget::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CPopupWidget::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CPopupWidget::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CPopupWidget::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CPopupWidget::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CPopupWidget::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return HTCLIENT;
}


LRESULT CPopupWidget::OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	Close();
	return 0 ;
}

LRESULT CPopupWidget::OnMouseLeave( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	Close();
	return 0 ;
}


LRESULT CPopupWidget::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch( uMsg ) {
	case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
	case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
	case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
	case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
	case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
	case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
	case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
	case WM_KILLFOCUS:     lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break; 
	case WM_MOUSELEAVE:	   lRes = OnMouseLeave(uMsg, wParam, lParam, bHandled); break; 
	default:
		bHandled = FALSE;
	}
	if( bHandled ) return lRes;
	if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

int GetSystemVolume()
{
	MMRESULT          rc;     
	HMIXER            hMixer; 
	MIXERLINE         mxl;    
	MIXERLINECONTROLS mxlc;   
	MIXERCONTROL      mxc;    

	rc = mixerOpen(&hMixer, 0, 0, 0, 0);

	if (MMSYSERR_NOERROR == rc) {
		ZeroMemory(&mxl, sizeof(MIXERLINE));

		mxl.cbStruct = sizeof(MIXERLINE);
		mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;

		rc = mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);

		if (MMSYSERR_NOERROR == rc) {
			ZeroMemory(&mxc, sizeof(MIXERCONTROL));
			mxc.cbStruct = sizeof(mxc);

			ZeroMemory(&mxlc, sizeof(MIXERLINECONTROLS));
			mxlc.cbStruct = sizeof(mxlc);

			mxlc.dwLineID = mxl.dwLineID;
			mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
			mxlc.cControls = 1;          
			mxlc.pamxctrl = &mxc;        
			mxlc.cbmxctrl = sizeof(mxc); 

			rc = mixerGetLineControls((HMIXEROBJ)hMixer,
				&mxlc,
				MIXER_GETLINECONTROLSF_ONEBYTYPE);

			if (MMSYSERR_NOERROR == rc) {
				MIXERCONTROLDETAILS        mxcd;
				MIXERCONTROLDETAILS_SIGNED volStruct;

				ZeroMemory(&mxcd, sizeof(mxcd));

				mxcd.cbStruct = sizeof(mxcd);      
				mxcd.dwControlID = mxc.dwControlID;
				mxcd.paDetails = &volStruct;       
				mxcd.cbDetails = sizeof(volStruct);
				mxcd.cChannels = 1;                

				rc = mixerGetControlDetails((HMIXEROBJ)hMixer,
					&mxcd,
					MIXER_GETCONTROLDETAILSF_VALUE);

				if (MMSYSERR_NOERROR == rc) {
					return volStruct.lValue;
				}
			}
		}
	}

	mixerClose(hMixer);
	return 0;
}


int GetSystemVolumeWin7()
{
	float volumelevel = .0f;

	HRESULT hr = S_OK;
	IAudioEndpointVolume *volume = NULL;
	IMMDeviceEnumerator *enumerator = NULL;
	IMMDeviceCollection *collection = NULL;
	IPropertyStore *props = NULL;
	LPWSTR wsz_id = NULL;
	IMMDevice *endpoint = NULL;

	//CoInitialize(NULL);
	hr = CoCreateInstance(  //创建IMMDeviceEnumerator实例
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&enumerator);
	if (hr != S_OK) {
		goto END_GET_VOLUME;
	}

	hr = enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &endpoint);
	if (hr != S_OK) {
		goto END_GET_VOLUME;
	}

	hr = endpoint->GetId(&wsz_id);
	if (hr != S_OK) {
		goto END_GET_VOLUME;
	}

	hr=endpoint->Activate(IID_IAudioEndpointVolume, CLSCTX_ALL, NULL, (void **)(&volume));
	if (hr != S_OK) {
		goto END_GET_VOLUME;
	}

	hr = volume->GetMasterVolumeLevelScalar(&volumelevel); //获取音量值
	if (hr != S_OK) {
		goto END_GET_VOLUME;
	}
	// hr = volume_api->SetMasterVolumeLevelScalar(0.4,NULL); //设置音量值
	CoTaskMemFree(wsz_id);
	
END_GET_VOLUME:
	if (enumerator) {
		enumerator->Release();
		enumerator = NULL;
	}
	if (collection) {
		collection->Release();
		collection = NULL;
	}
	if (endpoint) {
		endpoint->Release();
		endpoint = NULL;
	}
	if (props) {
		props->Release();
		props = NULL;
	}
	
	return volumelevel*100;
}

void SetSystemVolumeWin7(int nVolume)
{
	HRESULT hr = S_OK;
	IAudioEndpointVolume *volume = NULL;
	IMMDeviceEnumerator *enumerator = NULL;
	IMMDeviceCollection *collection = NULL;
	IPropertyStore *props = NULL;
	LPWSTR wsz_id = NULL;
	IMMDevice *endpoint = NULL;

	//CoInitialize(NULL);
	hr = CoCreateInstance(  //创建IMMDeviceEnumerator实例
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&enumerator);
	if (hr != S_OK) {
		goto END_GET_VOLUME;
	}

	hr = enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &endpoint);
	if (hr != S_OK) {
		goto END_GET_VOLUME;
	}

	hr = endpoint->GetId(&wsz_id);
	if (hr != S_OK) {
		goto END_GET_VOLUME;
	}

	hr=endpoint->Activate(IID_IAudioEndpointVolume, CLSCTX_ALL, NULL, (void **)(&volume));
	if (hr != S_OK) {
		goto END_GET_VOLUME;
	}
	
	float volumelevel = (float)nVolume / 100.0f;
	hr = volume->SetMasterVolumeLevelScalar(volumelevel,NULL); //设置音量值
	if (hr != S_OK) {
		goto END_GET_VOLUME;
	}

	CoTaskMemFree(wsz_id);

END_GET_VOLUME:
	if (enumerator) {
		enumerator->Release();
		enumerator = NULL;
	}
	if (collection) {
		collection->Release();
		collection = NULL;
	}
	if (endpoint) {
		endpoint->Release();
		endpoint = NULL;
	}
	if (props) {
		props->Release();
		props = NULL;
	}
}

void SetSystemVolume(int nValue)
{
	MMRESULT          rc;     
	HMIXER            hMixer; 
	MIXERLINE         mxl;    
	MIXERLINECONTROLS mxlc;   
	MIXERCONTROL      mxc;    

	rc = mixerOpen(&hMixer, 0, 0, 0, 0);

	if (MMSYSERR_NOERROR == rc) {
		ZeroMemory(&mxl, sizeof(MIXERLINE));

		mxl.cbStruct = sizeof(MIXERLINE);
		mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;

		rc = mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);

		if (MMSYSERR_NOERROR == rc) {
			ZeroMemory(&mxc, sizeof(MIXERCONTROL));
			mxc.cbStruct = sizeof(mxc);

			ZeroMemory(&mxlc, sizeof(MIXERLINECONTROLS));
			mxlc.cbStruct = sizeof(mxlc);

			mxlc.dwLineID = mxl.dwLineID;
			mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
			mxlc.cControls = 1;          
			mxlc.pamxctrl = &mxc;        
			mxlc.cbmxctrl = sizeof(mxc); 

			rc = mixerGetLineControls((HMIXEROBJ)hMixer,
				&mxlc,
				MIXER_GETLINECONTROLSF_ONEBYTYPE);

			if (MMSYSERR_NOERROR == rc) {
				MIXERCONTROLDETAILS        mxcd;
				MIXERCONTROLDETAILS_SIGNED volStruct;

				ZeroMemory(&mxcd, sizeof(mxcd));

				mxcd.cbStruct = sizeof(mxcd);      
				mxcd.dwControlID = mxc.dwControlID;
				mxcd.paDetails = &volStruct;       
				mxcd.cbDetails = sizeof(volStruct);
				mxcd.cChannels = 1;                

				volStruct.lValue = nValue; 
				rc = mixerSetControlDetails((HMIXEROBJ)hMixer,
					&mxcd,
					MIXER_SETCONTROLDETAILSF_VALUE);
			}
		}
	}

	mixerClose(hMixer);
}