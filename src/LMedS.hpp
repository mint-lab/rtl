#ifndef __RTL_LMEDS__
#define __RTL_LMEDS__

#include "RANSAC.hpp"

namespace RTL
{

template <class ModelT, class ModelSetT, class DatumT, class DataT>
class LMedS : public RANSAC<ModelT, ModelSetT, DatumT, DataT>
{
public:
    LMedS(Estimator<Model, ModelSet, Datum, Data>* estimator) : RANSAC<Model, ModelSet, Datum, Data>(estimator) { }

protected:
    virtual inline double EvaluateModel(const Model& model, const Data& data, int N)
    {
        std::vector<double> errors(N);
        for (int i = 0; i < N; i++)
            errors[i] = toolEstimator->ComputeError(model, data[i]);
        std::nth_element(errors.begin(), errors.begin() + N / 2, errors.end());
        return errors[N / 2];
    }
};

} // End of 'RTL'

#endif // End of '__RTL_LMEDS__'
