#pragma once
//#include <wtypes.h>

class RFRobotEventControl
{
public:
	RFRobotEventControl(void);
	~RFRobotEventControl(void);


	bool Empty();
	void GetValue(bool& bFire, int &X, int &Y);

	void Start(int width, int height);
	void Stop();

	void Pop();
	void Push();


	bool   m_haveValue;
	bool   m_isFire;
	double m_fX;
	double m_fY;

	int		m_nWidth;
	int		m_nHeight;

	bool   m_bStart;
	void* m_hEventControl;
};
