#include "RTL.hpp"
#include <iostream>

using namespace std;

// A mean calculator
class MeanEstimator : public RTL::Estimator<double, double, vector<double> >
{
public:
    // Calculate the mean of data at the sample indices
    virtual double ComputeModel(const vector<double>& data, const set<int>& samples)
    {
        double mean = 0;
        for (auto itr = samples.begin(); itr != samples.end(); itr++) mean += data[*itr];
        return mean / samples.size();
    }

    // Calculate error between the mean and given datum
    virtual double ComputeError(const double& mean, const double& datum) { return datum - mean; }
};

// The main function
int main(void)
{
    // The given data with outliers
    vector<double> data = { 2.5, 2.6, 2.7, 2.8, 2.9, 3, 3.1, 3.2, 100, 3.3, 3.4, 3.5 };

    // Find the best model using RANSAC
    MeanEstimator estimator;
    RTL::RANSAC<double, double, vector<double> > ransac(&estimator);
    double model;
    double loss = ransac.FindBest(model, data, data.size(), 1);

    // Determine inliers using the best model if necessary
    vector<int> inliers = ransac.FindInliers(model, data, data.size());

    // Print the result
    cout << "- Found Model: " << model << " (Loss: " << loss << ")" << endl;
    cout << "- The Number of Inliers: " << inliers.size() << " (N: " << data.size() << ")" << endl;

    return 0;
}
