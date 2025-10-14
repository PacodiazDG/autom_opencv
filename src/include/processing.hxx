
#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

class Processing
{
public:
	Processing();

	bool loadImage(const std::string &path);

	void setImage(const cv::Mat &img);

	void detectRed();

	std::vector<std::pair<std::string, double>> detectColors(double minFraction = 0.01);

	void showResults(const std::string &baseWindowName = "Processing");

	const cv::Mat &original() const { return img_; }
	const cv::Mat &mask() const { return mask_; }
	const cv::Mat &result() const { return result_; }

private:
	cv::Mat img_;
	cv::Mat hsv_;
	cv::Mat mask_;
	cv::Mat result_;
};
