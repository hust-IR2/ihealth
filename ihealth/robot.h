#pragma once
#include"passive_control.h"
#include "boundarydetection.h"
#include "control_card.h"
#include"active_control.h"
#include"emgcontrl.h"
#include "EyeMode.h"


class robot 
{
public:
	robot();
	~robot();
	/************************************************************************/
	/*                           被动模式接口                                */
	/************************************************************************/
	void clearPasvMove();
	void pushPasvMove(const Teach& move);
	bool isMoving();
	//开始被动运动，index-表示动作的索引
	void startPasvMove(int index);
	//停止被动运动
	void stopPasvMove();
	void getCurrentPasvMove(Teach& teach);
	//开始示教
	void startTeach();
	//结束示教
	void stopTeach();
	void getCurrentTeach(Teach& teach);

	//添加新动作
	void addPasvMove();
	//关节力矩值-数据接口，0-肩部关节力矩，1-肘部关节力矩
	bool* getSwitchData();
	/************************************************************************/
	/*                           主动模式接口                                */
	/************************************************************************/
	//开始主动运动
	void startActiveMove();
	//结束主动运动
	void stopActiveMove();
	//返回关节角度-数据接口,0-肩部关节角度，1-肘部关节角度
	void getAngle(double angles[2]);
	//返回握力-数据接口
	double getWirstForce();
	bool	isFire();
	void	getPlanePos(short w, short h, double XY[2]);
	void	setDamping(float FC=0.1);	
	/************************************************************************/
	/*                           sEMG模式接口                                */
	/************************************************************************/
	bool isEMGMove();
	//开始EMG运动
	void startEMGMove();
	//停止EMG运动
	void stopEMGMove();
	//获取EMG信号-数据接口，index-信号编号，分别为0，1，2，3
	double getEMGSignal(int index = 0);
	//返回关节角度-数据接口,0-肩部关节角度，1-肘部关节角度(同上面主动模式接口) 
	/************************************************************************/
	/*                           眼动模式接口                                */
	/************************************************************************/
	//返回关节角度-数据接口,0-肩部关节角度，1-肘部关节角度(同上面主动模式接口) 
	void enterEyeMode(); // call it while enter eye mode.
	void exitEyeMode();  // call it while enter eye mode.
	void getLeftRGB24(unsigned char* data, int _width, int _height);  // get image data of left eye
	void getRightRGB24(unsigned char* data, int _width, int _height); // get image data of right eye
	void startEyeMove(); // call it while clicking the start
	void stopEyeMove();  // call it while clicking the stop
	void setEyeVel(double factor); // set velocity
	void eyeCalibrate(); // call it before startEyeControl.

	//复位
	void resetPos();
	void stopResetPos();

	void setWindow(HWND hWnd);
public:
	PassiveControl *pasvMode;//被动控制模式
	boundaryDetection *bDetect;//边界检测
	activecontrol *activeCtrl;
	emgcontrl *EMGContrl;
	EyeMode *eyeModeCtl;
	
	HWND m_hWnd = NULL;
	bool m_isPasvModeStart;
	bool m_isActiveModeStart;
	bool m_isEmgModeStart;
};

void getSensorData(bool Travel_Switch[4]);