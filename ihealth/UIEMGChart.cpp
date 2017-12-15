#include "StdAfx.h"
#include "UIEMGChart.h"


CEMGWaveUI::CEMGWaveUI(void) 
{ 
	m_xMaxValue = 10 * 1000;
	m_nLineType = 0;
	m_nStartXValue = 0;
};
CEMGWaveUI::~CEMGWaveUI()
{

}

LPCTSTR CEMGWaveUI::GetClass() const
{
	return _T("EMGWaveUI");
}

LPVOID CEMGWaveUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("EMGLineWave")) == 0 ) return static_cast<CEMGWaveUI*>(this);
	return CControlUI::GetInterface(pstrName);
}


void CEMGWaveUI::SetLineColor(DWORD dwColor) 
{ 
	m_dwLineColor = dwColor;
};

void CEMGWaveUI::SetLineSize(DWORD dwSize)
{
	m_dwLineSize = dwSize;
}

void CEMGWaveUI::SetMinValue(double nValue)
{
	m_nMinValue = nValue;
}

void CEMGWaveUI::SetMaxValue(double nValue)
{
	m_nMaxValue = nValue;
}

void CEMGWaveUI::SetLineType(int nValue) 
{
	m_nLineType = nValue;
}

void CEMGWaveUI::SetStartX(int x)
{
	m_nStartXValue = x;
	Invalidate();
};


void CEMGWaveUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) 
{
	if( _tcscmp(pstrName, _T("linecolor")) == 0 ) {
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetLineColor(clrColor);
	} else if( _tcscmp(pstrName, _T("linesize")) == 0 ) SetLineSize(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("linetype")) == 0 ) SetLineType(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("min")) == 0 ) SetMinValue(_tstof(pstrValue));
	else if( _tcscmp(pstrName, _T("max")) == 0 ) SetMaxValue(_tstof(pstrValue));
	else CControlUI::SetAttribute(pstrName, pstrValue);
}

void CEMGWaveUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
	CControlUI::DoPaint(hDC, rcPaint);
	PaintLine(hDC);
}

void CEMGWaveUI::PaintLine(HDC hDC)
{
	Gdiplus::Point* pPoints = NULL;

	std::list<EMGLineWaveData> waves = GetCurrentData(); 
	int num = waves.size();
	if (num < 2) {
		return;
	}

	pPoints = new Gdiplus::Point[num];

	int width = m_rcItem.right - m_rcItem.left;
	int height = m_rcItem.bottom - m_rcItem.top;

	int nXStartValue = GetXStartValue();
	int i = 0;
	std::list<EMGLineWaveData>::iterator begin = waves.begin();
	for (; begin != waves.end(); begin++) {
		EMGLineWaveData data = *begin;
		int x = m_rcItem.left + width * (data.x - nXStartValue) / m_xMaxValue;
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
}

int CEMGWaveUI::GetXStartValue()
{
	int nMaxXValue = GetXEndValue();
	if (nMaxXValue > m_xMaxValue) {
		return nMaxXValue - m_xMaxValue;
	}
	return 0;
}

int CEMGWaveUI::GetXEndValue()
{
	int nMaxXValue = 0;
	std::list<EMGLineWaveData>::iterator begin = m_lstDatas.begin();
	for (; begin != m_lstDatas.end(); begin++) {
		nMaxXValue = nMaxXValue > begin->x ? nMaxXValue : begin->x;
	}

	return nMaxXValue;
}

std::list<EMGLineWaveData> CEMGWaveUI::GetCurrentData()
{
	std::list<EMGLineWaveData> waves;

	int nStart = GetXStartValue();
	int nEnd = GetXEndValue();
	std::list<EMGLineWaveData>::iterator begin = m_lstDatas.begin();
	for (; begin != m_lstDatas.end(); begin++) {
		if (begin->x >= nStart && begin->x <= nEnd) {
			waves.push_back(*begin);
		}
	}

	return waves;
}

void CEMGWaveUI::ClearData()
{
	m_lstDatas.clear();
	Invalidate();
}

void CEMGWaveUI::PushData(EMGLineWaveData data)
{
	m_lstDatas.push_back(data);
	Invalidate();
}

void CEMGWaveUI::PushData(std::list<EMGLineWaveData>& waves)
{
	m_lstDatas = waves;

	Invalidate();
}