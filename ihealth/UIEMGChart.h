#pragma once


struct EMGLineWaveData
{
	int x;				// 单位为ms
	double y;			// 根据实际情况
};

class CEMGWaveUI : public CControlUI
{
public:
	CEMGWaveUI(void);
	~CEMGWaveUI(void);

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void SetLineColor(DWORD dwColor);
	void SetLineSize(DWORD dwSize);
	void SetMinValue(double nValue);
	void SetMaxValue(double nValue);
	void SetLineType(int nValue);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void DoPaint(HDC hDC, const RECT& rcPaint);

	void PaintLine(HDC hDC);

	void SetStartX(int x);
	int	 GetXStartValue();
	int  GetXEndValue();

	std::list<EMGLineWaveData> GetCurrentData();
	void ClearData();
	void PushData(EMGLineWaveData data);
	void PushData(std::list<EMGLineWaveData>& waves);

	int		m_nStartXValue;
	double	m_nMinValue;
	double	m_nMaxValue;
	DWORD	m_xMaxValue;
	DWORD	m_dwLineColor;
	DWORD	m_dwLineSize;
	int		m_nLineType;		// 0 1
	std::list<EMGLineWaveData> m_lstDatas;
	int						m_timespace;
};
