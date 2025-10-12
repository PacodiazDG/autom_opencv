#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Cargar la imagen
    cv::Mat imagen = cv::imread("imagen.jpg");
    if (imagen.empty()) {
        std::cerr << "No se pudo abrir la imagen." << std::endl;
        return -1;
    }

    // Convertir de BGR a HSV
    cv::Mat hsv;
    cv::cvtColor(imagen, hsv, cv::COLOR_BGR2HSV);

    // Rango de color rojo (ajustable)
    cv::Scalar rojo_bajo1(0, 120, 70);
    cv::Scalar rojo_alto1(10, 255, 255);
    cv::Scalar rojo_bajo2(170, 120, 70);
    cv::Scalar rojo_alto2(180, 255, 255);

    // Crear máscaras (rojo tiene dos rangos en HSV)
    cv::Mat mask1, mask2;
    cv::inRange(hsv, rojo_bajo1, rojo_alto1, mask1);
    cv::inRange(hsv, rojo_bajo2, rojo_alto2, mask2);

    // Combinar ambas máscaras
    cv::Mat mask = mask1 | mask2;

    // Aplicar la máscara a la imagen original
    cv::Mat resultado;
    cv::bitwise_and(imagen, imagen, resultado, mask);

    // Mostrar resultados
    cv::imshow("Original", imagen);
    cv::imshow("Mascara", mask);
    cv::imshow("Color Detectado", resultado);

    cv::waitKey(0);
    return 0;
}
