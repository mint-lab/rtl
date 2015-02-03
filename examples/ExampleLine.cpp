#include <iostream>
#include <Line.hpp>
#include <RANSAC.hpp>

int main(void)
{
    const int N = 100;

    // Generate noisy data from the truth
    LineObserver observer;
    std::vector<Point2> data;
    std::vector<int> trueInliers;
    Line trueModel(0.6, -0.8, 1);
    if (!observer.GenerateData(data, trueInliers, trueModel, N, 0.1, 0.5)) return -1;

    // Print the data
    for (int i = 0; i < N; i++)
        std::cout << data[i] << std::endl;

    // Find the best model using RANSAC
    LineEstimator estimator;
    RTL::RANSAC<Line, std::vector<Line>, Point2, std::vector<Point2> > ransac(&estimator);
    Line bestModel;
    double bestLoss = ransac.FindBest(bestModel, data, N);

    // Print the result
    std::cout << "- True Model:  " << trueModel << std::endl;
    std::cout << "- Found Model: " << bestModel << " (Loss: " << bestLoss << ")" << std::endl;

    return 0;
}