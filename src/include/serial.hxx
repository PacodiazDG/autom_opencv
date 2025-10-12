#include <vector>
#include <utility>
#include "processing.hxx"

// Small helper class that encapsulates image loading and red color detection
class  Serial {
public:
    static int Open();

protected:
private:
	cv::Mat img_;
	cv::Mat hsv_;
	cv::Mat mask_;
	cv::Mat result_;
};
