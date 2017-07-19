#ifndef __RTL_RANSAC__
#define __RTL_RANSAC__

#include "Base.hpp"
#include <random>
#include <cmath>
#include <cassert>

namespace RTL
{

template <class Model, class Datum, class Data>
class RANSAC
{
public:
    RANSAC(Estimator<Model, Datum, Data>* estimator)
    {
        assert(estimator != NULL);

        toolEstimator = estimator;
        SetParamIteration();
        SetParamThreshold();
    }

    virtual double FindBest(Model& best, const Data& data, int N, int M)
    {
        assert(N > 0 && M > 0);

        Initialize(data, N);

        // Run RANSAC
        double bestloss = HUGE_VAL;
        int iteration = 0;
        while (IsContinued(iteration))
        {
            iteration++;

            // 1. Generate hypotheses
            Model model = GenerateModel(data, M);

            // 2. Evaluate the hypotheses
            double loss = EvaluateModel(model, data, N);
            if (loss < bestloss)
                if (!UpdateBest(best, bestloss, model, loss))
                    goto RANSAC_FIND_BEST_EXIT;
        }

RANSAC_FIND_BEST_EXIT:
        Terminate(best, data, N);
        return bestloss;
    }

    virtual std::vector<int> FindInliers(const Model& model, const Data& data, int N)
    {
        std::vector<int> inliers;
        for (int i = 0; i < N; i++)
        {
            double error = toolEstimator->ComputeError(model, data[i]);
            if (fabs(error) < paramThreshold) inliers.push_back(i);
        }
        return inliers;
    }

    void SetParamIteration(int iteration = 100) { paramIteration = iteration; }

    int GetParamIteration(void) { return paramIteration; }

    void SetParamThreshold(double threshold = 1) { paramThreshold = threshold; }

    int GetParamThreshold(void) { return paramThreshold; }

protected:
    virtual bool IsContinued(int iteration) { return (iteration < paramIteration); }

    virtual Model GenerateModel(const Data& data, int M)
    {
        std::set<int> samples;
        while (static_cast<int>(samples.size()) < M)
            samples.insert(toolUniform(toolGenerator));
        return toolEstimator->ComputeModel(data, samples);
    }

    virtual double EvaluateModel(const Model& model, const Data& data, int N)
    {
        double loss = 0;
        for (int i = 0; i < N; i++)
        {
            double error = toolEstimator->ComputeError(model, data[i]);
            loss += (fabs(error) > paramThreshold);
        }
        return loss;
    }

    virtual bool UpdateBest(Model& bestModel, double& bestCost, const Model& model, double cost)
    {
        bestModel = model;
        bestCost = cost;
        return true;
    }

    virtual void Initialize(const Data& data, int N) { toolUniform = std::uniform_int_distribution<int>(0, N - 1); }

    virtual void Terminate(const Model& bestModel, const Data& data, int N) { }

    std::mt19937 toolGenerator;

    std::uniform_int_distribution<int> toolUniform;

    Estimator<Model, Datum, Data>* toolEstimator;

    int paramSampleSize;

    int paramIteration;

    double paramThreshold;
}; // End of 'RANSAC'

} // End of 'RTL'

#endif // End of '__RTL_RANSAC__'
