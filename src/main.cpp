#include <iostream>
#include "include/processing.hxx"

int main(int argc, char **argv) {
    std::string path = "imagen.jpg";
    if (argc > 1) path = argv[1];

    Processing proc;
    if (!proc.loadImage(path)) return -1;

    auto detected = proc.detectColors(0.005);
    if (detected.empty()) {
        std::cout << "No se detectaron colores relevantes." << std::endl;
    } else {
        std::cout << "Colores detectados:" << std::endl;
        for (auto &p : detected) {
            std::cout << " - " << p.first << ": " << (p.second * 100.0) << "%" << std::endl;
            if (p.first=="orange"){
                std::cout <<"Color  naranja & operario detectado"<<std::endl;
            }
        }
    }

    //proc.showResults("DetectorColores");

    cv::waitKey(0);
    return 0;
}
