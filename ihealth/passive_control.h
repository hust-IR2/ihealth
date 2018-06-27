#ifndef PASVCONTRL_H
#define PASVCONTRL_H
#include"control_card.h"
#include"boundarydetection.h"
#include"active_control.h"
#include <vector>
#include <queue>
//#include <mutex>
#include <string>
#include <vector>

struct Teach {
	std::vector<double>Target_Pos[2];//存储位置的数组
	std::vector<double>Target_Vel[2];//存储速度的数组
	double Time;//运动时间 s
};

class PassiveControl {
public:
    PassiveControl();
    ~PassiveControl();

	//清除被动运动序列的数据
	void ClearMoveData();
	//加入新的示教数据到运动序列中（运动序列包含多条示教数据，示教数据包含角度和速度的数组）
	void PushbackMoveData(const Teach& teach);
	bool IsMoving();
    //开始被动运动，index-表示动作的索引
    void BeginMove(int index);
	//停止被动运动
    void StopMove();
	void GetCurrentMove(Teach& teach);
	//开始示教
	void StartTeach();
	void StopTeach();
	void GetCurrentTeach(Teach& teach);
	//添加新动作到运动序列中
	void AddCurrentTeachToData();
	void OnPASVHermite(double PosArm, double PosShoul, double Time);
	//示教的采样函数
	void Teach_Sample();
	void TeachCtrl();
	void Move_Sample();
	void Set_hWnd(HWND hWnd);

public:
	bool isBeginTeach;
	//判断是否在初始位置
	bool isInInitPos;
	//判断是否在运动中
	bool isMoving;
	bool isbeginMove;
	//默认运动时间
	int defaultCycleTime;
	//当前运动动作参数
    std::vector<int>currentMove;
	//存储添加动作信息
	std::vector<Teach>motionParam;
	//获取当前关节角度

	boundaryDetection *m_boundary_detection = NULL;
private:
	//初始化函数
	void init();
	HWND m_hWnd = NULL;
    int timecount;
    double PASVHermite_time;
	boundaryDetection * bDetect;
	activecontrol *activectrl;
	Teach currentTeach;
	Teach moveTeach;
	Teach moveSample;
	double Her_Teach_Time[2][2];//插值运动位置的时间
	double  Her_Teach_Vel[2][2];//插值运动的始末速度
	double  Her_Teach_Pos[2][2];//插值运动的始末位置
	std::vector<std::vector<int> >pasvMoveArray;
    std::queue<std::pair<std::string,int*>>pasvMove;  	
    //hermite插值算法-返回值为curtime时间点下电机的位置
    double PHermite(double foretime[2],//已知时间点
                        double forepos[2],//已知位置点
                        double forevel[2],//已知速度点
                        double t);////所求时间点
   
};

#endif // PASVCONTRL_H
