#ifndef BOUNDARYDETECTION_H
#define BOUNDARYDETECTION_H
#include "control_card.h"

class robot;

class boundaryDetection 
{
public:
    boundaryDetection();
    ~boundaryDetection();
	//获取光电开关值
    bool* GetSwithData();
	//获取角度值
	double* getAngle();
	double* getVel();
	//开始边界检测
	void startBydetect();
	//停止边界检测
	void stopBydetect();
	//加数据的读写锁
	 HANDLE hMutex;
	 HANDLE hAngleMutex;
	 HANDLE hVelMutex;

	 //控制线程
	 bool m_stop;
     //获取四个光电开关状态
     void getSensorData();
     //获取两个力矩传感器的数据
     void getTorqueData();
	 //获取拉力传感器数据
	 void GetPullSensorData();

	 void getTorqueData(double data[2]);
	 //获取关节角度
	 void getEncoderData();
	 //获取关节速度
	 void getJointVel();
	 void Set_hWnd(HWND hWnd);
     void check();
		
	 void SetRobot(robot *pRobot);
	 bool is_error_happens_;

private:
     //光电行程开关的示数，0-ORG0，1-MEL0，2-ORG1，3-MEL1
     bool Travel_Switch[4];
     //力矩传感器读数
     double Torque_Sensor[2];
	 //拉力传感器读数
	 double Pull_Sensor[4];
	 //角度
	 double angle[2];
	 //速度
	 double vel[2];
	 bool m_emergency_stop_status;

	 int vel_i;
	 double m_Pos_A[3];
	 double m_Pos_S[3];
    
	 HWND m_hWnd = NULL;
	 robot *m_pRobot = NULL;
	 unsigned m_Handle;
};

#endif // BOUNDARYDETECTION_H
