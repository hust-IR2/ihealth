#pragma once
#include <opencv2/opencv.hpp>
#include <pupiltracker/PupilTracker.h>
#include <pupiltracker/cvx.h>
#include <pupiltracker/utils.h>
#define _EYE_DEBUG_ 0
using namespace std;

class EyeTracking
{
public:
	EyeTracking();
	~EyeTracking();
	int pupilDetect();

	void setEyeImage(const cv::Mat& _image);
	const cv::Mat getEyeImage() const;

	const cv::Point2d getPupilCenter() const;
	const cv::RotatedRect getPupilEllipse() const;
private:
	pupiltracker::TrackerParams params;
	pupiltracker::ConfigFile cgf;

	int mWidth;
	int mHeight;
	cv::Mat *pEyeImage;
	cv::RotatedRect mPupilEllipse;

	cv::Point2d mPupilCenter;
};
