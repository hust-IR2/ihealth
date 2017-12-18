#include "RFRobotEventControl.h"
#include "RFMainWindow.h"

unsigned int __stdcall EventControl(PVOID pParam)
{
	RFRobotEventControl *robotEvent = (RFRobotEventControl*)pParam;

	while (true) {
		if (!robotEvent->m_bStart) {
			break;
		}

		bool bFire = RFMainWindow::MainWindow->m_robot.isFire();
		
		double XY[2];
		RFMainWindow::MainWindow->m_robot.getPlanePos(robotEvent->m_nWidth, robotEvent->m_nHeight, XY);

		robotEvent->m_fX = XY[0];
		robotEvent->m_fY = XY[1];
		robotEvent->m_isFire = bFire;
		robotEvent->Push();
	}

	return 0;
}

RFRobotEventControl::RFRobotEventControl(void)
{
	m_hEventControl = 0;
	m_bStart = false;

	m_isFire = false;
	m_fX = .0f;
	m_fY = .0f;

	m_haveValue = false;
	m_nWidth = 0;
	m_nHeight = 0;
}

RFRobotEventControl::~RFRobotEventControl(void)
{
}

bool RFRobotEventControl::Empty()
{
	return !m_haveValue;
}

void RFRobotEventControl::GetValue(bool& bFire, int &X, int &Y)
{
	bFire = m_isFire;
	X = m_fX;
	Y = m_fY;

	Pop();
}

void RFRobotEventControl::Pop()
{
	m_haveValue = false;
}

void RFRobotEventControl::Push()
{
	m_haveValue = true;
}

void RFRobotEventControl::Start(int width, int height)
{
	m_isFire = false;
	m_fX = .0f;
	m_fY = .0f;

	m_nWidth = width;
	m_nHeight = height;

	m_haveValue = false;
	m_bStart = true;
	m_hEventControl = (HANDLE)_beginthreadex(NULL, 0, EventControl, this, 0, NULL);
}

void RFRobotEventControl::Stop()
{
	if (m_bStart && m_hEventControl != 0) {
		m_bStart = false;

		::WaitForSingleObject(m_hEventControl, INFINITE);
	}

	m_haveValue = false;
	m_isFire = false;
	m_fX = .0f;
	m_fY = .0f;
	m_bStart = false;
}
