#include <iostream>
#include "Line.hpp"
#include "RANSAC.hpp"

// The main function
int main(void)
{
    // Generate noisy data from the truth
    LineObserver observer;
    std::vector<Point> data;
    std::vector<int> trueInliers;
    Line trueModel(0.6, -0.8, 1);
    if (!observer.GenerateData(data, trueInliers, trueModel, 100, 0.1, 0.5)) return -1;

    // Print the data
    for (size_t i = 0; i < data.size(); i++)
        std::cout << data[i] << std::endl;

    // Find the best model using RANSAC
    LineEstimator estimator;
    RTL::RANSAC<Line, std::vector<Line>, Point, std::vector<Point> > ransac(&estimator);
    Line bestModel;
    double bestLoss = ransac.FindBest(bestModel, data, data.size());

    // Find inliers using the best model
    std::vector<int> inliers;
    int numInliers = ransac.FindInliers(inliers, bestModel, data, data.size());

    // Print the result
    std::cout << "- True Model:  " << trueModel << std::endl;
    std::cout << "- Found Model: " << bestModel << " (Loss: " << bestLoss << ")" << std::endl;
    std::cout << "- The Number of Inliers: " << numInliers << std::endl;

    return 0;
}
