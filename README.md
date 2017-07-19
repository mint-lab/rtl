## RTL: RANSAC Template Library
_RANSAC Template Library (RTL)_ is an open-source robust regression tool especially with RANSAC family.
RTL aims to provide fast, accurate, and easy ways to estimate any model parameters with data contaminated with outliers (incorrect data).
RTL includes recent RANSAC variants with their performance evaluation with several models with synthetic and real data.
RTL is written in generic programming style (template in C++) for its further applications with user-defined models.
RTL is distributed under [Simplified BSD License](http://opensource.org/licenses/BSD-2-Clause).

### What is RANSAC?
_RANdom SAmple Consensus (RANSAC)_ is an iterative method to make any parameter estimator strong against outliers.
For example of line fitting, RANSAC enable to estimate a line parameter even though data points include wrong point observations far from the true line.
RANSAC is composed of two steps, hypothesis generation and hypothesis evaluation.
In the first step, RANSAC estimates a line (hypothesis) from randomly sampled point data.
In the second step, RANSAC counts the number of points which support the estimated line within the given threshold.
After several iterations of two steps, the final line can be obtained as a line who got the most supporters and inliers can determined as its supporters.
RANSAC was original proposed by Fischler and Bolles in 1981, but still utilized popularly to deal with outliers.

### Example
If a model estimator is defined in advance (e.g. LineEstimator), you can simply use RTL as follows.
```cpp
// Find the best model using RANSAC
LineEstimator estimator;
RTL::RANSAC<Line, Point, std::vector<Point> > ransac(&estimator);
Line model;
double loss = ransac.FindBest(model, data, data.size(), 2);

// Determine inliers using the best model if necessary
std::vector<int> inliers = ransac.FindInliers(model, data, data.size());
```

If you don't have a model estimator, you need to define it at first.
Please refer a simple example, [ExampleMean.cpp](https://github.com/sunglok/rtl/blob/master/examples/ExampleMean.cpp), which calculate the mean of data when they include outliers.

### Features
* __Robust Regression Algorithms__: RANSAC, LMedS, MSAC, MLESAC
* __Example Model Estimators__: LineEstimator
  * __Synthetic Data Generation__: LineObserver
* __Evaluation Tools__: Evaluator

### Authors
* [Sunglok Choi](http://sites.google.com/site/sunglok/) (sunglok AT hanmail DOT net)
* [Deok-Hwa Kim](http://rit.kaist.ac.kr/home/dhkim) (dhkim AT rit DOT kaist DOT ac DOT kr)
* Taemin Kim (luminans AT gmail DOT com)

### Reference
* Sunglok Choi, Taemin Kim, and Wonpil Yu, __Performance Evaluation of RANSAC Family__, in Proceedings of British Machine Vision Conference (BMVC), 2009 [PDF](https://sites.google.com/site/sunglok/files/Choi09_bmvc.pdf?attredirects=0)

### Acknowledgement
The authors thank the following contributors and projects.

* Junho Lee: He contributed LMedS and reported a bug.
