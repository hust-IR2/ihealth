#include "StdAfx.h"
#include "UIProgressEx.h"

CProgressExUI::CProgressExUI(void)
{
}

CProgressExUI::~CProgressExUI(void)
{
}

LPCTSTR CProgressExUI::GetClass() const
{
	return _T("MusicProgressUI");
}

LPVOID CProgressExUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("MusicProgress")) == 0 ) return static_cast<CProgressExUI*>(this);
	return CControlUI::GetInterface(pstrName);
}

void	CProgressExUI::SetProgressBkImage(LPCTSTR pstrName)
{
	if( m_progressBkImage.GetAttributeString() == pstrName ) return;

	m_progressBkImage.SetAttributeString(pstrName);
	Invalidate();
}

void	CProgressExUI::SetProgressForeImage(LPCTSTR pstrName)
{
	if( m_progressForeImage.GetAttributeString() == pstrName ) return;

	m_progressForeImage.SetAttributeString(pstrName);
	Invalidate();
}

void	CProgressExUI::SetProgressCurImage(LPCTSTR pstrName)
{

	if( m_progressCurImage.GetAttributeString() == pstrName ) return;

	m_progressCurImage.SetAttributeString(pstrName);
	Invalidate();
}

void	CProgressExUI::SetProgressTotal(DWORD dwTotal)
{
	m_dwTotal = dwTotal;
}

void	CProgressExUI::SetProgressCurrent(DWORD dwCurrent)
{
	m_dwCurrent = dwCurrent;

	if (GetParent()) {
		GetParent()->Invalidate();
	} else {
		Invalidate();
	}
}

void	CProgressExUI::SetProgressHeight(DWORD height)
{
	m_dwProgressHeight = height;
}

void	CProgressExUI::SetProgressCurrentHight(DWORD dwHeight)
{
	m_dwCurrentHeight = dwHeight;
}

void CProgressExUI::SetProgressCurrentWidth(DWORD dwWidth)
{
	m_dwCurrentWidth = dwWidth;
}

void CProgressExUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("pbkimage")) == 0 ) SetProgressBkImage(pstrValue);
	else if( _tcscmp(pstrName, _T("pforeimage")) == 0 ) SetProgressForeImage(pstrValue);
	else if( _tcscmp(pstrName, _T("pcurimage")) == 0 ) SetProgressCurImage(pstrValue);
	else if( _tcscmp(pstrName, _T("ptotal")) == 0 ) SetProgressTotal(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("pcurrent")) == 0 ) SetProgressCurrent(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("pheight")) == 0 ) SetProgressHeight(_ttoi(pstrValue)); 
	else if( _tcscmp(pstrName, _T("pcurretheight")) == 0 ) SetProgressCurrentHight(_ttoi(pstrValue)); 
	else if( _tcscmp(pstrName, _T("pcurretwidth")) == 0 ) SetProgressCurrentWidth(_ttoi(pstrValue)); 
	else CControlUI::SetAttribute(pstrName, pstrValue);
}

void CProgressExUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
	CControlUI::DoPaint(hDC, rcPaint);

	PaintProgressBkImage(hDC);
	PaintProgressForeImage(hDC);
	PaintProgressCurImage(hDC);
}

void CProgressExUI::PaintProgressBkImage(HDC hDC)
{
	if (!m_progressBkImage.LoadImage(m_pManager))
		return;

	RECT rcDest;

	rcDest.left = m_rcItem.left;
	rcDest.right = m_rcItem.right;
	rcDest.top = m_rcItem.top + (m_rcItem.bottom - m_rcItem.top - m_dwProgressHeight) / 2;
	rcDest.bottom = rcDest.top + m_dwProgressHeight;

	CRenderEngine::DrawImage(hDC, m_pManager, rcDest, m_rcPaint, m_progressBkImage);
	//CControlUI::DrawImage(hDC, m_progressBkImage, rcDest);
}
void CProgressExUI::PaintProgressForeImage(HDC hDC)
{
	if (!m_progressForeImage.LoadImage(m_pManager))
		return;

	RECT rcDest;

	rcDest.left = m_rcItem.left;
	rcDest.right = m_rcItem.left + ((m_rcItem.right - m_rcItem.left) * m_dwCurrent) / m_dwTotal;
	rcDest.top = m_rcItem.top + (m_rcItem.bottom - m_rcItem.top - m_dwProgressHeight) / 2;
	rcDest.bottom = rcDest.top + m_dwProgressHeight;


	CRenderEngine::DrawImage(hDC, m_pManager, rcDest, m_rcPaint, m_progressForeImage);
	//CControlUI::DrawImage(hDC, m_progressForeImage, rcDest);
}
void CProgressExUI::PaintProgressCurImage(HDC hDC)
{
	if (!m_progressCurImage.LoadImage(m_pManager))
		return;
	RECT rcDest;

	rcDest.left = m_rcItem.left + ((m_rcItem.right - m_rcItem.left) * m_dwCurrent) / m_dwTotal - m_dwCurrentWidth / 2;
	rcDest.right = m_rcItem.left + ((m_rcItem.right - m_rcItem.left) * m_dwCurrent) / m_dwTotal + m_dwCurrentWidth / 2;
	rcDest.top = m_rcItem.top + (m_rcItem.bottom - m_rcItem.top - m_dwCurrentHeight) / 2;
	rcDest.bottom = rcDest.top + m_dwCurrentHeight;


	CRenderEngine::DrawImage(hDC, m_pManager, rcDest, m_rcPaint, m_progressCurImage);
	//CControlUI::DrawImage(hDC, m_progressCurImage, rcDest);
}