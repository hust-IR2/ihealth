#pragma once
// Adlink head file////////
#include "APS168.h"
#include "ErrorCodeDef.h"
#include "APS_define.h"
#include "type_def.h"
//////////////////////////
#include <NIDAQmx.h>
#include "Log.h"

#define CON (0)
#define COFF (1) 

#define ON  (1)
#define OFF (0)
#define __MAX_DO_CH (24)
#define __MAX_DI_CH (24)
#define shoudlerAxisId 0
#define elbowAxisId 1
#define Unit_Convert  0.009  // 360/40000 传动系数，60-减数比，16384-一圈脉冲数，2pi-圆周   //
class contrlCard
{
public:
	contrlCard();
	~contrlCard();
	
	bool IsCardInitial(void);//检查运动控制卡是否初始化成功，成功返回true,失败返回false
    //运动控制卡初始化函数
    int Initial();
    //获取四个光电开关状态
    void Robot_Pos(bool Optoelc_Switch[2][2]);
    //离合器开闭 ON-开，OFF-关闭
    void SetClutch(bool onOroff=TRUE);
    //电机开闭，ON-开，OFF-关闭
    void ServeTheMotor(bool onOroff=TRUE );  
	//给电机速度命令
    void MotionMove(short AxisId,double Vel,bool switchData[2]);
	//获取两电机当前的角度
	void getEncoderData(double EncoderData[2]);
    //使得各电机参数归零
    void SetParamZero();
	//复位
	void resetPos();
	void Set_hWnd(HWND hWnd);
private:
	HWND m_hWnd = NULL;
    bool AxisStatus;
    bool ClutchStatu;
	long v_card_name;
	long v_board_id;
	long v_channel;
	int v_total_axis;
	bool v_is_card_initialed;
	long Is_thread_creat;
	//设置速度指令，AxisId-轴号，Vel-速度指令
    void SetVelocityCmd(short AxisId,double Vel);
    bool getAixsServeStatu(short AxisId);
};

