#ifndef PASVCONTRL_H
#define PASVCONTRL_H
#include"contrlCard.h"
#include"boundarydetection.h"
#include"activecontrol.h"
#include <vector>
#include <queue>
//#include <mutex>
#include <string>
#include <vector>

struct Teach
{
	std::vector<double>Target_Pos[2];//存储位置的数组
	std::vector<double>Target_Vel[2];//存储速度的数组
	double Time;//运动时间 s
};
class pasvContrl 
{
public:
    pasvContrl();
    ~pasvContrl();

	void clearMove();
	void pushMove(const Teach& teach);
	bool Moving();
    //开始被动运动，index-表示动作的索引
    void beginMove(int index, boundaryDetection *byDetect);
	//停止被动运动
    void stopMove();
	void getCurrentMove(Teach& teach);
	//开始示教
	void startTeach(boundaryDetection *byDetect);
	void stopTeach();
	void getCurrentTeach(Teach& teach);

	bool isBeginTeach;
	//添加新动作
    void addMovement();
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
	void getEncoderData(int EncoderData[2]);
	void getSensorData(bool Travel_Switch[4]);
	void OnPASVHermite(double PosArm,double PosShoul,double Time);
	//示教的采样函数
	void Teach_Sample();
	void TeachCtrl();
	void Move_Sample();
	void Set_hWnd(HWND hWnd);
	boundaryDetection *m_boundary_detection = NULL;
private:
	//初始化函数
	void init();
	HWND m_hWnd = NULL;
    int timecount;
    double PASVHermite_time;
	contrlCard *ctrlCard;
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
