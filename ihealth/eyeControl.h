#pragma once
#include "eyeTracking.h"

class EyeControl
{
public:
	EyeControl(int _step0 = 200, int _step1 = 200 , double _maxAngle0 = 50, double _maxAngle1 = 50, double _minSpeed = 30, double _maxSpeed = 70, double _lambda = 0.5);
	~EyeControl();

	void setSteps0(int _step);
	void setSteps1(int _step);

	void setMaxAngle0(double _angle);
	void setMaxAngle1(double _angle);

	double getAngle0();
	double getAngle1();
	std::pair<double, double> getAngles();

	void calibrate();

	cv::RotatedRect getLeftEyeEllipse();
	cv::RotatedRect getRightEyeEllipse();

	cv::Point getLeftCalibCenter();
	cv::Point getRightCalibCenter();

	void setControlSpeed(double _lambda); // _lambda is 0->1
	void setMinSpeed(double _min);
	void setMaxSpeed(double _max);

	void initRobotSurface();
	void update(const cv::Mat& _leftEye, const cv::Mat& _rightEye, double* _currentAngles, int controlMode = 1);

private:
	EyeTracking* pLeftEye; // !TODO  set the access to the setting
	EyeTracking*  pRightEye;
	cv::RotatedRect mLeftEyeEllipse;
	cv::RotatedRect mRightEyeEllipse;
	cv::Point mLeftEyeCalibCenter;
	cv::Point mRightEyeCalibCenter;
	double mAngle0;
	double mAngle1;
	int mSteps0; // divide mAngle0 to the number
	int mSteps1; // divide mAngle1 to the number
	double mMaxAngle0; // max value of mAngle0 to calculate the surface
	double mMaxAngle1; // max value of mAngle1 to calculate the surface

	double mLambdaSpeed; // 0->1, adjust the speed value between min and max.
	double mMinSpeed;
	double mMaxSpeed;
	
	cv::Mat* pRobotSurface; // save the 3D surface data of robot manipulation space
};
