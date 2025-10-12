// processing.cpp
#include "processing.hxx"
#include <iostream>

Processing::Processing() {}

bool Processing::loadImage(const std::string &path) {
	img_ = cv::imread(path);
	if (img_.empty()) {
		std::cerr << "No se pudo abrir la imagen: " << path << std::endl;
		return false;
	}
	return true;
}

void Processing::detectRed() {
	if (img_.empty()) return;

	// Convertir de BGR a HSV
	cv::cvtColor(img_, hsv_, cv::COLOR_BGR2HSV);

	// Rango de color rojo (ajustable)
	cv::Scalar rojo_bajo1(0, 120, 70);
	cv::Scalar rojo_alto1(10, 255, 255);
	cv::Scalar rojo_bajo2(170, 120, 70);
	cv::Scalar rojo_alto2(180, 255, 255);

	// Crear máscaras (rojo tiene dos rangos en HSV)
	cv::Mat mask1, mask2;
	cv::inRange(hsv_, rojo_bajo1, rojo_alto1, mask1);
	cv::inRange(hsv_, rojo_bajo2, rojo_alto2, mask2);

	// Combinar ambas máscaras
	mask_ = mask1 | mask2;

	// Aplicar la máscara a la imagen original
	cv::bitwise_and(img_, img_, result_, mask_);
}
// Helper: compute fraction of non-zero pixels in mask
static double fractionNonZero(const cv::Mat &m) {
	if (m.empty()) return 0.0;
	double nonZero = cv::countNonZero(m);
	double total = m.rows * m.cols;
	if (total <= 0) return 0.0;
	return nonZero / total;
}

std::vector<std::pair<std::string, double>> Processing::detectColors(double minFraction) {
	std::vector<std::pair<std::string, double>> results;
	if (img_.empty()) return results;

	cv::cvtColor(img_, hsv_, cv::COLOR_BGR2HSV);

	// Define color ranges in HSV: name -> pair(lower, upper)
	struct Range { cv::Scalar low, high; };
	// We'll treat red as two ranges (handled separately)
	std::vector<std::pair<std::string, std::vector<Range>>> colors;

	colors.push_back({"red", { {cv::Scalar(0, 120, 70), cv::Scalar(10, 255, 255)}, {cv::Scalar(170,120,70), cv::Scalar(180,255,255)} }});
	colors.push_back({"green", { {cv::Scalar(36, 50, 70), cv::Scalar(89, 255, 255)} }});
	colors.push_back({"blue", { {cv::Scalar(90, 50, 70), cv::Scalar(128, 255, 255)} }});
	colors.push_back({"yellow", { {cv::Scalar(15, 100, 100), cv::Scalar(35, 255, 255)} }});
	colors.push_back({"orange", { {cv::Scalar(10, 100, 20), cv::Scalar(24, 255, 255)} }});
	colors.push_back({"purple", { {cv::Scalar(129, 50, 70), cv::Scalar(169, 255, 255)} }});
	// light/dark detection (white/black)
	colors.push_back({"white", { {cv::Scalar(0, 0, 200), cv::Scalar(180, 25, 255)} }});
	colors.push_back({"black", { {cv::Scalar(0, 0, 0), cv::Scalar(180, 255, 30)} }});

	// Keep a combined mask for visualization
	cv::Mat combinedMask = cv::Mat::zeros(img_.rows, img_.cols, CV_8UC1);

	for (auto &c : colors) {
		cv::Mat colorMask = cv::Mat::zeros(img_.rows, img_.cols, CV_8UC1);
		for (auto &r : c.second) {
			cv::Mat tmp;
			cv::inRange(hsv_, r.low, r.high, tmp);
			colorMask |= tmp;
		}
		double frac = fractionNonZero(colorMask);
		if (frac >= minFraction) {
			results.push_back({c.first, frac});
			combinedMask |= colorMask;
		}
	}

	// Store combined mask and result image
	mask_ = combinedMask;
	if (!combinedMask.empty()) cv::bitwise_and(img_, img_, result_, combinedMask);
	else result_ = cv::Mat();

	return results;
}

void Processing::showResults(const std::string &baseWindowName) {
	if (img_.empty()) return;

	cv::imshow(baseWindowName + " - Original", img_);
	if (!mask_.empty()) cv::imshow(baseWindowName + " - Mascara", mask_);
	if (!result_.empty()) cv::imshow(baseWindowName + " - Color Detectado", result_);
}
