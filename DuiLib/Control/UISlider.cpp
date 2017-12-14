#include "StdAfx.h"
#include "UISlider.h"

namespace DuiLib
{
	CSliderUI::CSliderUI() : m_uButtonState(0), m_nStep(1),m_bSendMove(false)
	{
		m_uTextStyle = DT_SINGLELINE | DT_CENTER;
		m_szThumb.cx = m_szThumb.cy = 10;
	}

	LPCTSTR CSliderUI::GetClass() const
	{
		return _T("SliderUI");
	}

	UINT CSliderUI::GetControlFlags() const
	{
		if( IsEnabled() ) return UIFLAG_SETCURSOR;
		else return 0;
	}

	LPVOID CSliderUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_SLIDER) == 0 ) return static_cast<CSliderUI*>(this);
		return CProgressUI::GetInterface(pstrName);
	}

	void CSliderUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			m_uButtonState = 0;
		}
	}

	int CSliderUI::GetChangeStep()
	{
		return m_nStep;
	}

	void CSliderUI::SetChangeStep(int step)
	{
		m_nStep = step;
	}

	void CSliderUI::SetThumbSize(SIZE szXY)
	{
		m_szThumb = szXY;
	}

	RECT CSliderUI::GetThumbRect() const
	{
		if( m_bHorizontal ) {
			int left = m_rcItem.left + (m_rcItem.right - m_rcItem.left - m_szThumb.cx) * (m_nValue - m_nMin) / (m_nMax - m_nMin);
			int top = (m_rcItem.bottom + m_rcItem.top - m_szThumb.cy) / 2;
			return CDuiRect(left, top, left + m_szThumb.cx, top + m_szThumb.cy); 
		}
		else {
			int left = (m_rcItem.right + m_rcItem.left - m_szThumb.cx) / 2;
			int top = m_rcItem.bottom - m_szThumb.cy - (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy) * (m_nValue - m_nMin) / (m_nMax - m_nMin);
			return CDuiRect(left, top, left + m_szThumb.cx, top + m_szThumb.cy); 
		}
	}

	LPCTSTR CSliderUI::GetThumbImage() const
	{
		return m_thumbImage.GetAttributeString();
	}

	void CSliderUI::SetThumbImage(LPCTSTR pStrImage)
	{
		m_thumbImage.SetAttributeString(pStrImage);
		Invalidate();
	}

	LPCTSTR CSliderUI::GetThumbHotImage() const
	{
		return m_thumbHotImage.GetAttributeString();
	}

	void CSliderUI::SetThumbHotImage(LPCTSTR pStrImage)
	{
		m_thumbHotImage.SetAttributeString(pStrImage);
		Invalidate();
	}

	LPCTSTR CSliderUI::GetThumbPushedImage() const
	{
		return m_thumbPushedImage.GetAttributeString();
	}

	void CSliderUI::SetThumbPushedImage(LPCTSTR pStrImage)
	{
		m_thumbPushedImage.SetAttributeString(pStrImage);
		Invalidate();
	}


	void CSliderUI::SetForeWidth(DWORD dwWidth)
	{
		m_dwForeWidth = dwWidth;
	}

	void CSliderUI::SetBkWidth(DWORD dwWidth)
	{
		m_dwBkWidth = dwWidth;
	}

	void CSliderUI::SetBkHeight(DWORD dwHeight)
	{
		m_dwBkHeight = dwHeight;
	}

	void CSliderUI::SetThumbWidth(DWORD dwWidth)
	{
		m_dwThumbWidth = dwWidth;
	}

	void CSliderUI::SetThumbHeight(DWORD dwHeight)
	{
		m_dwThumbHeight = dwHeight;
	}

	void CSliderUI::SetValue(int nValue) //2014.7.28 redrain  当鼠标正在滑动滑块时不会收到SetValue的影响，比如滑动改变音乐的进度，不会因为外部一直调用SetValue而让我们无法滑动滑块
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) 
			return;
		CProgressUI::SetValue(nValue);
	}
	void CSliderUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CProgressUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
		{
			if( IsEnabled() ) {//2014.7.28 redrain 注释掉原来的代码，加上这些代码后可以让Slider不是在鼠标弹起时才改变滑块的位置
				m_uButtonState |= UISTATE_CAPTURED;

				int nValue;

				if( m_bHorizontal ) {
					if( event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2 ) nValue = m_nMax;
					else if( event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2 ) nValue = m_nMin;
					else nValue = m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2 ) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx);
				}
				else {
					if( event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2 ) nValue = m_nMin;
					else if( event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2  ) nValue = m_nMax;
					else nValue = m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2 ) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy);
				}
				if(m_nValue !=nValue && nValue>=m_nMin && nValue<=m_nMax)
				{
					m_nValue =nValue;
					Invalidate();
				}
			}
				return;
		}

		if( event.Type == UIEVENT_BUTTONUP )
		{
			int nValue;
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				m_uButtonState &= ~UISTATE_CAPTURED;
			}
			if( m_bHorizontal ) {
				if( event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2 ) nValue = m_nMax;
				else if( event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2 ) nValue = m_nMin;
				else nValue = m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2 ) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx);
			}
			else {
				if( event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2 ) nValue = m_nMin;
				else if( event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2  ) nValue = m_nMax;
				else nValue = m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2 ) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy);
			}
			if(/*m_nValue !=nValue && 2014.7.28 redrain 这个注释很关键，是他导致了鼠标拖动滑块无法发出DUI_MSGTYPE_VALUECHANGED消息*/nValue>=m_nMin && nValue<=m_nMax)
			{
				m_nValue =nValue;
				m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			return;
		}
		if( event.Type == UIEVENT_SCROLLWHEEL ) 
		{
			switch( LOWORD(event.wParam) ) {
		case SB_LINEUP:
			SetValue(GetValue() + GetChangeStep());
			m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
			return;
		case SB_LINEDOWN:
			SetValue(GetValue() - GetChangeStep());
			m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
			return;
			}
		}
		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {//2014.7.28 redrain 重写这个消息判断让Slider发出DUI_MSGTYPE_VALUECHANGED_MOVE消息，让他在滑动过程也发出消息，比如用在改变音量时，一边滑动就可以一边改变音量
				if( m_bHorizontal ) {
					if( event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2 ) m_nValue = m_nMax;
					else if( event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2 ) m_nValue = m_nMin;
					else m_nValue = m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2 ) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx);
				}
				else {
					if( event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2 ) m_nValue = m_nMin;
					else if( event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2  ) m_nValue = m_nMax;
					else m_nValue = m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2 ) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy);
				}
				if (m_bSendMove)
					m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED_MOVE);
				Invalidate();
			}

			// Generate the appropriate mouse messages
			POINT pt = event.ptMouse;
			RECT rcThumb = GetThumbRect();
			if( IsEnabled() && ::PtInRect(&rcThumb, event.ptMouse) ) {

				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}else
			{
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_SETCURSOR )
		{
			RECT rcThumb = GetThumbRect();
			if( IsEnabled()) {
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
				return;
			}
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{

		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled() ) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		CControlUI::DoEvent(event);
	}

	void CSliderUI::SetCanSendMove(bool bCanSend) //2014.7.28 redrain
	{
		m_bSendMove = bCanSend;
	}
	bool CSliderUI::GetCanSendMove() const //2014.7.28 redrain
	{
		return m_bSendMove;
	}

	void CSliderUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("thumbimage")) == 0 ) SetThumbImage(pstrValue);
		else if( _tcscmp(pstrName, _T("thumbhotimage")) == 0 ) SetThumbHotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("thumbpushedimage")) == 0 ) SetThumbPushedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("thumbsize")) == 0 ) {
			SIZE szXY = {0};
			LPTSTR pstr = NULL;
			szXY.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			szXY.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
			SetThumbSize(szXY);
		}
		else if( _tcscmp(pstrName, _T("step")) == 0 ) {
			SetChangeStep(_ttoi(pstrValue));
		}
		else if( _tcscmp(pstrName, _T("sendmove")) == 0 ) {
			SetCanSendMove(_tcscmp(pstrValue, _T("true")) == 0);
		}
		else if(_tcscmp(pstrName, _T("bkheight")) == 0) SetBkHeight(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("bkwidth")) == 0 ) SetBkWidth(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("forewidth")) == 0 ) SetForeWidth(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("thumbwidth")) == 0 ) SetThumbWidth(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("thumbheight")) == 0 ) SetThumbHeight(_ttoi(pstrValue));
		else CProgressUI::SetAttribute(pstrName, pstrValue);
	}

	void CSliderUI::PaintStatusImage(HDC hDC)
	{
		
	}

	void CSliderUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;

		// 绘制循序：背景颜色->背景图->状态图->文本->边框
		if( m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0 ) {
			CRenderClip roundClip;
			CRenderClip::GenerateRoundClip(hDC, m_rcPaint,  m_rcItem, m_cxyBorderRound.cx, m_cxyBorderRound.cy, roundClip);
			PaintBkImage(hDC);
			PaintForeImage(hDC);
			PaintThumbImage(hDC);
		}
		else {
			PaintBkImage(hDC);
			PaintForeImage(hDC);
			PaintThumbImage(hDC);
		}
	}

	void CSliderUI::PaintBkImage(HDC hDC)
	{
		if (!m_bkImage.LoadImage(m_pManager))
			return;

		RECT rcDest;

		if (m_bHorizontal) {
			rcDest.left = m_rcItem.left + (m_rcItem.right - m_rcItem.left - m_dwBkWidth) / 2;
			rcDest.right = rcDest.left + m_dwBkWidth;
			rcDest.top =  m_rcItem.top + (m_rcItem.bottom - m_rcItem.top - m_dwBkHeight) / 2 ;
			rcDest.bottom = m_rcItem.bottom -  (m_rcItem.bottom - m_rcItem.top - m_dwBkHeight) / 2;
		} else {
			rcDest.left = m_rcItem.left + (m_rcItem.right - m_rcItem.left - m_dwBkWidth) / 2;
			rcDest.right = rcDest.left + m_dwBkWidth;
			rcDest.top = m_rcItem.top;
			rcDest.bottom = m_rcItem.bottom;
		}

		CRenderEngine::DrawImage(hDC, m_pManager, rcDest, m_rcPaint, m_bkImage);
	}

	void CSliderUI::PaintForeImage(HDC hDC)
	{
		if (!m_foreImage.LoadImage(m_pManager))
			return;

		RECT rcDest;

		if (m_bHorizontal) {
			rcDest.left = m_rcItem.left + (m_rcItem.right - m_rcItem.left - m_dwBkWidth) / 2;
			rcDest.right = rcDest.left + (m_dwBkWidth* GetValue()) / GetMaxValue();
			rcDest.top =  m_rcItem.top + (m_rcItem.bottom - m_rcItem.top - m_dwBkHeight) / 2 ;
			rcDest.bottom = m_rcItem.bottom -  (m_rcItem.bottom - m_rcItem.top - m_dwBkHeight) / 2;
		} else {
			rcDest.left = m_rcItem.left + (m_rcItem.right - m_rcItem.left - m_dwBkWidth) / 2;
			rcDest.right = rcDest.left + m_dwBkWidth;
			rcDest.top =  m_rcItem.bottom - ((m_rcItem.bottom - m_rcItem.top) * GetValue()) / GetMaxValue();
			rcDest.bottom = m_rcItem.bottom;
		}

		CRenderEngine::DrawImage(hDC, m_pManager, rcDest, m_rcPaint, m_foreImage);
	}

	void CSliderUI::PaintThumbImage(HDC hDC)
	{
		if (!m_thumbImage.LoadImage(m_pManager))
			return;
		RECT rcDest;

		if (m_bHorizontal) {
			rcDest.left = m_rcItem.left + (m_dwBkWidth * GetValue()) / GetMaxValue();// - m_dwThumbHeight / 2;
			rcDest.right = rcDest.left + m_dwThumbWidth;

			rcDest.top = m_rcItem.top + (m_rcItem.bottom - m_rcItem.top - m_dwThumbHeight) / 2;
			rcDest.bottom = rcDest.top + m_dwThumbHeight;
		} else {
			rcDest.left = m_rcItem.left + (m_rcItem.right - m_rcItem.left - m_dwThumbWidth) / 2;
			rcDest.right = rcDest.left + m_dwThumbWidth;

			rcDest.top = m_rcItem.bottom - ((m_rcItem.bottom - m_rcItem.top) * GetValue()) / GetMaxValue() - m_dwThumbHeight / 2;
			rcDest.bottom = m_rcItem.bottom - ((m_rcItem.bottom - m_rcItem.top) * GetValue()) / GetMaxValue() + m_dwThumbHeight / 2;

		}


		CRenderEngine::DrawImage(hDC, m_pManager, rcDest, m_rcPaint, m_thumbImage);
	}



}
