#ifndef __PREPROCESSING_H__
#define __PREPROCESSING_H__

#include <vector>
#include <numeric>
#include <cmath>
#include <iterator>

#include "data_types.h"

void CalcMeanStdev( const std::vector<double> &data, double& mean, double& stdev )
{
    const size_t n_data = data.size();
    mean = 0.;
    stdev = 0;

    mean = std::accumulate( data.begin(), data.end(), 0. ) / double(n_data);

    stdev = 0.;
    std::for_each( data.begin(), data.end(), [&](const double x) {
        stdev += ( x - mean ) * ( x - mean );
    });
    stdev = sqrt( stdev / double(n_data-1) );
}

void Standardize( std::vector<double> &data ) {
    const size_t n_data = data.size();
    double mean = 0.;
    double stdev = 0;
    CalcMeanStdev( data, mean, stdev );

    for( int i = 0 ; i < n_data ; i++ ) {
        data[i] -= mean;
        data[i] /= stdev;
    }
}

void Standardize( IrisDataCollection& data )
{
    Standardize( data.sepal_length );
    Standardize( data.sepal_width );
    Standardize( data.petal_length );
    Standardize( data.petal_width );   
}

#endif /* __PREPROCESSING_H__ */