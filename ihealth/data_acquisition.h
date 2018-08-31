#pragma once
#include <NIDAQmx.h>

#include <Eigen/core>

class DataAcquisition {
public:
	static DataAcquisition &GetInstance();
	DataAcquisition(const DataAcquisition &) = delete;
	DataAcquisition(DataAcquisition &&) = delete;
	DataAcquisition &operator=(const DataAcquisition &) = delete;
	DataAcquisition &operator=(DataAcquisition &&) = delete;

	void AcquisiteTorqueData();
	void AcquisitePullSensorData();
	void AcquisiteSixDemensionData(double output_buf[6]);
	void AcquisiteGripData(double grip[1]);
	double ShoulderTorque();
	double ElbowTorque();
	double ShoulderForwardPull();
	double ShoulderBackwardPull();
	double ElbowForwardPull();
	double ElbowBackwardPull();

	//bool StartTask();
	//bool StopTask();

private:
	DataAcquisition();

private:
	TaskHandle m_task_handle;

	double shoulder_raw_torque_ = 0.0;
	double elbow_raw_torque_ = 0.0;

	double shoulder_raw_forward_pull_ = 0.0;
	double shoulder_raw_backward_pull_ = 0.0;
	double elbow_raw_forward_pull_ = 0.0;
	double elbow_raw_backward_pull_ = 0.0;

	static const char *kTorqueChannel;
	static const char *kPullSensorChannel;
	static const char *kSixDimensionForceChannel;
	static const char *kGripChannel;
	static const double kRawToReal;

	static Eigen::Matrix<double, 6, 6>  kTransformMatrix;
};