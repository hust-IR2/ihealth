#ifndef EMGCONTRL_H
#define EMGCONTRL_H
#include<NIDAQmx.h>
#include"control_card.h"
#include"boundarydetection.h"
#include<vector>
class emgcontrl
{
public:
    emgcontrl();
    ~emgcontrl();
    void start(boundaryDetection *bDet);
    void stop();
    double getRawData(int index=0);
    void beginMove();
    void stopMove();
	bool isStopThread;
	void acquistRawData();//获取EMG sensor数据
    void emgContrl();
	bool isBeginMove;

	HWND m_hWnd;
private:
    
    bool isBeginSaveData;
    
    bool *switchData;
    HANDLE dataMutex;
    boundaryDetection *bDetect;
    std::vector<double>rawData[4];
};

#endif // EMGCONTRL_H
