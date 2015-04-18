#ifndef __RTL_BASE__
#define __RTL_BASE__

#include <set>
#include <vector>

namespace RTL
{

template <class Model, class ModelSet, class Datum, class Data>
class Estimator
{
public:
    Estimator(size_t M) : SAMPLE_SIZE(M) { }

    virtual int ComputeModel(ModelSet& models, const Data& data, const std::set<int>& samples) = 0;

    virtual double ComputeError(const Model& model, const Datum& datum) = 0;

    const size_t SAMPLE_SIZE;
};

template <class Model, class Datum, class Data>
class Observer
{
public:

    virtual bool GenerateData(Data& data, std::vector<int>& inliers, const Model& model, int N, double noise = 0, double ratio = 1) = 0;
};

} // End of 'RTL'

#endif // End of '__RTL_BASE__'
