// FTWrapper.h: interface for the FTWrapper class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "DAQSys.h"
#include"ftconfig.h"
#include<string>
class FTWrapper  
{
public:
	
	enum DAQFT_STATUS { DAQ_ERROR=0, DAQ_READY=1, CALFILE_LOADED=2, ALL_READY=3 , UNKNOWN=4};
	FTWrapper();
	virtual ~FTWrapper();
	//puts the bias voltages in argument, fills with 0s if bias is not enabled	
	void GetBiasVoltages(double biases[]);
	// this function returns a value of type FTWrapper::DAQFT_STATUS
	DAQFT_STATUS GetStatus() { return status; }
	// returns the daq err message if status == DAQ_ERROR, else ""
    std::string GetDAQErrorMessage() { return daqerrmsg; }
	// returns the _FTSensor object held by this FTWrapper object
	// reads a Calibration File and resets instance variables
    int LoadCalFile();
	// fills a 7-element array of doubles with Force and Torque measurements
	int GetForcesAndTorques(double[7]);
	// fills a 7-element array of doubles with Raw Voltages
	int GetRawVoltages(double[7]);
	// specifies whether force/torque and voltages will be biased or not
    void setFUnit();
    void setTUnit();
	void BiasCurrentLoad(bool bias);
	// returns the active calibration of the _FTSensor object
	// returns the index of the active calibration of the _FTSensor object
	short GetActiveCalibrationIndex() { return ActiveCalibration; }
	/* returns a pointer to the DAQSys object */
	DAQSys* getDAQSys() { return mDAQSys; }


private:
	double biasVoltage[7];
    std::string daqerrmsg;
	// DAQSys object used to aquire data from the NI-DAQ device
	DAQSys* mDAQSys;
	// _FTSensor object used to read calibration files and get the Active Calibration
	// an index into the _FTSensor's Calibrations list of the Active Calibration
	short ActiveCalibration;
	// a boolean representing whether force/torque and voltages should be biased
	bool bias;
	DAQFT_STATUS status;
	char *calfilepath;      // name of calibration file
	unsigned short index;   // index of calibration in file (second parameter; default = 1)
	Calibration *cal;		// struct containing calibration information
	short sts;              // return value from functions
};
