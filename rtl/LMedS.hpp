#ifndef __RTL_LMEDS__
#define __RTL_LMEDS__

#include "RANSAC.hpp"
#include <algorithm>

namespace RTL
{

template <class Model, class Datum, class Data>
class LMedS : virtual public RANSAC<Model, Datum, Data>
{
public:
    LMedS(Estimator<Model, Datum, Data>* estimator) : RANSAC<Model, Datum, Data>(estimator) { }

protected:
    virtual double EvaluateModel(const Model& model, const Data& data, int N)
    {
        std::vector<double> errors(N);
        for (int i = 0; i < N; i++)
            errors[i] = fabs(toolEstimator->ComputeError(model, data[i]));
        std::nth_element(errors.begin(), errors.begin() + N / 2, errors.end());
        return errors[N / 2];
    }
};

} // End of 'RTL'

#endif // End of '__RTL_LMEDS__'
