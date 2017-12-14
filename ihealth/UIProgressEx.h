#pragma once

class CProgressExUI : public CControlUI
{
public:
	CProgressExUI(void);
	~CProgressExUI(void);

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void	SetProgressBkImage(LPCTSTR pstrName);
	void	SetProgressForeImage(LPCTSTR pstrName);
	void	SetProgressCurImage(LPCTSTR pstrName);

	void	SetProgressTotal(DWORD dwTotal);
	void	SetProgressCurrent(DWORD dwCurrent);

	void	SetProgressHeight(DWORD height);
	void	SetProgressCurrentHight(DWORD dwHeight);
	void	SetProgressCurrentWidth(DWORD dwWidth);

	void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void DoPaint(HDC hDC, const RECT& rcPaint);

	void PaintProgressBkImage(HDC hDC);
	void PaintProgressForeImage(HDC hDC);
	void PaintProgressCurImage(HDC hDC);

	CImageAttribute m_progressBkImage;
	CImageAttribute m_progressForeImage;
	CImageAttribute m_progressCurImage;

	DWORD m_dwTotal;
	DWORD m_dwCurrent;
	DWORD m_dwProgressHeight;
	DWORD m_dwCurrentHeight;
	DWORD m_dwCurrentWidth;
};
