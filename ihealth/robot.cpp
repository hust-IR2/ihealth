#include "robot.h"

#include <windows.h>
#include <mmsystem.h>

#include "Log.h"

#pragma comment(lib,"winmm.lib")
#define RESET_TIMER 100
MMRESULT Mtimer_ID=0;
UINT wAccuracy=0;

robot::robot()
{
	bDetect = NULL;
	bDetect = new boundaryDetection;
	bDetect->SetRobot(this);

	pasvMode = NULL;
	pasvMode = new PassiveControl;
	
	ControlCard::GetInstance().Initial();

	activeCtrl = NULL;
	activeCtrl = new activecontrol;
	EMGContrl = NULL;
	EMGContrl = new emgcontrl;

	eyeModeCtl = NULL;
	eyeModeCtl = new EyeMode(bDetect);
	
	pasvMode->m_boundary_detection = bDetect;
	bDetect->startBydetect();
	m_isActiveModeStart = false;
	m_isEmgModeStart = false;
	m_isPasvModeStart = false;
}

robot::~robot() {
	if (pasvMode != NULL)
		delete pasvMode;
	if (bDetect != NULL) {
		delete bDetect;
	}
	if (NULL != activeCtrl)
		delete activeCtrl;
	if (NULL != EMGContrl)
		delete EMGContrl;
	if (NULL != eyeModeCtl)
		delete eyeModeCtl;
}

unsigned __stdcall PositionResetThread(void *) {
	ControlCard::GetInstance().ResetPosition();
	return 0;
}

void robot::clearPasvMove()
{
	pasvMode->ClearMoveData();
}

void robot::pushPasvMove(const Teach& move)
{
	pasvMode->PushbackMoveData(move);
}

bool robot::isMoving()
{
	return pasvMode->IsMoving();
}

void robot::startPasvMove(int index)
{
	//if (ctrlCard->IsCardInitial()) {
	if (m_isPasvModeStart == false) {
		m_isPasvModeStart = true;
		pasvMode->BeginMove(index);
	}
	//}
}
void robot::stopPasvMove()
{
	//if (ctrlCard->IsCardInitial()) {
	if (m_isPasvModeStart == true) {
		m_isPasvModeStart = false;
		pasvMode->StopMove();
	}
	//}
}
void robot::getCurrentPasvMove(Teach& teach)
{
	//if (ctrlCard->IsCardInitial()) {
		pasvMode->GetCurrentMove(teach);
	//}
}
void robot::startTeach() {
		pasvMode->StartTeach();
}
void robot::stopTeach()
{
	//if (ctrlCard->IsCardInitial()) {
		pasvMode->StopTeach();
	//}
}

void robot::getCurrentTeach(Teach& teach)
{
	//if (ctrlCard->IsCardInitial()) {
		pasvMode->GetCurrentTeach(teach);
	//}
}

void robot::addPasvMove()
{
	//if (ctrlCard->IsCardInitial()) {
		pasvMode->AddCurrentTeachToData();
	//}
}
void robot::startActiveMove() {
	if (!m_isActiveModeStart) {
		activeCtrl->startMove();
		m_isActiveModeStart = true;
	}
}

void robot::stopActiveMove()
{
	//if (ctrlCard->IsCardInitial()) {
	if (m_isActiveModeStart) {
		activeCtrl->stopMove();
		m_isActiveModeStart = false;
	}
	//}
}
void robot::getAngle(double angles[2])
{
}

double robot::getWirstForce()
{
	double output;
	
	output = activeCtrl->getWirstForce();
	
	return output;
}
bool robot::isFire()
{
	//if (ctrlCard->IsCardInitial()) {
		return activeCtrl->isFire();
	//}
	//return false;
}

void robot::getPlanePos(short w, short h, double XY[2])
{
	activeCtrl->getEndsXY(734, 600, XY);
}

void robot::setDamping(float FC/* =0.1 */)
{
	activeCtrl->setDamping(FC);
}


void robot::setEyeVel(double factor)
{
	eyeModeCtl->setVel(factor);
}
void robot::eyeCalibrate()
{
	eyeModeCtl->calibrate();
}
void robot::startEyeMove()
{
	eyeModeCtl->start();
}
void robot::stopEyeMove()
{
	eyeModeCtl->stop();
}
void robot::enterEyeMode()
{
	eyeModeCtl->enter();
}
void robot::exitEyeMode()
{
	eyeModeCtl->exit();
}

void robot::getLeftRGB24(unsigned char* data, int _width, int _height)
{
	eyeModeCtl->getRGB24(data, _width, _height, EyeMode::LEFT);
}
void robot::getRightRGB24(unsigned char* data, int _width, int _height)
{
	eyeModeCtl->getRGB24(data, _width, _height, EyeMode::RIGHT);
}

void robot::resetPos() {
	_beginthreadex(NULL, 0, PositionResetThread, NULL, 0, NULL);
}

void robot::setWindow(HWND hWnd)
{
	m_hWnd = hWnd;
	bDetect->Set_hWnd(hWnd);
	pasvMode->Set_hWnd(hWnd);
	ControlCard::GetInstance().Set_hWnd(hWnd);
	EMGContrl->m_hWnd = hWnd;
}

bool robot::isEMGMove()
{
	return EMGContrl->isBeginMove;
}

void robot::startEMGMove()
{
	//if (ctrlCard->IsCardInitial()) {
	if (m_isEmgModeStart == false) {
		m_isEmgModeStart = true;
		EMGContrl->start(bDetect);
	}	
	//}
}
void robot::stopEMGMove()
{
	//if (ctrlCard->IsCardInitial()) {
	if (m_isEmgModeStart == true) {
		m_isEmgModeStart = false;
		EMGContrl->stop();
	}
	//}
}

double robot::getEMGSignal(int index /* = 0 */)
{
	return EMGContrl->getRawData(index);
}

void robot::stopResetPos()
{
	if(Mtimer_ID!=0)
		timeKillEvent(Mtimer_ID);
	if(wAccuracy!=0)
		timeEndPeriod(wAccuracy);
}


void getSensorData(bool Travel_Switch[4])
{
	I32 DI_Group = 0; // If DI channel less than 32
	I32 DI_Data = 0; // Di data
	I32 di_ch[InputChannels];
	I32 returnCode = 0; // Function return code
	returnCode = APS_read_d_input(0, DI_Group, &DI_Data);
	for (int i = 0; i < InputChannels; i++)
		di_ch[i] = ((DI_Data >> i) & 1);

	Travel_Switch[0] = di_ch[16];//0号电机ORG信号-肘部电机
	Travel_Switch[1] = di_ch[17];//0号电机MEL信号-肘部电机

	Travel_Switch[2] = di_ch[18];//1号电机ORG信号-肩部电机
	Travel_Switch[3] = di_ch[19];//1号电机MEL信号-肩部电机
}

