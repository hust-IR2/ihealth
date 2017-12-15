// DAQSys.h: interface for the DAQSys class.
//
//////////////////////////////////////////////////////////////////////
#include "NIDAQmx.h"
#include<string>
#pragma once

#define NUM_CHANNELS 7
#define ERROR_MESSAGE_SIZE 2048
#define SCAN_BUFFER_SIZE 280
/* this is used for development only, so that a DAQ is not required to test the GUI */
#define TEST_WITHOUT_DAQ 0
class DAQSys  
{
public:
	/* this method is used to setup the DAQ during object construction of when changed in the Program Options */
	int inited(short firstChannel, double scanRate);	
	/* scans all 7 gauges, or returns last scan if boolean is true, returns 0 for success or -1 for saturation */
	int ScanGauges(double voltages[7], bool useStoredValues);	
	/* return the device number of the DAQ - originally defaults to 1 */
    std::string getDeviceName() { return m_cstrDeviceName; }
	/* return the first channel to be scanned by the DAQ - originally defaults to 1 */
	short getFirstChannel() { return firstChannel; }
	/* returns the scan rate to be used when scanning the gauges */
	double getScanRate() { return scanRate; }
	DAQSys();
	virtual ~DAQSys();

private:
	TaskHandle m_th; /* The task handle to the NI-DAQmx scan. */
	double range;
	bool bipolarity;
	double scanRate;		
    std::string m_cstrDeviceName;
	short firstChannel;	
	unsigned short numChannels;	
	long scanBuffer[SCAN_BUFFER_SIZE]; /*memory buffer used by scanning operation*/
	unsigned int scanCount; /*the number of scans in a reading.  averaged out to get
								the final result*/
	double m_dUpperSaturation; /* The upper level past which a gauge is considered
							   saturated. */
	double m_dLowerSaturation; /* The lower level past which a gauge is considered
							   saturated. */

};
