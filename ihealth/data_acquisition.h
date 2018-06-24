#pragma once
#include <NIDAQmx.h>

class DataAcquisition {
public:
	static DataAcquisition &GetInstacne();
	DataAcquisition(const DataAcquisition &) = delete;
	DataAcquisition(DataAcquisition &&) = delete;
	DataAcquisition &operator=(const DataAcquisition &) = delete;
	DataAcquisition &operator=(DataAcquisition &&) = delete;

	void AcquisiteTorqueData();
	void AcquisitePullSensorData();
	double ShoulderTorque();
	double ElbowTorque();
	double ShoulderForwardPull();
	double ShoulderBackwardPull();
	double ElbowForwardPull();
	double ElbowBackwardPull();

private:
	DataAcquisition();

private:
	double shoulder_raw_torque_ = 0.0;
	double elbow_raw_torque_ = 0.0;

	double shoulder_raw_forward_pull_ = 0.0;
	double shoulder_raw_backward_pull_ = 0.0;
	double elbow_raw_forward_pull_ = 0.0;
	double elbow_raw_backward_pull_ = 0.0;

	static const char *kTorqueChannel;
	static const char *kPullSensorChannel;
	static const double kRawToReal;
};