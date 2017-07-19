#ifndef __RTL_LINE__
#define __RTL_LINE__

#include "Base.hpp"
#include <cmath>
#include <limits>
#include <random>

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

class LineEstimator : virtual public RTL::Estimator<Line, Point, std::vector<Point> >
{
public:
    virtual Line ComputeModel(const std::vector<Point>& data, const std::set<int>& samples)
    {
        double meanX = 0, meanY = 0, meanXX = 0, meanYY = 0, meanXY = 0;
        for (auto itr = samples.begin(); itr != samples.end(); itr++)
        {
            const Point& p = data[*itr];
            meanX += p.x;
            meanY += p.y;
            meanXX += p.x * p.x;
            meanYY += p.y * p.y;
            meanXY += p.x * p.y;
        }
        size_t M = samples.size();
        meanX /= M;
        meanY /= M;
        meanXX /= M;
        meanYY /= M;
        meanXY /= M;
        double a = meanXX - meanX * meanX;
        double b = meanXY - meanX * meanY;
        double d = meanYY - meanY * meanY;

        Line line;
        if (fabs(b) > DBL_EPSILON)
        {
            // Calculate the first eigen vector of A = [a, b; b, d]
            // Ref. http://www.math.harvard.edu/archive/21b_fall_04/exhibits/2dmatrices/index.html
            double T2 = (a + d) / 2;
            double lambda = T2 - sqrt(T2 * T2 - (a * d - b * b));
            double v1 = lambda - d, v2 = b;
            double norm = sqrt(v1 * v1 + v2 * v2);
            line.a = v1 / norm;
            line.b = v2 / norm;
        }
        else
        {
            line.a = 1;
            line.b = 0;
        }
        line.c = -line.a * meanX - line.b * meanY;
        return line;
    }

    virtual double ComputeError(const Line& line, const Point& point)
    {
        return line.a * point.x + line.b * point.y + line.c;
    }
}; // End of 'LineEstimator'

class LineObserver : virtual public RTL::Observer<Line, Point, std::vector<Point> >
{
public:
    LineObserver(Point _max = Point(640, 480), Point _min = Point(0, 0)) : RANGE_MAX(_max), RANGE_MIN(_min) { }

    virtual std::vector<Point> GenerateData(const Line& line, int N, std::vector<int>& inliers, double noise = 0, double ratio = 1)
    {
        std::mt19937 generator;
        std::uniform_real_distribution<double> uniform(0, 1);
        std::normal_distribution<double> normal(0, 1);

        std::vector<Point> data;
        if (fabs(line.b) > fabs(line.a))
        {
            for (int i = 0; i < N; i++)
            {
                Point point;
                point.x = (RANGE_MAX.x - RANGE_MIN.x) * uniform(generator) + RANGE_MIN.x;
                double vote = uniform(generator);
                if (vote > ratio)
                {
                    // Generate an outlier
                    point.y = (RANGE_MAX.y - RANGE_MIN.y) * uniform(generator) + RANGE_MIN.y;
                }
                else
                {
                    // Generate an inlier
                    point.y = (line.a * point.x + line.c) / -line.b;
                    point.x += noise * normal(generator);
                    point.y += noise * normal(generator);
                    inliers.push_back(i);
                }
                data.push_back(point);
            }
        }
        else
        {
            for (int i = 0; i < N; i++)
            {
                Point point;
                point.y = (RANGE_MAX.y - RANGE_MIN.y) * uniform(generator) + RANGE_MIN.y;
                double vote = uniform(generator);
                if (vote > ratio)
                {
                    // Generate an outlier
                    point.x = (RANGE_MAX.x - RANGE_MIN.x) * uniform(generator) + RANGE_MIN.x;
                }
                else
                {
                    // Generate an inlier
                    point.x = (line.b * point.y + line.c) / -line.a;
                    point.x += noise * normal(generator);
                    point.y += noise * normal(generator);
                    inliers.push_back(i);
                }
                data.push_back(point);
            }
        }
        return data;
    }

    const Point RANGE_MIN;

    const Point RANGE_MAX;
};

#endif // End of '__RTL_LINE__'
