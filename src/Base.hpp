#ifndef __RTL_BASE__
#define __RTL_BASE__

#include <set>
#include <vector>

namespace RTL
{

template <class ModelT, class ModelSetT, class DatumT, class DataT>
class Estimator
{
public:
    Estimator(size_t M) : SAMPLE_SIZE(M) { }

    typedef ModelT                      Model;

    typedef ModelSetT                   ModelSet;

    typedef DatumT                      Datum;

    typedef DataT                       Data;

    virtual int ComputeModel(ModelSet& models, const Data& data, const std::set<int>& samples) = 0;

    virtual double ComputeError(const Model& model, const Datum& datum) = 0;

    const size_t SAMPLE_SIZE;
};

template <class ModelT, class DatumT, class DataT>
class Observer
{
public:
    typedef ModelT                      Model;

    typedef DatumT                      Datum;

    typedef DataT                       Data;

    virtual bool GenerateData(Data& data, std::vector<int>& inliers, const Model& model, int N, double noise = 0, double ratio = 1) = 0;
};

} // End of 'RTL'

#endif // End of '__RTL_BASE__'
