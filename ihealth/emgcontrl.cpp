#include "emgcontrl.h"
#include <process.h> 

#define EMG_TIME 0.1
double Data_Linear[10]={0};
double RawData[10]={0};
emgcontrl::emgcontrl() {
	isStopThread=false;
    isBeginMove=false;
    bDetect=NULL;
	dataMutex= CreateMutex(NULL, FALSE, NULL);
    //qDebug()<<"emgcontrl construct done!";
}
emgcontrl::~emgcontrl() {
    stop();
}
void emgcontrl::acquistRawData()
{
    TaskHandle  taskHandle = 0;
    int32       read=0;
    int status =0;
     status =DAQmxCreateTask("EMGTask", &taskHandle);
     status =DAQmxCreateAIVoltageChan(taskHandle, "Dev4/ai0:5", "EMGChannel", DAQmx_Val_RSE, 0, 5, DAQmx_Val_Volts, NULL);

     status =DAQmxCfgSampClkTiming(taskHandle, "OnboardClock", 1000, DAQmx_Val_Rising, DAQmx_Val_ContSamps, 20);

     status =DAQmxStartTask(taskHandle);

     status =DAQmxReadAnalogF64(taskHandle, 10, 0.2, DAQmx_Val_GroupByScanNumber, Data_Linear, 60, &read, NULL);
     status =DAQmxStopTask(taskHandle);
     status =DAQmxClearTask(taskHandle);

	 double *pRawData = new double[6];
     for(int i=0;i<4;i++)
     {      
		 WaitForSingleObject(dataMutex, INFINITE);
         RawData[i]= Data_Linear[i];
		 pRawData[i] = RawData[i];
		 ReleaseMutex(dataMutex);
     }

	 double raw_arm = 0;
	 double raw_shoulder = 0;
	 APS_get_position_f(ElbowAxisId, &raw_arm);
	 APS_get_position_f(ShoulderAxisId, &raw_shoulder);
	 pRawData[4] = raw_shoulder*Unit_Convert;
	 pRawData[5] = raw_arm*Unit_Convert;

	 ::PostMessage(m_hWnd, 2050, NULL, (LPARAM)pRawData);
}
unsigned int __stdcall EMGThreadFun(PVOID pParam)
{
	emgcontrl *EMGCtrl = (emgcontrl*)pParam;
	UINT oldTickCount, newTickCount;
	oldTickCount = GetTickCount();
	int i = 0;
	while (TRUE)
	{
		if (EMGCtrl->isStopThread)
			break;
		//延时 BOYDET_TIME s
		while (TRUE)
		{
			newTickCount = GetTickCount();
			if (newTickCount - oldTickCount >= EMG_TIME * 1000)
			{
				oldTickCount = newTickCount;
				break;
			}
			else
				SwitchToThread();
		}
		EMGCtrl->acquistRawData();
		if (EMGCtrl->isBeginMove && (i % 4 == 0))
			EMGCtrl->emgContrl();
		i++;
	}
	return 0;
}
void emgcontrl::start(boundaryDetection *bDet)
{
    //qDebug()<<"EMG  started!";
	bDetect = bDet;
	isStopThread=false;
	HANDLE handle;
	//开始跟随运动
	beginMove();
	handle = (HANDLE)_beginthreadex(NULL, 0, EMGThreadFun, this, 0, NULL);

}
void emgcontrl::stop()
{
     //qDebug()<<"EMG  Stoped!";
	//结束跟随运动
	 stopMove();
	 isStopThread = true;

}
double emgcontrl::getRawData(int index)
{ 
	WaitForSingleObject(dataMutex, INFINITE);
    if(RawData[index]<0)
         RawData[index]=0;
    double reading=RawData[index];
	ReleaseMutex(dataMutex);
   // qDebug()<<"RawData "<<index<<" is "<<RawData[index];
    if(index<4)
        return reading;
    else
    {
        //qDebug()<<"index is "<<index<<"not corrected";
        return 0;
    }
}
void emgcontrl::beginMove()
{
	ControlCard::GetInstance().SetMotor(MotorOn);
	ControlCard::GetInstance().SetClutch(ClutchOn);
    isBeginMove=true;
}
void emgcontrl::stopMove() {
	ControlCard::GetInstance().SetMotor(MotorOff);
	ControlCard::GetInstance().SetClutch(ClutchOff);
    isBeginMove=false;
}
void emgcontrl::emgContrl()
{
    bool *swithData=bDetect->GetSwithData();
    bool  shoulderSwitch[2]={0};
    bool  elbowSwitch[2]={0};

    //获取光电传感器读数
    for(int i=0;i<2;i++)
    {
        elbowSwitch[i]=swithData[i];
        shoulderSwitch[i]= swithData[2+i];
    }
	WaitForSingleObject(dataMutex, INFINITE);
    double elbowVel=RawData[0]-RawData[1];
    double shoulderVel=RawData[2]-RawData[3];
	ReleaseMutex(dataMutex);
	ControlCard::GetInstance().VelocityMove(ShoulderAxisId, shoulderVel);
	ControlCard::GetInstance().VelocityMove(ElbowAxisId, elbowVel);
}

