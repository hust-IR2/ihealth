#include "data_acquisition.h"

const char *DataAcquisition::kTorqueChannel = "dev3/ai18:19";
const char *DataAcquisition::kPullSensorChannel = "dev3/ai0:3";
const double DataAcquisition::kRawToReal = 2.0;

DataAcquisition::DataAcquisition() = default;

DataAcquisition &DataAcquisition::GetInstacne() {
	static DataAcquisition instance;
	return instance;
}

void DataAcquisition::AcquisiteTorqueData() {
	TaskHandle taskHandle = 0;
	int32 read = 0;
	int status = 0;
	double torque_data[2] { 0 };
	status = DAQmxCreateTask("TorqueDataTask", &taskHandle);
	status = DAQmxCreateAIVoltageChan(taskHandle, kTorqueChannel, "TorqueDataChannel", DAQmx_Val_RSE, -10, 10, DAQmx_Val_Volts, NULL);
	status = DAQmxCfgSampClkTiming(taskHandle, "OnboardClock", 1000, DAQmx_Val_Rising, DAQmx_Val_ContSamps, 10);
	status = DAQmxStartTask(taskHandle);
	status = DAQmxReadAnalogF64(taskHandle, 1, 0.2, DAQmx_Val_GroupByScanNumber, torque_data, 2, &read, NULL);
	status = DAQmxStopTask(taskHandle);
	status = DAQmxClearTask(taskHandle);

	/*这里需要根据实际的连线确定正确的顺序*/
	shoulder_raw_torque_ = torque_data[1];
	elbow_raw_torque_ = torque_data[0];
}

void DataAcquisition::AcquisitePullSensorData() {
	TaskHandle taskHandle = 0;
	int32 read = 0;
	int status = 0;
	double pull_sensor_data[4] { 0 };
	status = DAQmxCreateTask("PullDataTask", &taskHandle);
	status = DAQmxCreateAIVoltageChan(taskHandle, kPullSensorChannel, "PullDataChannel", DAQmx_Val_RSE, -10, 10, DAQmx_Val_Volts, NULL);
	status = DAQmxCfgSampClkTiming(taskHandle, "OnboardClock", 1000, DAQmx_Val_Rising, DAQmx_Val_ContSamps, 10);
	status = DAQmxStartTask(taskHandle);
	status = DAQmxReadAnalogF64(taskHandle, 1, 0.2, DAQmx_Val_GroupByScanNumber, pull_sensor_data, 4, &read, NULL);
	status = DAQmxStopTask(taskHandle);
	status = DAQmxClearTask(taskHandle);

	/*这里需要根据实际连线确定正确的顺序*/
	shoulder_raw_forward_pull_ = pull_sensor_data[0];
	shoulder_raw_backward_pull_ = pull_sensor_data[1];
	elbow_raw_forward_pull_ = pull_sensor_data[2];
	elbow_raw_backward_pull_ = pull_sensor_data[3];
}

double DataAcquisition::ShoulderTorque() {
	return kRawToReal * shoulder_raw_torque_;
}

double DataAcquisition::ElbowTorque() {
	return kRawToReal * elbow_raw_torque_;
}

double DataAcquisition::ShoulderForwardPull() {
	return kRawToReal * shoulder_raw_forward_pull_;
}

double DataAcquisition::ShoulderBackwardPull() {
	return kRawToReal * shoulder_raw_backward_pull_;
}

double DataAcquisition::ElbowForwardPull() {
	return kRawToReal * elbow_raw_forward_pull_;
}

double DataAcquisition::ElbowBackwardPull() {
	return kRawToReal * elbow_raw_backward_pull_;
}