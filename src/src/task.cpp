#include <iostream>
#include <opencv2/opencv.hpp>
#include "processing.hxx"
#include "task.hxx"
#include <serial.hxx>
#include <unistd.h>
using namespace zbar;

int task::task_function()
{

#ifndef DEBUG

    const std::string esp32_url = "http://10.0.1.58:81/stream";
    cv::VideoCapture cap(esp32_url, cv::CAP_FFMPEG);
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
            sleep(3);
            std::cout << "Reintentando...\n";
            continue;
        }

        proc.setImage(frame);
        detectCodes(frame);
#ifdef Operador
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

                    if (!Serial::sendMessage("ORANGE\n"))
                    {
                        std::cerr << "Error enviando mensaje por serial." << std::endl;
                    }
                    std::cout << "Color naranja & operario detectado" << std::endl;
                    sleep(50);
                }
            }
        }
#endif

#ifndef realses
        proc.showResults("DetectorColores");

        if (cv::waitKey(30) == 27)
            break;
    }
#endif

    cap.release();
    cv::destroyAllWindows();
    return 0;
}

std::vector<std::string> task::detectCodes(const cv::Mat &frame)
{
    std::vector<std::string> results;
    if (frame.empty())
    {
        std::cerr << "Frame vacío, no se puede procesar." << std::endl;
        return results;
    }
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    ImageScanner scanner;
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
    Image zbarImage(
        gray.cols,
        gray.rows,
        "Y800",
        gray.data,
        gray.cols * gray.rows);

    int n = scanner.scan(zbarImage);
    if (n <= 0)
    {
        return results;
    }
    for (auto it = zbarImage.symbol_begin(); it != zbarImage.symbol_end(); ++it)
    {
        std::string type = it->get_type_name();
        std::string data = it->get_data();
        std::cout << "[Detectado] Tipo: " << type << " | Dato: " << data << std::endl;
        results.push_back(data);
    }
    return results;
}
