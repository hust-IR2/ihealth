#pragma once
#include <control.h>
#include <Strmif.h>
#include <Dshow.h>
#pragma include_alias( "dxtrans.h", "qedit.h" )

#define __IDxtCompositor_INTERFACE_DEFINED__

#define __IDxtAlphaSetter_INTERFACE_DEFINED__

#define __IDxtJpeg_INTERFACE_DEFINED__

#define __IDxtKey_INTERFACE_DEFINED__
//#include <qedit.h>

class CAVPlayerUI : public CControlUI/*, public ISampleGrabberCB*/
{
public:
	CAVPlayerUI(void);
	~CAVPlayerUI(void);

	LPCTSTR	GetClass() const;
	LPVOID	GetInterface(LPCTSTR pstrName);

	//STDMETHODIMP_(ULONG) AddRef();
	//STDMETHODIMP_(ULONG) Release();
	//STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);
	//STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample );
	//STDMETHODIMP BufferCB( double dblSampleTime, BYTE * pBuffer, long lBufferSize );

	void DoInit();
	void SetVisible(bool bVisible = true);
	void SetInternVisible(bool bVisible = true);
	void SetPos(RECT rc);
	void SetEye(bool left);
	void PlayVideo();
	void StopVideo();
	void ShowVideo(bool bVisible);

	void	DoEvent(TEventUI& event) override;

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	
	HWND m_hWnd;         // 视频显示的窗口句柄
	HDC	 m_hPaintDC;
	bool			m_leftEye;
	HBITMAP			m_hBitmap;
	unsigned char*	m_pixels;

	bool m_isPlaying;
};


void YUY2_RGB3(BYTE *YUY2buff,BYTE *RGBbuff,DWORD w);
int YUV2RGB(void* pYUV, void* pRGB, int width, int height, bool alphaYUV, bool alphaRGB);