#include "RTL.hpp"
#include <iostream>

using namespace std;

// The main function
int main(void)
{
    // Generate noisy data from the truth
    Line trueModel(0.6, 0.8, -300);
    vector<int> trueInliers;
    LineObserver observer;
    vector<Point> data = observer.GenerateData(trueModel, 100, trueInliers, 0.1, 0.6);
    if (data.empty()) return -1;

    // Find the best model using RANSAC
    LineEstimator estimator;
    RTL::RANSAC<Line, Point, vector<Point> > ransac(&estimator);
    Line model;
    double loss = ransac.FindBest(model, data, data.size(), 2);

    // Determine inliers using the best model if necessary
    vector<int> inliers = ransac.FindInliers(model, data, data.size());

	// Print the result
    cout << "- True Model:  " << trueModel << endl;
    cout << "- Found Model: " << model << " (Loss: " << loss << ")" << endl;
    cout << "- The Number of Inliers: " << inliers.size() << " (N: " << data.size() << ")" << endl;

    return 0;
}
