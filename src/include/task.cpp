#include <iostream>
#include <opencv2/opencv.hpp>
#include "processing.hxx"
#include "task.hxx"
#include "serial.hxx"


#define DEBUG true
int task::task_function()
{
    
    
#ifndef DEBUG
    
    const std::string esp32_url = "http://192.168.4.1:81/stream"; 
    cv::VideoCapture cap(esp32_url);
#else
    
    cv::VideoCapture cap(0);
#endif
    if (!cap.isOpened())
    {
        std::cerr << "No se pudo abrir la cámara." << std::endl;
        return -1;
    }

    Processing proc;
    cv::Mat frame;

    std::cout << "Presiona ESC para salir.\n";

    while (true)
    {
        cap >> frame;
        if (frame.empty())
        {
            std::cerr << "Error capturando frame." << std::endl;
            break;
        }

    
    proc.setImage(frame); 

        auto detected = proc.detectColors(0.005);
        if (detected.empty())
        {
            std::cout << "No se detectaron colores relevantes." << std::endl;
        }
        else
        {
            std::cout << "Colores detectados:" << std::endl;
            for (auto &p : detected)
            {
                std::cout << " - " << p.first << ": " << (p.second * 100.0) << "%" << std::endl;
                if (p.first == "orange")
                {
                    std::cout << "Color naranja & operario detectado" << std::endl;
                    
                    if (!Serial::sendMessage("ORANGE\n"))
                    {
                        std::cerr << "Failed to send serial message" << std::endl;
                    }
                }
            }
        }

        
        proc.showResults("DetectorColores");

        
        if (cv::waitKey(30) == 27)
            break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}

