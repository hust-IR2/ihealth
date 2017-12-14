#include "StdAfx.h"
#include "UIChart.h"

CWaveUI::CWaveUI(void) 
{ 
	m_xMaxValue = 10 * 1000;
	m_nLineType = 0;
	m_nStartXValue = 0;
	m_scrollbar=NULL;
	m_nLineNum = 1;
};
CWaveUI::~CWaveUI()
{

}

LPCTSTR CWaveUI::GetClass() const
{
	 return _T("WaveUI");
}

LPVOID CWaveUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("LineWave")) == 0 ) return static_cast<CWaveUI*>(this);
	return CControlUI::GetInterface(pstrName);
}


void CWaveUI::SetLineColor(DWORD dwColor) 
{ 
	m_dwLineColor = dwColor;
}

void CWaveUI::SetLineColor1(DWORD dwColor) 
{ 
	m_dwLineColor1 = dwColor;
}

void CWaveUI::SetLineColor2(DWORD dwColor) 
{ 
	m_dwLineColor2 = dwColor;
}

void CWaveUI::SetLineColor3(DWORD dwColor) 
{ 
	m_dwLineColor3 = dwColor;
}

void CWaveUI::SetLineSize(DWORD dwSize)
{
	m_dwLineSize = dwSize;
}

void CWaveUI::SetMinValue(double nValue)
{
	m_nMinValue = nValue;
}

void CWaveUI::SetMaxValue(double nValue)
{
	m_nMaxValue = nValue;
}

void CWaveUI::SetLineType(int nValue) 
{
	m_nLineType = nValue;
}

void CWaveUI::SetLineNum(int nNum)
{
	m_nLineNum = nNum;
}

void CWaveUI::SetScroll(std::wstring scrollname) 
{
	m_scrollname = scrollname;
}

void CWaveUI::SetTimeSpace(int nValue) 
{
	m_timespace = nValue;
}

void CWaveUI::SetStartX(int x)
{
	m_nStartXValue = x;Invalidate();
};


void CWaveUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) 
{
	if( _tcscmp(pstrName, _T("linecolor")) == 0 ) {
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetLineColor(clrColor);
	} else if( _tcscmp(pstrName, _T("linecolor1")) == 0 ) {
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetLineColor1(clrColor);
	} else if( _tcscmp(pstrName, _T("linecolor2")) == 0 ) {
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetLineColor2(clrColor);
	} else if( _tcscmp(pstrName, _T("linecolor3")) == 0 ) {
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetLineColor3(clrColor);
	} else if( _tcscmp(pstrName, _T("linesize")) == 0 ) SetLineSize(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("linetype")) == 0 ) SetLineType(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("linenum")) == 0 ) SetLineNum(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("min")) == 0 ) SetMinValue(_tstof(pstrValue));
	else if( _tcscmp(pstrName, _T("max")) == 0 ) SetMaxValue(_tstof(pstrValue));
	else if(_tcscmp(pstrName, _T("scrollname")) == 0) SetScroll(pstrValue);
	else if(_tcscmp(pstrName, _T("timespace")) == 0) SetTimeSpace(_ttoi(pstrValue));
	else CControlUI::SetAttribute(pstrName, pstrValue);
}

void CWaveUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
	CControlUI::DoPaint(hDC, rcPaint);

	if (!m_scrollbar && !m_scrollname.empty()) {
		m_scrollbar = static_cast<CScrollBarUI*>(m_pManager->FindControl(m_scrollname.c_str()));
	}

	if (m_scrollbar && m_lstDatas.size() > 0) {
		std::list<LineWaveData>::iterator iter = m_lstDatas.begin();

		for (int i = 0; i < m_lstDatas.size() - 1; i++) {
			iter++;
		}

		if (iter->x <= m_xMaxValue) {
			m_scrollbar->SetScrollRange(0);
		} else {
			int cxNeeded = (double)(iter->x - m_xMaxValue) / (double)m_xMaxValue * (m_rcItem.right - m_rcItem.left);
			m_scrollbar->SetScrollRange(cxNeeded);
		}
	}

	PaintLine(hDC);
}

void CWaveUI::PaintLine(HDC hDC)
{
	Gdiplus::Point* pPoints = NULL;

	std::list<LineWaveData> waves = GetCurrentData(); 
	int num = waves.size();
	if (num < 4) {
		return;
	}
	
	if (m_nLineNum == 1) {
		pPoints = new Gdiplus::Point[num];

		int width = m_rcItem.right - m_rcItem.left;
		int height = m_rcItem.bottom - m_rcItem.top;

		int i = 0;
		std::list<LineWaveData>::iterator begin = waves.begin();
		for (; begin != waves.end(); begin++) {
			LineWaveData data = *begin;
			int x = m_rcItem.left + width * (data.x - GetXStartValue()) / m_xMaxValue;
			int y = m_rcItem.bottom - height * (data.y - m_nMinValue) / (m_nMaxValue - m_nMinValue);

			pPoints[i].X = x;
			pPoints[i].Y = y;
			i++;
		}

		Gdiplus::Graphics graphics(hDC);
		Gdiplus::Pen pen(Gdiplus::Color(GetAdjustColor(m_dwLineColor)), (Gdiplus::REAL)m_dwLineSize);
		pen.SetDashStyle(Gdiplus::DashStyleSolid);

		if (m_nLineType == 0) {
			graphics.DrawLines(&pen, pPoints, num);
		} else {
			graphics.DrawCurve(&pen, pPoints, num);
		}

		delete pPoints;
		pPoints = NULL;
	} else if (m_nLineNum == 2){
		std::list<LineWaveData> wave[2];
		
		int index = 0;
		std::list<LineWaveData>::iterator begin = waves.begin();
		for (; begin != waves.end(); begin++) {
			if (index % 2 == 0) {
				wave[0].push_back(*begin);
			} else {
				wave[1].push_back(*begin);
			}

			index++;
		}

		DWORD dwLineColor[2] = {m_dwLineColor, m_dwLineColor1};
		
		for (int i = 0; i < 2; i++) {
			std::list<LineWaveData> waveData = wave[i];
			int nNum = waveData.size();
			pPoints = new Gdiplus::Point[nNum];

			int width = m_rcItem.right - m_rcItem.left;
			int height = m_rcItem.bottom - m_rcItem.top;

			int pointi = 0; 
			std::list<LineWaveData>::iterator begin = waveData.begin();
			for (; begin != waveData.end(); begin++) {
				LineWaveData data = *begin;
				int x = m_rcItem.left + width * (data.x - GetXStartValue()) / m_xMaxValue;
				int y = m_rcItem.bottom - height * (data.y - m_nMinValue) / (m_nMaxValue - m_nMinValue);

				pPoints[pointi].X = x;
				pPoints[pointi].Y = y;
				pointi++;
			}

			Gdiplus::Graphics graphics(hDC);
			Gdiplus::Pen pen(Gdiplus::Color(GetAdjustColor(dwLineColor[i])), (Gdiplus::REAL)m_dwLineSize);
			pen.SetDashStyle(Gdiplus::DashStyleSolid);

			if (m_nLineType == 0) {
				graphics.DrawLines(&pen, pPoints, nNum);
			} else {
				graphics.DrawCurve(&pen, pPoints, nNum);
			}

			delete pPoints;
			pPoints = NULL;
		}
	} else if (m_nLineNum == 4) {
		std::list<LineWaveData> wave[4];

		int index = 0;
		std::list<LineWaveData>::iterator begin = waves.begin();
		for (; begin != waves.end(); begin++) {
			if (index % 4 == 0) {
				wave[0].push_back(*begin);
			} else if (index % 4 == 1) {
				wave[1].push_back(*begin);
			} else if (index % 4 == 2) {
				wave[2].push_back(*begin);
			}  else if (index % 4 == 3) {
				wave[3].push_back(*begin);
			}

			index++;
		}

		DWORD dwLineColor[4] = {m_dwLineColor, m_dwLineColor1, m_dwLineColor2, m_dwLineColor3};
		for (int i = 0; i < 4; i++) {
			int nNum = wave[i].size();
			pPoints = new Gdiplus::Point[nNum];

			int width = m_rcItem.right - m_rcItem.left;
			int height = m_rcItem.bottom - m_rcItem.top;

			int pointi = 0;
			std::list<LineWaveData>::iterator begin = wave[i].begin();
			for (; begin != wave[i].end(); begin++) {
				LineWaveData data = *begin;
				int x = m_rcItem.left + width * (data.x - GetXStartValue()) / m_xMaxValue;
				int y = m_rcItem.bottom - height * (data.y - m_nMinValue) / (m_nMaxValue - m_nMinValue);

				pPoints[pointi].X = x;
				pPoints[pointi].Y = y;
				pointi++;
			}

			Gdiplus::Graphics graphics(hDC);
			Gdiplus::Pen pen(Gdiplus::Color(GetAdjustColor(dwLineColor[i])), (Gdiplus::REAL)m_dwLineSize);
			pen.SetDashStyle(Gdiplus::DashStyleSolid);

			if (m_nLineType == 0) {
				graphics.DrawLines(&pen, pPoints, nNum);
			} else {
				graphics.DrawCurve(&pen, pPoints, nNum);
			}

			delete pPoints;
			pPoints = NULL;
		}
	}

}

int CWaveUI::GetXStartValue()
{
	int x = m_scrollbar->GetScrollPos();

	return x * m_xMaxValue / (m_rcItem.right - m_rcItem.left);
}

int CWaveUI::GetXEndValue()
{
	return GetXStartValue() + m_xMaxValue;
}

std::list<LineWaveData> CWaveUI::GetCurrentData()
{
	std::list<LineWaveData> waves;

	int nStart = GetXStartValue();
	int nEnd = GetXEndValue();
	std::list<LineWaveData>::iterator begin = m_lstDatas.begin();
	for (; begin != m_lstDatas.end(); begin++) {
		if (begin->x >= nStart && begin->x <= nEnd) {
			waves.push_back(*begin);
		}
	}

	return waves;
}

void CWaveUI::ClearData()
{
	m_lstDatas.clear();
}

void CWaveUI::PushData(LineWaveData data)
{
	m_lstDatas.push_back(data);
}

void CWaveUI::PushData(std::list<LineWaveData>& waves)
{
	m_lstDatas = waves;

	Invalidate();
}