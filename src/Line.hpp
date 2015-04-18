#ifndef __RTL_LINE__
#define __RTL_LINE__

#include "Base.hpp"
#include <cmath>
#include <limits>
#include <random>
#include <cfloat>

using RTL::Estimator;

class Point
{
    public:
        Point() : x(0), y(0) { }

        Point(double _x, double _y) : x(_x), y(_y) { }

        friend std::ostream& operator<<(std::ostream& out, const Point& p) { return out << p.x << ", " << p.y; }

        double x, y;
};

class Line
{
    public:
        Line() : a(0), b(0), c(0) { }

        Line(double _a, double _b, double _c) : a(_a), b(_b), c(_c) { }

        friend std::ostream& operator<<(std::ostream& out, const Line& l) { return out << l.a << ", " << l.b << ", " << l.c; }

        double a, b, c;
};

class LineEstimator : virtual public RTL::Estimator<Line, std::vector<Line>, Point, std::vector<Point> >
{
    public:
        typedef Line Model;
        typedef std::vector<Line> ModelSet;
        typedef Point Datum;
        typedef std::vector<Point> Data;

    public:

        LineEstimator(size_t M = 2) : Estimator(M) { }

        virtual int ComputeModel(ModelSet& models, const Data& data, const std::set<int>& samples)
        {
            double meanX = 0, meanY = 0, meanXX = 0, meanYY = 0, meanXY = 0;
            for (std::set<int>::const_iterator it = samples.begin();it != samples.end(); it++)
            {
                const Datum& p = data[*it];
                meanX += p.x;
                meanY += p.y;
                meanXX += p.x * p.x;
                meanYY += p.y * p.y;
                meanXY += p.x * p.y;
            }
            int M = static_cast<int>(samples.size());
            meanX /= M;
            meanY /= M;
            meanXX /= M;
            meanYY /= M;
            meanXY /= M;
            double a = meanXX - meanX*meanX;
            double b = meanXY - meanX*meanY;
            double d = meanYY - meanY*meanY;

            Model model;
            if (fabs(b) > DBL_EPSILON)
            {
                // Calculate the first eigen vector of A = [a, b; b, d]
                // Ref. http://www.math.harvard.edu/archive/21b_fall_04/exhibits/2dmatrices/index.html
                double T2 = (a + d) / 2;
                double lambda = T2 - sqrt(T2 * T2 - (a * d - b * b));
                double v1 = lambda - d, v2 = b;
                double norm = sqrt(v1 * v1 + v2 * v2);
                model.a = v1 / norm;
                model.b = v2 / norm;
            }
            else
            {
                model.a = 1;
                model.b = 0;
            }
            model.c = -model.a * meanX - model.b * meanY;
            models.push_back(model);
            return 1;
        }

        virtual double ComputeError(const Model& model, const Datum& datum)
        {
            return fabs(model.a * datum.x + model.b * datum.y + model.c);
        }
}; // End of 'LineEstimator'

class LineObserver : public RTL::Observer<Line, Point, std::vector<Point> >
{

    public:
        typedef Line Model;
        typedef Point Datum;
        typedef std::vector<Point> Data;

    public:
        LineObserver() : RANGE_MIN(Point(0, 0)), RANGE_MAX(Point(640, 480))
    {
    }

        LineObserver(Point _min, Point _max) : RANGE_MIN(_min), RANGE_MAX(_max) { }

        virtual bool GenerateData(Data& data, std::vector<int>& inliers, const Model& model, int N, double noise = 0, double ratio = 1)
        {
            std::mt19937 generator;
            std::uniform_real_distribution<double> uniform(0, 1);
            std::normal_distribution<double> normal(0, 1);

            if (fabs(model.b) > DBL_EPSILON)
            {
                for (int i = 0; i < N; i++)
                {
                    Datum datum;
                    datum.x = (RANGE_MAX.x - RANGE_MIN.x) * uniform(generator) - RANGE_MIN.x;
                    double vote = uniform(generator);
                    if (vote > ratio)
                    {
                        // Generate an outlier
                        datum.y = (RANGE_MAX.y - RANGE_MIN.y) * uniform(generator) - RANGE_MIN.y;
                    }
                    else
                    {
                        // Generate an inlier
                        datum.y = (model.a * datum.x + model.c) / -model.b;
                        datum.x += noise * normal(generator);
                        datum.y += noise * normal(generator);
                        inliers.push_back(i);
                    }
                    data.push_back(datum);
                }
            }
            else
            {
                for (int i = 0; i < N; i++)
                {
                    Datum datum;
                    datum.y = (RANGE_MAX.y - RANGE_MIN.y) * uniform(generator) - RANGE_MIN.y;
                    double vote = uniform(generator);
                    if (vote > ratio)
                    {
                        // Generate an outlier
                        datum.x = (RANGE_MAX.x - RANGE_MIN.x) * uniform(generator) - RANGE_MIN.x;
                    }
                    else
                    {
                        // Generate an inlier
                        datum.x = (model.b * datum.y + model.c) / -model.a;
                        datum.x += noise * normal(generator);
                        datum.y += noise * normal(generator);
                        inliers.push_back(i);
                    }
                    data.push_back(datum);
                }
            }
            return true;
        }

        const Point RANGE_MIN;

        const Point RANGE_MAX;
};

#endif // End of '__RTL_LINE__'
