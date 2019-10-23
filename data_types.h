#ifndef __DATA_TYPES_H__
#define __DATA_TYPES_H__

#include <vector>
#include <string>

enum IRIS_VARIETY {
    SETOSA = 0,
    VERSICOLOR = 1,
    VIRGINICA = 2,
    kNumVarieties = 3
};

 
struct IrisDataCollection {
    std::vector<double> sepal_length;
    std::vector<double> sepal_width;
    std::vector<double> petal_length;
    std::vector<double> petal_width;
    //std::string variety;
    std::vector<IRIS_VARIETY> variety;
};

#endif /* __DATA_TYPES_H__ */
