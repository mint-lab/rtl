#include <iostream>
#include <fstream>
#include "RANSAC.hpp"
#include "MSAC.hpp"
#include "Line.hpp"
#include "Evaluator.hpp"

class ExpVar
{
public:
    ExpVar() : dataNum(0), noiseLevel(0), inlierRate(0) { }
    ExpVar(int num, double level, double rate) : dataNum(num), noiseLevel(level), inlierRate(rate) { }

    int dataNum;
    double noiseLevel;
    double inlierRate;
};

const Line   CONFIG_MODEL_TRUTH (0.6, -0.8, 1);
const ExpVar CONFIG_EXP_DEFAULT ( 200, 0.6, 0.5);
const ExpVar CONFIG_EXP_MIN     ( 100, 0.2, 0.1);
const ExpVar CONFIG_EXP_MAX     (1000, 2.0, 0.9);
const ExpVar CONFIG_EXP_STEP    ( 100, 0.2, 0.1);
const int    CONFIG_EXP_TRIAL = 1;
const char*  CONFIG_EXP_NAME1 = "LineRandom(DataNum).csv";
const char*  CONFIG_EXP_NAME2 = "LineRandom(NoiseLevel).csv";
const char*  CONFIG_EXP_NAME3 = "LineRandom(InlierRate).csv";

typedef RTL::RANSAC<Line, std::vector<Line>, Point, std::vector<Point> >* AlgoPtr;

bool RunRandomExp(const char* output, AlgoPtr algoPtr[], int algoNum, LineEstimator& estimator, const ExpVar& expMax, const ExpVar& expMin, const ExpVar& expStep, int expTrial, bool verbose = true)
{
    if (output == NULL || algoPtr == NULL) return false;
    std::ofstream record;
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
                    LineObserver observer;
                    std::vector<Point> data;
                    std::vector<int> trueInliers;
                    if (!observer.GenerateData(data, trueInliers, CONFIG_MODEL_TRUTH, var.dataNum, var.noiseLevel, var.inlierRate)) goto RUN_RANDOM_EXP_FAIL;
                    Evaluator<Line, std::vector<Line>, Point, std::vector<Point> > evaluator(&estimator);
                    if (!evaluator.SetGroundTruth(data, trueInliers, CONFIG_MODEL_TRUTH, var.dataNum)) goto RUN_RANDOM_EXP_FAIL;

                    for (int algoIndex = 0; algoIndex < algoNum; algoIndex++)
                    {
                        // Find the best model and its supporting inliers
                        Line bestModel;
                        StopWatch watch;
                        watch.Start();
                        double bestLoss = algoPtr[algoIndex]->FindBest(bestModel, data, data.size());
                        double compTime = watch.GetElapse();
                        std::vector<int> inliers;
                        algoPtr[algoIndex]->FindInliers(inliers, bestModel, data, data.size());

                        // Evaluate the results
                        double nsse = evaluator.EvaluateModel(bestModel);
                        Score score = evaluator.EvaluateInliers(inliers);

                        // Write and print the results
                        // - Format: dataNum, noiseLevel, inlierRate, expTrial, algoIndex, compTime, NSSE, TP, FP, FN
                        record << var.dataNum << ", " << var.noiseLevel << ", " << var.inlierRate << ", " << trial << ", " << algoIndex << ", " << compTime << ", " << nsse << ", " << score.tp << ", " << score.fp << ", " << score.fn << std::endl;
                        if (verbose)
                            std::cout << var.dataNum << ", " << var.noiseLevel << ", " << var.inlierRate << ", " << trial << ", " << algoIndex << std::endl;
                    }
                } // End of 'for (expTrial)'
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
    // Prepare algorithms
    LineEstimator estimator;
    RTL::RANSAC<Line, std::vector<Line>, Point, std::vector<Point> > ransac(&estimator);
    RTL::MSAC<Line, std::vector<Line>, Point, std::vector<Point> > msac(&estimator);
    AlgoPtr algorithms[] =
    {
        &ransac,
        &msac,
    };
    const int ALGO_NUM = sizeof(algorithms) / sizeof(AlgoPtr);

    // Perform an experiment with varying 'dataNum'
    ExpVar expMin, expMax, expStep(CONFIG_EXP_STEP);
    expMin = CONFIG_EXP_DEFAULT;
    expMin.dataNum = CONFIG_EXP_MIN.dataNum;
    expMax = CONFIG_EXP_DEFAULT;
    expMax.dataNum = CONFIG_EXP_MAX.dataNum;
    RunRandomExp(CONFIG_EXP_NAME1, algorithms, ALGO_NUM, estimator, expMax, expMin, CONFIG_EXP_STEP, CONFIG_EXP_TRIAL);

    // Perform an experiment with varying 'noiseLevel'
    expMin = CONFIG_EXP_DEFAULT;
    expMin.noiseLevel = CONFIG_EXP_MIN.noiseLevel;
    expMax = CONFIG_EXP_DEFAULT;
    expMax.noiseLevel = CONFIG_EXP_MAX.noiseLevel;
    RunRandomExp(CONFIG_EXP_NAME2, algorithms, ALGO_NUM, estimator, expMax, expMin, CONFIG_EXP_STEP, CONFIG_EXP_TRIAL);

    // Perform an experiment with varying 'inlierRate'
    expMin = CONFIG_EXP_DEFAULT;
    expMin.inlierRate = CONFIG_EXP_MIN.inlierRate;
    expMax = CONFIG_EXP_DEFAULT;
    expMax.inlierRate = CONFIG_EXP_MAX.inlierRate;
    RunRandomExp(CONFIG_EXP_NAME3, algorithms, ALGO_NUM, estimator, expMax, expMin, CONFIG_EXP_STEP, CONFIG_EXP_TRIAL);

    return 0;
}