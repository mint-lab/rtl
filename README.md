## RTL: RANSAC Template Library
_RANSAC Template Library_ is an open-source robust regression library especially with RANSAC family.
It aims to provide fast, accurate, and easy ways to estimate any unknown model with data contaminated with outliers.
It is written in generic programming style with C++ and distributed under [Simplified BSD License](http://opensource.org/licenses/BSD-2-Clause).

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
Please refer an example, [ExampleMean.cpp](https://github.com/sunglok/rtl/blob/master/examples/ExampleMean.cpp), which calculate the mean of data when they include outliers.

### Features
* __Robust Regression Algorithms__: RANSAC, LMedS, MSAC, MLESAC
* __Example Model Estimators__: LineEstimator
  * __Synthetic Data Generation__: LineObserver
* __Evaluation Tools__: Evaluator

### Authors
* [Sunglok Choi](http://sites.google.com/site/sunglok/) (sunglok AT hanmail DOT net)
* Taemin Kim

### Reference
* Sunglok Choi, Taemin Kim, and Wonpil Yu, __Performance Evaluation of RANSAC Family__, in Proceedings of British Machine Vision Conference (BMVC), 2009 [PDF](https://sites.google.com/site/sunglok/files/Choi09_bmvc.pdf?attredirects=0)

### Acknowledgement
The authors thank the following contributors and projects.

* Junho Lee: He contributed LMedS and reported a bug.
