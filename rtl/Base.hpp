#ifndef __RTL_BASE__
#define __RTL_BASE__

#include <set>
#include <vector>

namespace RTL
{

template <class ModelT, class DatumT, class DataT>
class Estimator
{
public:
    typedef ModelT                      Model;

    typedef DatumT                      Datum;

    typedef DataT                       Data;

    virtual Model ComputeModel(const Data& data, const std::set<int>& samples) = 0;

    virtual double ComputeError(const Model& model, const Datum& datum) = 0;
};

template <class ModelT, class DatumT, class DataT>
class Observer
{
public:
    typedef ModelT                      Model;

    typedef DatumT                      Datum;

    typedef DataT                       Data;

    virtual Data GenerateData(const Model& model, int N, std::vector<int>& inliers, double noise = 0, double ratio = 1) = 0;
};

} // End of 'RTL'

#endif // End of '__RTL_BASE__'
