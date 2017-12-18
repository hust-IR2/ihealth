#include "StdAfx.h"
#include "UIAVPlayer.h"
#include <uuids.h>
#include "RFMainWindow.h"
#include "RFCharSet.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE( x )if ( NULL != x ){x->Release();x = NULL;}
#endif

CAVPlayerUI::CAVPlayerUI(void) : m_hBitmap(NULL)
{
	m_hBitmap = NULL;
	m_pixels = NULL;
	m_leftEye = false;
	m_isPlaying = false;
}

CAVPlayerUI::~CAVPlayerUI(void)
{
	StopVideo();
}

LPCTSTR CAVPlayerUI::GetClass() const
{
	return _T("AVPlayerUI");
}


LPVOID CAVPlayerUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("AVPlayer")) == 0 ) return static_cast<CAVPlayerUI*>(this);
	return CControlUI::GetInterface(pstrName);
}

void CAVPlayerUI::DoInit()
{
	HWND hWnd = ::CreateWindowEx(0, _T("#32770"), _T("AVMedia"), UI_WNDSTYLE_CHILD,
		0, 0, 0, 0, GetManager()->GetPaintWindow(), (HMENU)0, NULL, NULL);


	if (::IsWindow(hWnd))
		m_hWnd = hWnd;
	else
		delete this;
}

void CAVPlayerUI::SetVisible(bool bVisible /*= true*/)
{
	__super::SetVisible(bVisible);
	::ShowWindow(m_hWnd, bVisible);

	if (bVisible) {
		PlayVideo();
	} else {
		StopVideo();
	}
}

void CAVPlayerUI::ShowVideo(bool bVisible)
{
	::ShowWindow(m_hWnd, bVisible);
}

void CAVPlayerUI::SetInternVisible(bool bVisible /*= true*/)
{
	__super::SetInternVisible(bVisible);
	::ShowWindow(m_hWnd, bVisible);

	if (bVisible) {
		PlayVideo();
	} else {
		StopVideo();
	}
}

void CAVPlayerUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	CControlUI::SetAttribute(pstrName, pstrValue);

	if( _tcscmp(pstrName, _T("eye")) == 0 ) {
		SetEye(_tcscmp(pstrValue, _T("left")) == 0);
	}
}

void CAVPlayerUI::SetPos(RECT rc)
{
	__super::SetPos(rc);
	::SetWindowPos(m_hWnd, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);

}

void CAVPlayerUI::SetEye(bool left)
{
	m_leftEye = left;
}

void CAVPlayerUI::PlayVideo()
{
	if (m_pManager) {
		m_isPlaying = true;
		m_pManager->SetTimer( this, 101, 60U);
	}
}

void CAVPlayerUI::StopVideo()
{
	if (m_pManager) {
		m_isPlaying = false;
		m_pManager->KillTimer(this, 101);
	}
}

void CAVPlayerUI::DoEvent( TEventUI& event )
{
	if( event.Type == UIEVENT_TIMER && (UINT_PTR)event.wParam == 101 && m_isPlaying) {
		int width = m_rcItem.right - m_rcItem.left;
		int height = m_rcItem.bottom - m_rcItem.top;
		
		unsigned char* pBuffer = new unsigned char[width * height * 3];

		if (m_leftEye) {
			RFMainWindow::MainWindow->m_robot.getLeftRGB24(pBuffer, width, height);
		} else {
			RFMainWindow::MainWindow->m_robot.getRightRGB24(pBuffer, width, height);
		}
		
		HBITMAP hBitmap;
		unsigned char *pixels = NULL;
		BITMAPINFO bmi;
		ZeroMemory(&bmi, sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = width;
		bmi.bmiHeader.biHeight = height;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biSizeImage = width * height * 4;
		bmi.bmiHeader.biCompression = BI_RGB;
		if (!m_hBitmap) {
			hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void **)&pixels, NULL, 0);
			m_pixels = pixels;
			m_hBitmap = hBitmap;
		} else {
			pixels = m_pixels;
			hBitmap = m_hBitmap;
		}

		//YUY22RGB(pBuffer, pixels, m_lWidth, m_lHeight);
		for (int i = 0; i < width * height; i++) {
			pixels[i * 4] = pBuffer[i * 3];
			pixels[i * 4 + 1] = pBuffer[i * 3 + 1];
			pixels[i * 4 + 2] = pBuffer[i * 3 + 2];
			pixels[i * 4 + 3] = 0xFF;
		}

		//YUY2_RGB3(pBuffer, pixels, m_lWidth*m_lHeight);
		BITMAP bitmap; 
		::GetObject(hBitmap, sizeof(BITMAP), &bitmap); 
		HDC hdc = ::GetDC(m_hWnd);  
		HDC hdcMem = ::CreateCompatibleDC(hdc);  
		::SelectObject(hdcMem, hBitmap);  
		::SetStretchBltMode (hdc, HALFTONE); 
		::SetBrushOrgEx(hdc, 0, 0, NULL); 
		::StretchBlt(hdc, 0, 0, m_rcItem.right - m_rcItem.left, 
			m_rcItem.bottom - m_rcItem.top, hdcMem,
			0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);

		//::DeleteObject(hBitmap);
		::DeleteDC(hdcMem);  
		::ReleaseDC(m_hWnd, hdc); 

		delete pBuffer;
		pBuffer = NULL;
	}
}
