#ifndef __RTL_EVALUATOR__
#define __RTL_EVALUATOR__

#include "Base.hpp"
#include <algorithm>
#ifdef _WIN32
#   include <windows.h>
#endif

class Score
{
public:
    Score() : tp(0), fp(0), tn(0), fn(0) { }

    int tp;

    int fp;

    int tn;

    int fn;
};

template <class ModelT, class ModelSetT, class DatumT, class DataT>
class Evaluator
{
public:
    typedef ModelT                      Model;

    typedef ModelSetT                   ModelSet;

    typedef DatumT                      Datum;

    typedef DataT                       Data;

    Evaluator(RTL::Estimator<Model, ModelSet, Datum, Data>* estimator)
    {
        assert(estimator != NULL);
        toolEstimator = estimator;
        trueSSE = -1;
    }

    bool SetGroundTruth(const Data& data, const std::vector<int> inliers, const Model& model, int N)
    {
        trueModel = model;
        trueInliers = inliers;
        noisyData.clear();
        for (int i = 0; i < N; i++)
            noisyData.push_back(data[i]);

        trueSSE = 0;
        for (size_t i = 0; i < trueInliers.size(); i++)
        {
            double error = toolEstimator->ComputeError(trueModel, noisyData[trueInliers[i]]);
            trueSSE += error * error;
        }
        return true;
    }

    double EvaluateModel(const Model& model)
    {
        if (trueSSE <= 0) return -1; // Check initialization

        double SSE = 0;
        for (size_t i = 0; i < trueInliers.size(); i++)
        {
            double error = toolEstimator->ComputeError(model, noisyData[trueInliers[i]]);
            SSE += error * error;
        }
        return (SSE / trueSSE);
    }

    Score EvaluateInliers(const std::vector<int>& inliers)
    {
        Score score;
        if (trueInliers.empty()) return score; // Check initialization

        for (size_t i = 0; i < inliers.size(); i++)
        {
            bool found = (std::find(trueInliers.begin(), trueInliers.end(), inliers[i]) != trueInliers.end());
            if (found) score.tp++;
            else       score.fp++;
        }
        int t = static_cast<int>(trueInliers.size());
        int f = static_cast<int>(noisyData.size() - trueInliers.size());
        score.fn = t - score.tp;
        score.tn = f - score.fp;
        return score;
    }

protected:
    RTL::Estimator<Model, ModelSet, Datum, Data>* toolEstimator;

    Model trueModel;

    std::vector<int> trueInliers;

    double trueSSE;

    std::vector<Datum> noisyData;
}; // End of 'Evaluator'

class StopWatch
{
public:
    StopWatch()
    {
        ::QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
        ::QueryPerformanceCounter((LARGE_INTEGER*)&start);
    }

    bool Start(void)
    {
        ::QueryPerformanceCounter((LARGE_INTEGER*)&start);
        return true;
    }

    double GetElapse(void)
    {
        assert(freq.QuadPart != 0);
        LARGE_INTEGER finish;
        ::QueryPerformanceCounter((LARGE_INTEGER*)&finish);
        return (static_cast<double>(finish.QuadPart - start.QuadPart) / freq.QuadPart);
    }

private:
    LARGE_INTEGER freq;

    LARGE_INTEGER start;
};

#endif // End of '__RTL_EVALUATOR__'
