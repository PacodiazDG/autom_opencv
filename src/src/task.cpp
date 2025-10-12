#include <iostream>
#include <opencv2/opencv.hpp>
#include "processing.hxx"
#include "task.hxx"
#include <serial.hxx>


#define DEBUG true
int task::task_function()
{
    // If DEBUG is defined we use the laptop camera (index 0).
    // Otherwise we attempt to open an ESP32-CAM HTTP stream URL.
#ifndef DEBUG
    // Example ESP32-CAM stream URL (modify to your device IP/path)
    const std::string esp32_url = "http://192.168.4.1:81/stream"; // change as needed
    cv::VideoCapture cap(esp32_url);
#else
    // Abrir la cámara local (0 es el índice de la cámara por defecto)
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

    // Use the Processing API that accepts a cv::Mat frame directly.
    proc.setImage(frame); // setImage() ahora acepta cv::Mat (desde processing.hxx)

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
                    // Notify Arduino over serial
                    if (!Serial::sendMessage("ORANGE\n"))
                    {
                        std::cerr << "Error enviando mensaje por serial." << std::endl;
                    }
                    std::cout << "Color naranja & operario detectado" << std::endl;
                }
            }
        }

        // Mostrar resultados visuales
        proc.showResults("DetectorColores");

        // Salir con ESC
        if (cv::waitKey(30) == 27)
            break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}

