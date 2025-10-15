#include <iostream>
#include <opencv2/opencv.hpp>
#include "processing.hxx"
#include <serial.hxx>
#include <unistd.h> 
#include <zbar.h>


//#define DEBUG true
#define Operador true
#define realse true
class task
{
private:
    /* data */
public:
 static int task_function();
 static std::vector<std::string> detectCodes(const cv::Mat& frame);
};

