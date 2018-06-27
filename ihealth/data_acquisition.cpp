#include "data_acquisition.h"
#include <Eigen/core>

using namespace Eigen;

const char *DataAcquisition::kTorqueChannel = "dev3/ai18:19";
const char *DataAcquisition::kPullSensorChannel = "dev3/ai0:3";
const char *DataAcquisition::kSixDimensionForceChannel = "dev1/ai0:5";
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
	double torque_data[2]{ 0 };
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
	double pull_sensor_data[4]{ 0 };
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

void DataAcquisition::AcquisiteSixDemensionData(double output_buf[6]) {
	TaskHandle taskHandle = 0;
	int32 read = 0;
	int status = 0;
	double raw_data[6];
	status = DAQmxCreateTask("SixDemensionDataTask", &taskHandle);
	status = DAQmxCreateAIVoltageChan(taskHandle, kSixDimensionForceChannel, "PullDataChannel", DAQmx_Val_Diff, -10, 10, DAQmx_Val_Volts, NULL);
	status = DAQmxCfgSampClkTiming(taskHandle, "OnboardClock", 1000, DAQmx_Val_Rising, DAQmx_Val_ContSamps, 10);
	status = DAQmxStartTask(taskHandle);
	status = DAQmxReadAnalogF64(taskHandle, 1, 0.2, DAQmx_Val_GroupByScanNumber, raw_data, 6, &read, NULL);
	status = DAQmxStopTask(taskHandle);
	status = DAQmxClearTask(taskHandle);

	//计算
	Matrix<double, 6, 6> m;
	m << 0.08729, -0.00137, 0.59422, 47.13000, 0.06298, -47.92455,
		0.77953, -54.66462, 0.13566, 27.25556, 0.51927, 27.55145,
		65.33104, 1.23854, 64.00936, -0.70825, 65.17850, -1.71349,
		-0.06931, -0.07835, -1.20443, 0.02817, 1.08440, 0.01389,
		1.34213, 0.01130, -0.59052, -0.04531, -0.61663, 0.05584,
		-0.01694, 1.06263, 0.00710, 1.01083, -0.00028, 0.94187;
	Matrix<double, 6, 1> dat;
	for (int i = 0; i < 6; ++i) {
		dat(i, 0) = raw_data[i];
	}

	VectorXd result(6);
	result = m * dat;

	//减去偏置
	VectorXd bias(6);
	bias << 12.0295, 18.6972, -82.3837, -1.92185, 1.24067, 0.6300;
	result = result - bias;

	for (int i = 0; i < 6; ++i) {
		output_buf[i] = result(i);
	}
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