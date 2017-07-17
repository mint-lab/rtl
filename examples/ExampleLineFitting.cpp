#include <iostream>
#include <iomanip>
#include "RTL.hpp"

// The main function
int main(void)
{
    // Generate noisy data from the truth
    LineObserver observer;
    std::vector<Point> data;
    std::vector<bool> trueInliers;
    Line trueModel(0.6, -0.8, 1);
    if (!observer.GenerateData(data, trueInliers, trueModel, 100, 0.1, 0.6)) return -1;

    // Find the best model using RANSAC
    LineEstimator estimator;
    RTL::RANSAC<Line, std::vector<Line>, Point, std::vector<Point> > ransac(&estimator);
    Line bestModel;
    double bestLoss = ransac.FindBest(bestModel, data, data.size());

    // Find inliers using the best model
    std::vector<bool> inliers;
    int numInliers = ransac.FindInliers(inliers, bestModel, data, data.size());

    // Print the data
	std::ios_base::fmtflags original_flags = std::cout.flags();

	std::cout << " No:      x         y     true ransac  xor" << std::endl;
    for (size_t i = 0; i < data.size(); i++) {
		std::cout.setf(std::ios_base::right, std::ios_base::adjustfield);
		std::cout.width(3);
        std::cout << i << ": ";
		std::cout.width(8);
		std::cout.setf(std::ios_base::right, std::ios_base::adjustfield);
		std::cout << std::fixed << std::setprecision(2) << data[i].x << ", ";
		std::cout.width(8);
		std::cout.setf(std::ios_base::right, std::ios_base::adjustfield);
		std::cout << std::fixed << std::setprecision(2) << data[i].y;
		std::cout << "    " << (trueInliers[i] ? "O" : "X");
		std::cout << "    " << (inliers[i] ? "O" : "X");
		std::cout << "    " << ((trueInliers[i] ^ inliers[i]) ? "*" : " ");
		std::cout << std::endl;
	}

	std::cout.flags(original_flags);

	// Print the result
    std::cout << "- True Model:  " << trueModel << std::endl;
    std::cout << "- Found Model: " << bestModel << " (Loss: " << bestLoss << ")" << std::endl;
    std::cout << "- The Number of Inliers: " << numInliers << std::endl;

    return 0;
}
