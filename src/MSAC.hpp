#ifndef __RTL_MSAC__
#define __RTL_MSAC__

#include "RANSAC.hpp"

namespace RTL
{

template <class Model, class ModelSet, class Datum, class Data>
class MSAC : public virtual RANSAC<Model, ModelSet, Datum, Data>
{
public:

    MSAC(Estimator<Model, ModelSet, Datum, Data>* estimator): RANSAC<Model, ModelSet, Datum, Data>(estimator) { }

protected:
    virtual inline double EvaluateModel(const Model& model, const Data& data, int N)
    {
        double loss = 0;
        for (int i = 0; i < N; i++)
        {
            double error = this->toolEstimator->ComputeError(model, data[i]);
            if (error > this->paramThreshold || error < -this->paramThreshold) loss += this->paramThreshold * this->paramThreshold;
            else loss += error * error;
        }
        return loss;
    }
};

} // End of 'RTL'

#endif // End of '__RTL_MSAC__'
