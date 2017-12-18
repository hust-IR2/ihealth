#pragma once
#include <queue>


struct LineWaveData
{
	int x;				// 单位为ms
	double y;			// 根据实际情况
};

class CWaveUI : public CControlUI
{
public:
	CWaveUI(void);
	~CWaveUI(void);

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void SetLineColor(DWORD dwColor);
	void SetLineColor1(DWORD dwColor);
	void SetLineColor2(DWORD dwColor);
	void SetLineColor3(DWORD dwColor);
	void SetLineSize(DWORD dwSize);
	void SetMinValue(double nValue);
	void SetMaxValue(double nValue);
	void SetLineType(int nValue);
	void SetLineNum(int nNum);
	void SetScroll(std::wstring scrollname);

	void SetTimeSpace(int nValue);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void DoPaint(HDC hDC, const RECT& rcPaint);

	void PaintLine(HDC hDC);

	void SetStartX(int x);

	int GetXStartValue();
	int GetXEndValue();

	std::list<LineWaveData> GetCurrentData();
	void ClearData();
	void PushData(LineWaveData data);
	void PushData(std::list<LineWaveData>& waves);

	int		m_nStartXValue;
	double	m_nMinValue;
	double	m_nMaxValue;
	DWORD	m_xMaxValue;
	DWORD	m_dwLineColor;
	DWORD	m_dwLineColor1;
	DWORD	m_dwLineColor2;
	DWORD	m_dwLineColor3;
	DWORD	m_dwLineSize;
	int		m_nLineType;		// 0 1
	int		m_nLineNum;
	std::list<LineWaveData> m_lstDatas;
	std::wstring			m_scrollname;
	CScrollBarUI*			m_scrollbar;
	int						m_timespace;
};
