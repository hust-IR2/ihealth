#include "eyeTracking.h"
#include <assert.h>
EyeTracking::EyeTracking() :mPupilCenter(cv::Point2d(0, 0))
{
	pEyeImage = NULL;

	cgf.read("../../resource/params/pupil.cfg");
	params.Radius_Min = cgf.get<int>("Radius_Min");
	params.Radius_Max = cgf.get<int>("Radius_Max");

	params.CannyBlur = cgf.get<int>("CannyBlue");
	params.CannyThreshold1 = cgf.get<int>("CannyThreshold1");
	params.CannyThreshold2 = cgf.get<int>("CannyThreshold2");
	params.StarburstPoints = cgf.get<int>("StarburstPoints");

	params.PercentageInliers = cgf.get<int>("PercentageInliers");
	params.InlierIterations = cgf.get<int>("InlierIterations");
	params.ImageAwareSupport = cgf.get<bool>("ImageAwareSupport");
	params.EarlyTerminationPercentage = cgf.get<int>("EarlyTerminationPercentage");
	params.EarlyRejection = cgf.get<int>("EarlyRejection");
	params.Seed = cgf.get<int>("Seed");

}

EyeTracking::~EyeTracking()
{
	if (pEyeImage != NULL)
		delete pEyeImage;
}

int EyeTracking::pupilDetect() {
	pupiltracker::findPupilEllipse_out out;
	pupiltracker::tracker_log log;

	if (pupiltracker::findPupilEllipse(params, *pEyeImage, out, log)) {
		mPupilCenter = out.pPupil;
		mPupilEllipse = out.elPupil;
		return 0;
	}
	return -1;
}
void EyeTracking::setEyeImage(const cv::Mat& _image)
{
	if (pEyeImage == NULL) {
		mWidth = _image.cols;
		mHeight = _image.rows;
		pEyeImage = new cv::Mat(_image.size(), CV_8UC3);
	}
	*pEyeImage = _image.clone();
}

const cv::Mat EyeTracking::getEyeImage() const
{
	return cv::Mat(*pEyeImage);
}

const cv::Point2d EyeTracking::getPupilCenter() const
{
	return this->mPupilCenter;
}

const cv::RotatedRect EyeTracking::getPupilEllipse() const {
	return mPupilEllipse;
}