#ifndef __RTL_MLESAC__
#define __RTL_MLESAC__

#include "MSAC.hpp"

#ifndef M_PI
#   define M_PI                         3.14159265358979323846
#endif

namespace RTL
{

template <class ModelT, class ModelSetT, class DatumT, class DataT>
class MLESAC : public MSAC<ModelT, ModelSetT, DatumT, DataT>
{
public:
    MLESAC(Estimator<Model, ModelSet, Datum, Data>* estimator) : MSAC<Model, ModelSet, Datum, Data>(estimator)
    {
        dataError2 = NULL;
        paramIterationEM = 5;
        paramSigmaScale = 1.96;
    }

protected:
    virtual inline void Initialize(const Data& data, int N)
    {
        MSAC::Initialize(data, N);
        dataError2 = new double[N];
        assert(dataError2 != NULL);
        double sigma = paramThreshold / paramSigmaScale;
        dataSigma2 = sigma * sigma;
    }

    virtual inline double EvaluateModel(const Model& model, const Data& data, int N)
    {
        // Calculate squared error
        double minError = HUGE_VAL, maxError = -HUGE_VAL;
        for (int i = 0; i < N; i++)
        {
            double error = toolEstimator->ComputeError(model, data[i]);
            if (error < minError) minError = error;
            if (error > maxError) maxError = error;
            dataError2[i] = error * error;
        }

        // Estimate the inlier ratio using EM
        const double nu = maxError - minError;
        double gamma = 0.5;
        for (int iter = 0; iter < paramIterationEM; iter++)
        {
            double sumPosteriorProb = 0;
            const double probOutlier = (1 - gamma) / nu;
            const double probInlierCoeff = gamma / sqrt(2 * M_PI * dataSigma2);
            for (int i = 0; i < N; i++)
            {
                double probInlier = probInlierCoeff * exp(-0.5 * dataError2[i] / dataSigma2);
                sumPosteriorProb += probInlier / (probInlier + probOutlier);
            }
            gamma = sumPosteriorProb / N;
        }

        // Evaluate the model
        double sumLogLikelihood = 0;
        const double probOutlier = (1 - gamma) / nu;
        const double probInlierCoeff = gamma / sqrt(2 * M_PI * dataSigma2);
        for (int i = 0; i < N; i++)
        {
            double probInlier = probInlierCoeff * exp(-0.5 * dataError2[i] / dataSigma2);
            sumLogLikelihood = sumLogLikelihood - log(probInlier + probOutlier);
        }
        return sumLogLikelihood;
    }

    virtual inline void Terminate(const Data& data, int N, const Model& bestModel)
    {
        if (dataError2 != NULL)
        {
            delete [] dataError2;
            dataError2 = NULL;
        }
        MSAC::Terminate(data, N, bestModel);
    }

    int paramIterationEM;

    double paramSigmaScale;

    double* dataError2;

    double dataSigma2;
};

} // End of 'RTL'

#endif // End of '__RTL_MLESAC__'
