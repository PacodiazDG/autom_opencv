// processing.hxx
#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <utility>

// Small helper class that encapsulates image loading and red color detection
class Processing {
public:
	Processing();
	// Load an image from disk. Returns true if successful.
	bool loadImage(const std::string &path);
	// Set image directly from a cv::Mat (e.g., camera frame or network stream)
	void setImage(const cv::Mat &img);
	// Run the red color detection pipeline
	void detectRed();
	// Detect multiple colors and return a vector of (color name, fraction of image [0..1])
	// minFraction: minimum fraction of pixels to report a color (default 1%)
	std::vector<std::pair<std::string, double>> detectColors(double minFraction = 0.01);
	// Show the original, mask and result windows
	void showResults(const std::string &baseWindowName = "Processing");

	// Accessors
	const cv::Mat &original() const { return img_; }
	const cv::Mat &mask() const { return mask_; }
	const cv::Mat &result() const { return result_; }

private:
	cv::Mat img_;
	cv::Mat hsv_;
	cv::Mat mask_;
	cv::Mat result_;
};
