// FTWrapper.cpp: implementation of the FTWrapper class.
//
//////////////////////////////////////////////////////////////////////

#include "FTWrapper.h"
#include <direct.h>
#include <windows.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FTWrapper::FTWrapper()
{
	// CreateDispatch is required to be called exactly once
	// therefore, creating multiple instances of FTWrapper will probably not work
	ActiveCalibration = -1;
	status = UNKNOWN;
	daqerrmsg = "";
	bias = false;
	mDAQSys = NULL;
	cal = NULL;
	for (int i=0; i<7; i++) {
		biasVoltage[i] = 0.0;
	}	
	mDAQSys = new DAQSys;
	status = DAQ_READY;


}

FTWrapper::~FTWrapper()
{
	if (NULL != mDAQSys)
		delete mDAQSys;
	destroyCalibration(cal);
}

// reads a Calibration File and resets instance variables
int FTWrapper::LoadCalFile()
{
	// create Calibration struct
    cal = createCalibration("C:\\Calibration\\FT18228.cal",1);
	if (cal == NULL) {
		daqerrmsg = "Specified calibration could not be loaded";
        return -1;
	}
	return 0;
}
// returns a new 7-element array of doubles containing Force and Torque measurements
int FTWrapper::GetForcesAndTorques(double output[7])
{
		double ft_voltages[7];
		float currentVoltage[7] = {0};
		float FT[7] = { 0 };
		int mDAQstatus = mDAQSys->ScanGauges(ft_voltages, false);
		for (int i = 0; i < 7; i++)
			currentVoltage[i] = ft_voltages[i];

        ConvertToFT(cal, currentVoltage, FT);

		for (int j=0; j<6; j++) {
			// COleSafeArrays use pass-by-reference methods to read or write
            output[j]=FT[j];
		}
		// copy the thermistor voltage over to the output for display
		output[6] = ft_voltages[6];
		return mDAQstatus;
}

// returns a new 7-element array of doubles containing Raw Voltages
int FTWrapper::GetRawVoltages(double voltages[7])
{
	if (status & DAQ_READY) {
		// initiate a new scan
		int mDAQstatus = mDAQSys->ScanGauges(voltages, false);
		if (bias) {
			// don't bias voltage[6], it is the thermistor
			for(int i=0;i<6;i++) {
				voltages[i] = voltages[i] - biasVoltage[i];
			}
		}
		return mDAQstatus;
	}
	else {
		return -1;
	}
}

// specifies whether force/torque and voltages will be biased or not
void FTWrapper::BiasCurrentLoad(bool newbias)
{
	float currentVoltage[7] = { 0 };
    if (newbias == true) {
		//这里测试因为可能还没读到数据就开始Bias，导致Bias不正常，应该确保读到数据之后再Bias
		while (biasVoltage[0] == 0) {
			mDAQSys->ScanGauges(biasVoltage, true);
		}
		for (int i = 0; i < 7; i++)
			currentVoltage[i] = biasVoltage[i];
	
		Bias(cal, currentVoltage);
	}	
}
void FTWrapper::setFUnit()
{
    sts = SetForceUnits(cal, "N");
	switch (sts) {
	case 0: break;	// successful completion
	case 1:daqerrmsg="Invalid Calibration struct";
	case 2: daqerrmsg = "Invalid force units";
	default: daqerrmsg = "Unknown error";
	}
}
void FTWrapper::setTUnit()
{
    sts = SetTorqueUnits(cal, "N-m");
	switch (sts) {
	case 0: break;	// successful completion
	case 1: daqerrmsg = "Invalid Calibration struct";
	case 2: daqerrmsg = "Invalid torque units";
	default: daqerrmsg = "Unknown error";
	}
}
// returns the active calibration of the _FTSensor object
void FTWrapper::GetBiasVoltages(double biases[])
{
	int i;
	for (i = 0; i < 7; i++)
	{
		/*set bias to 0 if bias is not active*/
		biases[i] = bias ? biasVoltage[i] : 0;
	}
}
