#include "RTL.hpp"
#include <iostream>
#include <fstream>

using namespace std;

class ExpVar
{
public:
    ExpVar() : dataNum(0), noiseLevel(0), inlierRate(0) { }
    ExpVar(int num, double level, double rate) : dataNum(num), noiseLevel(level), inlierRate(rate) { }

    int dataNum;
    double noiseLevel;
    double inlierRate;
};

typedef RTL::RANSAC<Line, Point, vector<Point> >* AlgoPtr;

bool RunRandomExp(const char* output, AlgoPtr algoPtr[], int algoNum, LineEstimator& estimator, Line truth, const ExpVar& expMax, const ExpVar& expMin, const ExpVar& expStep, int expTrial, bool verbose = true)
{
    if (output == NULL || algoPtr == NULL) return false;
    ofstream record;
    record.open(output);

    ExpVar var;
    for (var.dataNum = expMin.dataNum; var.dataNum <= expMax.dataNum; var.dataNum += expStep.dataNum)
    {
        for (var.noiseLevel = expMin.noiseLevel; var.noiseLevel <= expMax.noiseLevel; var.noiseLevel += expStep.noiseLevel)
        {
            for (var.inlierRate = expMin.inlierRate; var.inlierRate <= expMax.inlierRate; var.inlierRate += expStep.inlierRate)
            {
                for (int trial = 0; trial < expTrial; trial++)
                {
                    // Generate noisy data from the truth
                    vector<int> trueInliers;
                    LineObserver observer;
                    vector<Point> data = observer.GenerateData(truth, var.dataNum, trueInliers, var.noiseLevel, var.inlierRate);
                    if (data.empty()) goto RUN_RANDOM_EXP_FAIL;
                    Evaluator<Line, Point, vector<Point> > evaluator(&estimator);
                    if (!evaluator.SetGroundTruth(truth, data, var.dataNum, trueInliers)) goto RUN_RANDOM_EXP_FAIL;

                    for (int algoIndex = 0; algoIndex < algoNum; algoIndex++)
                    {
                        // Find the best model and its supporting inliers
                        Line model;
                        StopWatch watch;
                        watch.Start();
                        double loss = algoPtr[algoIndex]->FindBest(model, data, data.size(), 2);
                        double ctime = watch.GetElapse();
                        vector<int> inliers = algoPtr[algoIndex]->FindInliers(model, data, data.size());

                        // Evaluate the results
                        double nsse = evaluator.EvaluateModel(model);
                        Score score = evaluator.EvaluateInliers(inliers);

                        // Write and print the results
                        // - Format: dataNum, noiseLevel, inlierRate, expTrial, algoIndex, compTime, NSSE, TP, FP, FN
                        record << var.dataNum << ", " << var.noiseLevel << ", " << var.inlierRate << ", " << trial << ", " << algoIndex << ", " << ctime << ", " << nsse << ", " << score.tp << ", " << score.fp << ", " << score.fn << endl;
                    }
                } // End of 'for (expTrial)'
                if (verbose)
                    cout << var.dataNum << ", " << var.noiseLevel << ", " << var.inlierRate << endl;
            } // End of 'for (expVar.inlierRate)'
        } // End of 'for (expVar.noiseLevel)'
    } // End of 'for (expVar.dataNum)'
    record.close();
    return true;

RUN_RANDOM_EXP_FAIL:
    record.close();
    return false;
}

int main(void)
{
    // Configure experiments
    const Line   CONFIG_MODEL_TRUTH (0.6, 0.8, -300);
    const ExpVar CONFIG_EXP_DEFAULT ( 200, 0.6, 0.5);
    const ExpVar CONFIG_EXP_MIN     ( 100, 0.2, 0.1);
    const ExpVar CONFIG_EXP_MAX     (1000, 2.0, 0.9);
    const ExpVar CONFIG_EXP_STEP    ( 100, 0.2, 0.1);
    const int    CONFIG_EXP_TRIAL = 1000;
    const char*  CONFIG_EXP_NAME1 = "LineRandom(DataNum).csv";
    const char*  CONFIG_EXP_NAME2 = "LineRandom(NoiseLevel).csv";
    const char*  CONFIG_EXP_NAME3 = "LineRandom(InlierRate).csv";

    // Prepare algorithms
    LineEstimator estimator;
    RTL::RANSAC<Line, Point, vector<Point> > ransac(&estimator);
    RTL::LMedS<Line, Point, vector<Point> > lmeds(&estimator);
    RTL::MSAC<Line, Point, vector<Point> > msac(&estimator);
    RTL::MLESAC<Line, Point, vector<Point> > mlesac(&estimator);
    AlgoPtr algorithms[] =
    {
        &ransac,
        &lmeds,
        &msac,
        &mlesac,
    };
    const int ALGO_NUM = sizeof(algorithms) / sizeof(AlgoPtr);

    // Perform an experiment with varying 'dataNum'
    ExpVar expMin, expMax, expStep(CONFIG_EXP_STEP);
    expMin = CONFIG_EXP_DEFAULT;
    expMin.dataNum = CONFIG_EXP_MIN.dataNum;
    expMax = CONFIG_EXP_DEFAULT;
    expMax.dataNum = CONFIG_EXP_MAX.dataNum;
    RunRandomExp(CONFIG_EXP_NAME1, algorithms, ALGO_NUM, estimator, CONFIG_MODEL_TRUTH, expMax, expMin, CONFIG_EXP_STEP, CONFIG_EXP_TRIAL);

    // Perform an experiment with varying 'noiseLevel'
    expMin = CONFIG_EXP_DEFAULT;
    expMin.noiseLevel = CONFIG_EXP_MIN.noiseLevel;
    expMax = CONFIG_EXP_DEFAULT;
    expMax.noiseLevel = CONFIG_EXP_MAX.noiseLevel;
    RunRandomExp(CONFIG_EXP_NAME2, algorithms, ALGO_NUM, estimator, CONFIG_MODEL_TRUTH, expMax, expMin, CONFIG_EXP_STEP, CONFIG_EXP_TRIAL);

    // Perform an experiment with varying 'inlierRate'
    expMin = CONFIG_EXP_DEFAULT;
    expMin.inlierRate = CONFIG_EXP_MIN.inlierRate;
    expMax = CONFIG_EXP_DEFAULT;
    expMax.inlierRate = CONFIG_EXP_MAX.inlierRate;
    RunRandomExp(CONFIG_EXP_NAME3, algorithms, ALGO_NUM, estimator, CONFIG_MODEL_TRUTH, expMax, expMin, CONFIG_EXP_STEP, CONFIG_EXP_TRIAL);

    return 0;
}