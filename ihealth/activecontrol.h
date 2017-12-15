#ifndef ACTIVECONTROL_H
#define ACTIVECONTROL_H
/*#include <Eigen/Core>
#include <unsupported/Eigen/MatrixFunctions>*/
#include "FTWrapper.h"
#include "matrix.h"
#include "contrlCard.h"

#include "boundarydetection.h"

class activecontrol
{
public:
    activecontrol();
    ~activecontrol();
    void startAcquisit();
    void stopAcquisit();
    void startMove(boundaryDetection *Angle);
    void stopMove();

	void timerAcquisit();

	void getRawAngle(double angle[2]);
	double* getAngles();
	double getWirstForce();
	bool isFire();
	//获取机器人末端位置
	void getEndsXY(short Axis_X, short Axis_Y, double XY[2]);
	void setDamping(float FC=0.1);

	boundaryDetection *m_boundary_detection = NULL;
	bool m_stop;
	bool isMove;
private:
    FTWrapper mFTWrapper;
	boundaryDetection *bDetect;//关节角
    contrlCard *ctrlCard;
    //力控模式算出的命令速度
    double cmdVel[2];
    double jAngle[2];
	void FTSContrl();
    //将原始值进行坐标变换
    void Raw2Trans(double RAWData[6],double DistData[6]);
    //将转换后的值进行滤波-二阶巴特沃斯低通滤波器
    void Trans2Filter(double TransData[6],double FiltedData[6]);
    void FiltedVolt2Vel(double FiltedData[6]);
    
	HANDLE m_hThread;
};

#endif // ACTIVECONTROL_H
