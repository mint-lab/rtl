#ifndef __RTL_RANSAC__
#define __RTL_RANSAC__

#include "Base.hpp"
#include <random>
#include <cmath>
#include <cassert>

namespace RTL
{

template <class ModelT, class ModelSetT, class DatumT, class DataT>
class RANSAC
{
public:
    typedef ModelT                      Model;

    typedef ModelSetT                   ModelSet;

    typedef DatumT                      Datum;

    typedef DataT                       Data;

    RANSAC(Estimator<Model, ModelSet, Datum, Data>* estimator);

    bool SetParamIteration(int iteration = 100) { paramIteration = iteration; return true; }

    int GetParamIteration(void) { return paramIteration; }

    bool SetParamThreshold(double threshold = 1) { paramThreshold = threshold; return true; }

    int GetParamThreshold(void) { return paramThreshold; }

    virtual double FindBest(Model& best, const Data& data, int N);

    virtual int FindInliers(std::vector<int>& inliers, const Model& model, const Data& data, int N);

    virtual int FindInliers(std::vector<bool>& inliers, const Model& model, const Data& data, int N);

protected:
    virtual inline bool IsContinued(int iteration);

    virtual inline int GenerateModel(ModelSet& models, const Data& data);

    virtual inline double EvaluateModel(const Model& model, const Data& data, int N);

    virtual inline bool UpdateBest(Model& bestModel, double& bestCost, const Model& model, double cost);

    std::tr1::mt19937 toolGenerator;

    std::tr1::uniform_int<int> toolUniform;

    Estimator<Model, ModelSet, Datum, Data>* toolEstimator;

    int paramIteration;

    double paramThreshold;
};

template <class Model, class ModelSet, class Datum, class Data>
RANSAC<Model, ModelSet, Datum, Data>::RANSAC(Estimator<Model, ModelSet, Datum, Data>* estimator)
{
    assert(estimator != NULL);

    toolEstimator = estimator;
    SetParamIteration();
    SetParamThreshold();
}

template <class Model, class ModelSet, class Datum, class Data>
int RANSAC<Model, ModelSet, Datum, Data>::FindInliers(std::vector<int>& inliers, const Model& model, const Data& data, int N)
{
    for (int i = 0; i < N; i++)
    {
        double error = toolEstimator->ComputeError(model, data[i]);
        if (fabs(error) < paramThreshold) inliers.push_back(i);
    }
    return static_cast<int>(inliers.size());
}

template <class Model, class ModelSet, class Datum, class Data>
int RANSAC<Model, ModelSet, Datum, Data>::FindInliers(std::vector<bool>& inliers, const Model& model, const Data& data, int N)
{
	int k = 0;
    for (int i = 0; i < N; i++)
    {
        double error = toolEstimator->ComputeError(model, data[i]);
        if (fabs(error) < paramThreshold) {
			inliers.push_back(true);
			++k;
		} else {
			inliers.push_back(false);
		}
    }
    return static_cast<int>(k);
}

template <class Model, class ModelSet, class Datum, class Data>
double RANSAC<Model, ModelSet, Datum, Data>::FindBest(Model& best, const Data& data, int N)
{
    toolUniform = std::tr1::uniform_int<int>(0, N - 1);

    double bestloss = HUGE_VAL;
    int iteration = 0;
    while (IsContinued(iteration))
    {
        iteration++;

        // 1. Generate hypotheses
        ModelSet models;
        int num = GenerateModel(models, data);

        // 2. Evaluate the hypotheses
        for (int i = 0; i < num; i++)
        {
            double modelloss = EvaluateModel(models[i], data, N);
            if (modelloss < bestloss)
                if (!UpdateBest(best, bestloss, models[i], modelloss))
                    goto RANSAC_FIND_BEST_EXIT;
        }
    }
RANSAC_FIND_BEST_EXIT:
    return bestloss;
}

template <class Model, class ModelSet, class Datum, class Data>
bool RANSAC<Model, ModelSet, Datum, Data>::IsContinued(int iteration)
{
    return (iteration < paramIteration);
}

template <class Model, class ModelSet, class Datum, class Data>
int RANSAC<Model, ModelSet, Datum, Data>::GenerateModel(ModelSet& models, const Data& data)
{
    std::set<int> samples;
    while (samples.size() < toolEstimator->SAMPLE_SIZE)
        samples.insert(toolUniform(toolGenerator));
    return toolEstimator->ComputeModel(models, data, samples);
}

template <class Model, class ModelSet, class Datum, class Data>
double RANSAC<Model, ModelSet, Datum, Data>::EvaluateModel(const Model& model, const Data& data, int N)
{
    double loss = 0;
    for (int i = 0; i < N; i++)
    {
        double error = toolEstimator->ComputeError(model, data[i]);
        loss += (fabs(error) > paramThreshold);
    }
    return loss;
}

template <class Model, class ModelSet, class Datum, class Data>
bool RANSAC<Model, ModelSet, Datum, Data>::UpdateBest(Model& bestModel, double& bestCost, const Model& model, double cost)
{
    bestModel = model;
    bestCost = cost;
    return true;
}

} // End of 'RTL'

#endif // End of '__RTL_RANSAC__'
