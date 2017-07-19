#ifndef __RTL_MSAC__
#define __RTL_MSAC__

#include "RANSAC.hpp"

namespace RTL
{

template <class Model, class Datum, class Data>
class MSAC : virtual public RANSAC<Model, Datum, Data>
{
public:
    MSAC(Estimator<Model, Datum, Data>* estimator) : RANSAC<Model, Datum, Data>(estimator) { }

protected:
    virtual inline double EvaluateModel(const Model& model, const Data& data, int N)
    {
        double loss = 0;
        for (int i = 0; i < N; i++)
        {
            double error = toolEstimator->ComputeError(model, data[i]);
            if (error > paramThreshold || error < -paramThreshold) loss += paramThreshold * paramThreshold;
            else loss += error * error;
        }
        return loss;
    }
};

} // End of 'RTL'

#endif // End of '__RTL_MSAC__'
