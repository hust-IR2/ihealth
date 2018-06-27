#include "passive_control.h"
#include <windows.h>
#include <process.h> 
#include<iostream>
#define TIMER_SLEEP   0.1
const double Comfort_Pos[2]={0,0};//开始运动的初始位置，人觉得舒服的位置
bool isStopThread = false;
int loop_count = 0;
int Target_count = 0;
PassiveControl::PassiveControl()
{
    //初始化动作队列
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++) {
			Her_Teach_Time[i][j] = 0;
			//更新插值位置范围
			Her_Teach_Pos[i][j] = 0;
			//更新插值速度范围
			Her_Teach_Vel[i][j] = 0;
		}
	}
    isInInitPos=true;
	isMoving=false;
	isBeginTeach = false;
	isbeginMove=false;

    timecount=0;
    PASVHermite_time=0;
	defaultCycleTime =16;
	bDetect = NULL;
	activectrl = NULL;
	activectrl = new activecontrol;
	//init();
}

PassiveControl::~PassiveControl() {
	if (activectrl != NULL) {
		delete activectrl;
	}
}

void PassiveControl::AddCurrentTeachToData() {
	motionParam.push_back(currentTeach);
}

void PassiveControl::init() {
	//添加默认的四个动作
    /*int existMove[4][2]={{55,0},{53,43},{60,27},{62,40}};
    for(int i=0;i<4;i++)
     addMovement(existMove[i],defaultCycleTime);*/
}

//这里示教和被动运动都写在了这个函数里面，通过isBeginTeach或者isbeginMove来决定运行哪一个部分。
unsigned int __stdcall ThreadFun(PVOID pParam) {
	PassiveControl *pasvCtrl= (PassiveControl*)pParam;
	UINT oldTickCount, newTickCount;
	oldTickCount = GetTickCount();
	while (TRUE) {
		//延时 TIMER_SLEEP s
		while (TRUE) {
			newTickCount = GetTickCount();
			if (newTickCount - oldTickCount >= TIMER_SLEEP*1000) {
				oldTickCount = newTickCount;
				break;
			}

			else
				SwitchToThread();
		}
		if (isStopThread) {
			break;
		}
		//是否开始示教
		//一秒钟采集一次
		if (pasvCtrl->isBeginTeach && (loop_count % 10 == 0)) {
			pasvCtrl->Teach_Sample();
		}

		//是否开始运动
		if (pasvCtrl->isbeginMove) {
			pasvCtrl->TeachCtrl();
		}
		
		if(pasvCtrl->isbeginMove && (loop_count%2 == 0)) {	// 5HZ
			pasvCtrl->Move_Sample();
		}


		loop_count++;
		
		/*double armCmdAngle = static_cast<double>(pasvCtrl->currentMove[0]);
		double shulCmdAngle = static_cast<double>(pasvCtrl->currentMove[1]);
		double cycleTime= static_cast<double>(pasvCtrl->currentMove[2])/2;
		pasvCtrl->OnPASVHermite(53, 43, 8);*/
	}
	pasvCtrl->isMoving = false;
	std::cout << "Thread end!" ;
	return 0;
}

void PassiveControl::ClearMoveData()
{
	motionParam.clear();
}

void PassiveControl::PushbackMoveData(const Teach& teach)
{
	motionParam.push_back(teach);
}

bool PassiveControl::IsMoving()
{
	return isMoving;
}

void PassiveControl::BeginMove(int index) {
    //判断index是否有效
	if (index >= motionParam.size())
		return;
	//判断是否在运动中
	if(isMoving)
		return;
	//取出示教所需要的数据
	moveTeach = motionParam.at(index);
	//各项计数归零
	loop_count = 0;
	timecount = 0;
	Target_count = 0;
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++) {
			Her_Teach_Time[i][j] = 0;
			//更新插值位置范围
			Her_Teach_Pos[i][j] = 0;
			//更新插值速度范围
			Her_Teach_Vel[i][j] = 0;
		}
	}
	for (int k = 0; k < 2; k++) {
		moveSample.Target_Pos[k].clear();
		moveSample.Target_Vel[k].clear();
	}
	
	//打开电机，离合器
	ControlCard::GetInstance().SetMotor(MotorOn);
	ControlCard::GetInstance().SetClutch(ClutchOn);
	//关闭示教采集功能
	isBeginTeach = false;
	//打开线程
	isStopThread = false;
	//开始运动
	isbeginMove = true;
	HANDLE handle;
	handle= (HANDLE)_beginthreadex(NULL, 0, ThreadFun, this, 0, NULL);
	//WaitForSingleObject(handle, INFINITE);  
}
void PassiveControl::StopMove() {
	//关闭电机
	ControlCard::GetInstance().SetMotor(MotorOff);
	//关闭离合器
	ControlCard::GetInstance().SetClutch(ClutchOff);
    //关闭线程
	isbeginMove = false;
	isStopThread = true;
	isBeginTeach = false;
	isMoving = false;
}

void PassiveControl::GetCurrentMove(Teach& teach) {
	teach = moveSample;

	//这里原本有清空数据，但是我认为不应该清空
	//for (int k = 0; k < 2; k++) {
	//	//moveSample.Target_Pos[k].clear();
	//	moveSample.Target_Vel[k].clear();
	//}
}

void PassiveControl::OnPASVHermite(double PosArm,double PosShoul,double Time) {
        double time_forwrd[2]={0,Time};//回到初始位置的时间*-++*+++
        double time_back[2]={Time+2,Time*2+2};//回到初始位置的时间

        double vel_forwrd[2]={0,0};//回到初始位置的始末速度
        double vel_back[2]={0,0};//回到初始位置的始末速度

        double armpos_forwrd[2]={Comfort_Pos[1],PosArm};
        double armpos_back[2]={PosArm,Comfort_Pos[1]};

        double shoulpos_forwrd[2]={Comfort_Pos[0],PosShoul};
        double shoulpos_back[2]={PosShoul,Comfort_Pos[0]};
		double joint_angle[2] = {0};
		ControlCard::GetInstance().GetEncoderData(joint_angle);
		double cmdVel[2] = { 0 };

        //被动模式命令位置
        double arm_motor_cmd=0;
        double shoul_motor_cmd=0;
        PASVHermite_time = timecount * TIMER_SLEEP;
        if (PASVHermite_time<=Time) {
			
            arm_motor_cmd=PHermite(time_forwrd,armpos_forwrd,vel_forwrd,PASVHermite_time);
			cmdVel[1] = (arm_motor_cmd - joint_angle[1])/TIMER_SLEEP;
			ControlCard::GetInstance().VelocityMove(ElbowAxisId, cmdVel[1]);
            //APS_absolute_move(elbowAxisId,arm_motor_cmd/Unit_Convert,15/Unit_Convert);


            shoul_motor_cmd=PHermite(time_forwrd,shoulpos_forwrd,vel_forwrd,PASVHermite_time);
			cmdVel[0] = (shoul_motor_cmd - joint_angle[0]) / TIMER_SLEEP;
			ControlCard::GetInstance().VelocityMove(ShoulderAxisId, cmdVel[0]);
            //APS_absolute_move(shoudlerAxisId,shoul_motor_cmd/Unit_Convert,15/Unit_Convert);

        }
		else if ((PASVHermite_time > Time)&&(PASVHermite_time<(Time + 2)))
		{
			APS_stop_move(ElbowAxisId);
			APS_stop_move(ShoulderAxisId);
		}
        else if((PASVHermite_time<=(Time*2+2))&&(PASVHermite_time>=(Time+2)))
        {
            arm_motor_cmd=PHermite(time_back,armpos_back,vel_back,PASVHermite_time);
			cmdVel[1] = (arm_motor_cmd - joint_angle[1]) / TIMER_SLEEP;
			ControlCard::GetInstance().VelocityMove(ElbowAxisId, cmdVel[1]);
            //APS_absolute_move(elbowAxisId,arm_motor_cmd/Unit_Convert,15/Unit_Convert);


            shoul_motor_cmd=PHermite(time_back,shoulpos_back,vel_back,PASVHermite_time);
			cmdVel[0] = (shoul_motor_cmd - joint_angle[0]) / TIMER_SLEEP;
			ControlCard::GetInstance().VelocityMove(ShoulderAxisId, cmdVel[0]);
            //APS_absolute_move(shoudlerAxisId,shoul_motor_cmd/Unit_Convert,15/Unit_Convert);

        }
        else if(PASVHermite_time>=(Time*2+2))
        {
			StopMove();
            //std::cout<<"Motion Done!"<<std::endl;
        }
        /*std::cout<< "Time is:" << PASVHermite_time<<std::endl;
		std::cout << "Arm cmd pos is:" << arm_motor_cmd<<std::endl;
		std::cout << "Shoulder cmd pos is:" << shoul_motor_cmd<<std::endl;*/
        timecount++;
}
double PassiveControl::PHermite(double foretime[2],double forepos[2],double forevel[2],double t)
{
    double Houtput=0;
    double a[2]={0};
    double b[2]={0};
    a[0]=(1-2*(t-foretime[0])/(foretime[0]-foretime[1]))*pow((t-foretime[1])/(foretime[0]-foretime[1]),2);
    a[1]=(1-2*(t-foretime[1])/(foretime[1]-foretime[0]))*pow((t-foretime[0])/(foretime[1]-foretime[0]),2);
    b[0]=(t-foretime[0])*pow((t-foretime[1])/(foretime[0]-foretime[1]),2);
    b[1]=(t-foretime[1])*pow((t-foretime[0])/(foretime[1]-foretime[0]),2);
    Houtput=a[0]*forepos[0]+a[1]*forepos[1]+b[0]*forevel[0]+b[1]*forevel[1];
    return Houtput;
}
void PassiveControl::StartTeach() {
	if (!isMoving) {		
		activectrl->startMove();
		isBeginTeach = true;
		isStopThread = false;

		//清空currentTeach
		for (int k = 0; k < 2; k++) {
			if (!currentTeach.Target_Pos[k].empty()) {
			    currentTeach.Target_Pos[k].clear();
			 }

			if (!currentTeach.Target_Vel[k].empty()) {
				currentTeach.Target_Vel->clear();
			}
		}

		HANDLE handle;
		handle = (HANDLE)_beginthreadex(NULL, 0, ThreadFun, this, 0, NULL);
	}
}

void PassiveControl::StopTeach() {
	activectrl->stopMove();
	isBeginTeach = false;
	isStopThread = true;
	isMoving = false;
}

void PassiveControl::GetCurrentTeach(Teach& teach)
{
	teach = currentTeach;
}

void PassiveControl::Teach_Sample() {

	//取角度
	double joint_angle[2]{ 0 };
	double joint_vel[2]{ 0 };
	ControlCard::GetInstance().GetEncoderData(joint_angle);
	ControlCard::GetInstance().GetJointVelocity(joint_vel);
	for (int i = 0; i < 2; i++) {
		currentTeach.Target_Pos[i].push_back(joint_angle[i]);
		currentTeach.Target_Vel[i].push_back(joint_vel[i]);
	}
}

void PassiveControl::Move_Sample()
{
	//取角度
	//double *jointAngle = bDetect->getAngle();
	//double *jointVel = bDetect->getVel();
	//bDetect->getTorqueData();

	double jointAngle[2];
	bDetect->getTorqueData(jointAngle);
	for (int i = 0; i < 2; i++) {
		//moveSample.Target_Pos[i].push_back(jointAngle[i]);
		moveSample.Target_Vel[i].push_back(jointAngle[i]);
	}
}

//这个函数是对目标点进行插值，然后根据插值的位置进行运动
void PassiveControl::TeachCtrl() {
	double Teach_Time = loop_count*0.1;
	I32 Axis[2] = { ShoulderAxisId, ElbowAxisId };

	//每过一秒就更新插值区间
	if (loop_count % 10 == 0) {
		if (Target_count < moveTeach.Target_Vel[0].size() - 1) {
			for (int i = 0; i<2; i++) {
				//更新插值时间范围
				Her_Teach_Time[i][0] = Teach_Time;
				Her_Teach_Time[i][1] = Teach_Time + 1;
				//更新插值位置范围
				Her_Teach_Pos[i][0] = moveTeach.Target_Pos[i].at(Target_count);
				Her_Teach_Pos[i][1] = moveTeach.Target_Pos[i].at(Target_count + 1);
				//更新插值速度范围
				Her_Teach_Vel[i][0] = moveTeach.Target_Vel[i].at(Target_count);
				Her_Teach_Vel[i][1] = moveTeach.Target_Vel[i].at(Target_count + 1);
			}
			Target_count++;
		} else {
			//运动完成，停止运动
			StopMove();
		}
	}

	//在插值区间内，相当于每100ms就运动到一个新的插值点。这里的插值有问题，连续发了两次命令
	for (int j = 0; j < 2; j++) {
		double Teach_Cmd = PHermite(Her_Teach_Time[j],
			Her_Teach_Pos[j],
			Her_Teach_Vel[j],
			Teach_Time);
		APS_absolute_move(Axis[j], Teach_Cmd/ Unit_Convert, 15/Unit_Convert);
		isMoving = true;
	}
}

void PassiveControl::Set_hWnd(HWND hWnd) {
	m_hWnd = hWnd;
}