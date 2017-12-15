#include "eyeControl.h"
#include "Matrix.h"
EyeControl::EyeControl(int _step0 , int _step1, double _maxAngle0, double _maxAngle1, double _minSpeed, double _maxSpeed, double _lambda):
	mAngle0(0),
	mAngle1(0),
	mSteps0(_step0),
	mSteps1(_step1),
	mMaxAngle0(_maxAngle0),
	mMaxAngle1(_maxAngle1),
	mMinSpeed(_minSpeed),
	mMaxSpeed(_maxSpeed),
	mLambdaSpeed(_lambda)
{
	pLeftEye = NULL;
	pLeftEye = new EyeTracking;
	pRightEye = NULL;
	pRightEye = new EyeTracking;
	pRobotSurface = new cv::Mat(mSteps0, mSteps1, CV_64FC3);
	initRobotSurface();
}

EyeControl::~EyeControl()
{
	delete pLeftEye;
	delete pRightEye;
	delete pRobotSurface;
}

void EyeControl::setSteps0(int _step)
{
	mSteps0 = _step;
}

void EyeControl::setSteps1(int _step)
{
	mSteps1 = _step;
}

void EyeControl::setMaxAngle0(double _angle)
{
	mMaxAngle0 = _angle;
}

void EyeControl::setMaxAngle1(double _angle)
{
	mMaxAngle1 = _angle;
}

double EyeControl::getAngle0()
{
	return mAngle0;
}

double EyeControl::getAngle1()
{
	return mAngle1;
}

std::pair<double, double> EyeControl::getAngles()
{
	return std::pair<double, double>(mAngle0, mAngle1);
}

void EyeControl::calibrate()
{
	mLeftEyeCalibCenter = cv::Point(mLeftEyeEllipse.center.x, mLeftEyeEllipse.center.y);
	mRightEyeCalibCenter = cv::Point(mRightEyeEllipse.center.x, mRightEyeEllipse.center.y);
	//AllocConsole();

}
cv::RotatedRect EyeControl::getLeftEyeEllipse()
{
	return mLeftEyeEllipse;
}

cv::RotatedRect EyeControl::getRightEyeEllipse()
{
	return mRightEyeEllipse;
}

cv::Point EyeControl::getLeftCalibCenter() {
	return mLeftEyeCalibCenter;
}
cv::Point EyeControl::getRightCalibCenter() {
	return mRightEyeCalibCenter;
}
void EyeControl::setControlSpeed(double _lambda)
{
	mLambdaSpeed = _lambda;
}

void EyeControl::setMinSpeed(double _min)
{
	mMinSpeed = _min;
}

void EyeControl::setMaxSpeed(double _max)
{
	mMaxSpeed = _max;
}

void EyeControl::initRobotSurface()
{
	for (int i = 0; i < mSteps0; i++) {
		for (int j = 0; j < mSteps1; j++)
		{
			MatrixXd motorU(2, 1);
			MatrixXd theta(6, 1);
			motorU(0) = mMaxAngle0 * i / mSteps0;
			motorU(1) = mMaxAngle1 * j / mSteps1;
			fwd_geo_coup(motorU, theta);
			MatrixXd T0h(4, 4);
			fwd_geo_kineB(theta, T0h);
			pRobotSurface->at<cv::Vec3d>(i, j)[0] = T0h(0, 3);
			pRobotSurface->at<cv::Vec3d>(i, j)[1] = T0h(1, 3);
			pRobotSurface->at<cv::Vec3d>(i, j)[2] = T0h(2, 3);

			//AllocConsole();
			//freopen("CONOUT$", "w", stdout);
			//printf("mSteps : %d, %d\n", i, j);
			//printf("pRobotSurface x: %lf\n", pRobotSurface->at<cv::Vec3d>(i, j)[0]);
			//printf("pRobotSurface y: %lf\n", pRobotSurface->at<cv::Vec3d>(i, j)[1]);
			//printf("pRobotSurface z: %lf\n\n", pRobotSurface->at<cv::Vec3d>(i, j)[2]);
		}
	}
}

void EyeControl::update(const cv::Mat& _leftEyeData, const cv::Mat& _rightEyeData, double* _currentAngles, int controlMode)
{
	if (_currentAngles[0] > mMaxAngle0)_currentAngles[0] = mMaxAngle0;
	else if (_currentAngles[0] < 0)_currentAngles[0] = 0;
	if (_currentAngles[1] > mMaxAngle1)_currentAngles[1] = mMaxAngle1;
	else if (_currentAngles[1] < 0)_currentAngles[1] = 0;
	pLeftEye->setEyeImage(_leftEyeData);
	pRightEye->setEyeImage(_rightEyeData);
	// get pupil center
	pRightEye->pupilDetect();
	mRightEyeEllipse = pRightEye->getPupilEllipse();
	pLeftEye->pupilDetect();
	mLeftEyeEllipse = pLeftEye->getPupilEllipse();

	cv::Point2d BiMeanDirection;
	cv::Point3d controlDirection;

	cv::Point pupilDirectionL, pupilDirectionR, depthDireciton;
	pupilDirectionL = cv::Point(mLeftEyeEllipse.center.x, mLeftEyeEllipse.center.y) - mLeftEyeCalibCenter;
	pupilDirectionR = cv::Point(mRightEyeEllipse.center.x, mRightEyeEllipse.center.y) - mRightEyeCalibCenter;

	BiMeanDirection = (pupilDirectionL + pupilDirectionR) / 2;


	//    double deptIndex = 2 ;


	int minDisI = 1000;
	int minDisJ = 1000;
	double minDistance = 10000;

	/*feedback control: head pose to the object, use eye to guide the robot to the object,
	* as which means that the robot move just towards the  opposite direction of the
	* pupilDirection
	*/
	controlDirection.y = BiMeanDirection.x;
	controlDirection.x = BiMeanDirection.y;
	// controlDirection.z = -deptIndex*(pupilDirectionL.x - pupilDirectionR.x);
	controlDirection.z = 0;
	//current point in the workspace index

	double controlSpeed = (mMaxSpeed - mMinSpeed) * mLambdaSpeed + mMinSpeed;
	controlDirection = controlDirection / norm(controlDirection) * controlSpeed;

	int i = _currentAngles[0] / mMaxAngle0 * (mSteps0 - 1);
	int j = _currentAngles[1] / mMaxAngle1 * (mSteps1 - 1);

	/*search the nearest point in the robot workspace in a neighr area of 1/20 of the total workspace to the next controled point
	init the current point as the nearest point*/
	const int radius = 10;
	int iRange[2] = { (i - mSteps0 / radius) < 0 ? 0 : (i - mSteps0 / radius), (i + mSteps0 / radius) < mSteps0 ? (i + mSteps0 / radius) : mSteps0 };
	int jRange[2] = { (j - mSteps1 / radius) < 0 ? 0 : (j - mSteps1 / radius), (j + mSteps1 / radius) < mSteps1 ? (j + mSteps1 / radius) : mSteps1 };
	for (int tempI = iRange[0]; tempI < iRange[1]; tempI++) {
		for (int tempJ = jRange[0]; tempJ < jRange[1]; tempJ++) {
			if (tempI == i&&tempJ == j)
				continue;
			double dis = norm(pRobotSurface->at<cv::Point3d>(i, j) + controlDirection - pRobotSurface->at<cv::Point3d>(tempI, tempJ));
			if (dis < minDistance) {
				minDisI = tempI;
				minDisJ = tempJ;
				minDistance = dis;
			}
		}
	}
	mAngle0 = minDisI * mMaxAngle0 / mSteps0;
	mAngle1 = minDisJ * mMaxAngle1 / mSteps1;
	/*AllocConsole();
	freopen("CONOUT$", "w", stdout);
	printf("controlDirection : %lf %lf\n", controlDirection.x, controlDirection.y);
	printf("minDisIJ : %d %d\n", minDisI, minDisJ);
	printf("mAngle0 mAngle1: %lf, %lf\n", mAngle0, mAngle1);
	printf("_currentAngles: %lf, %lf\n\n", _currentAngles[0], _currentAngles[1]);*/
}