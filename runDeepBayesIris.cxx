// ***************************************************************
// This file was created using the bat-project script
// for project DeepBayesIris.
// bat-project is part of Bayesian Analysis Toolkit (BAT).
// BAT can be downloaded from http://mpp.mpg.de/bat
// ***************************************************************

#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <numeric>

#include <BAT/BCLog.h>
#include <BAT/BCAux.h>

#include "DeepBayesIris.h"

// taken from: https://github.com/ben-strasser/fast-cpp-csv-parser
#include "csv.h"

#define CSV_IO_NO_THREAD

void Standardize( IrisDataCollection_t& data )
{
    const size_t n_data = data.size();
    double mean = 0.;
    double stdev = 0;

    // sepal_length
    mean = 0.;
    stdev = 0;
    for( int i = 0; i < n_data ; i++ ) {
            mean += data[i].sepal_length;
    }
    mean /= float(n_data);

    for( int i = 0; i < n_data ; i++ ) {
        stdev += ( data[i].sepal_length - mean )*( data[i].sepal_length - mean );
    }
    stdev /= float(n_data);
    stdev = std::sqrt(stdev);

    for( int i = 0; i < n_data ; i++ ) {
        data[i].sepal_length = ( data[i].sepal_length - mean ) / stdev;
    }

    // sepal_width
    mean = 0.;
    stdev = 0;
    for( int i = 0; i < n_data ; i++ ) {
            mean += data[i].sepal_width;
    }
    mean /= float(n_data);

    for( int i = 0; i < n_data ; i++ ) {
        stdev += ( data[i].sepal_width - mean )*( data[i].sepal_width - mean );
    }
    stdev /= float(n_data);
    stdev = std::sqrt(stdev);

    for( int i = 0; i < n_data ; i++ ) {
        data[i].sepal_width = ( data[i].sepal_width - mean ) / stdev;
    }

    // petal_length
    mean = 0.;
    stdev = 0;
    for( int i = 0; i < n_data ; i++ ) {
            mean += data[i].petal_length;
    }
    mean /= float(n_data);

    for( int i = 0; i < n_data ; i++ ) {
        stdev += ( data[i].petal_length - mean )*( data[i].petal_length - mean );
    }
    stdev /= float(n_data);
    stdev = std::sqrt(stdev);

    for( int i = 0; i < n_data ; i++ ) {
        data[i].petal_length = ( data[i].petal_length - mean ) / stdev;
    }

    // petal_width
    mean = 0.;
    stdev = 0;
    for( int i = 0; i < n_data ; i++ ) {
            mean += data[i].petal_width;
    }
    mean /= float(n_data);

    for( int i = 0; i < n_data ; i++ ) {
        stdev += ( data[i].petal_width - mean )*( data[i].petal_width - mean );
    }
    stdev /= float(n_data);
    stdev = std::sqrt(stdev);

    for( int i = 0; i < n_data ; i++ ) {
        data[i].petal_width = ( data[i].petal_width - mean ) / stdev;
    }
}

int main()
{
    // set nicer style for drawing than the ROOT default
    BCAux::SetStyle();

    // open log file
    BCLog::OpenLog("log.txt", BCLog::detail, BCLog::detail);

    // Read Iris data from file

    io::CSVReader<5> ifile("iris.csv");
    ifile.read_header( io::ignore_extra_column, "sepal.length","sepal.width","petal.length","petal.width","variety" );

    IrisDataCollection_t all_iris_data;
    iris_data_t iris_data;
    std::string variety;
    while(ifile.read_row( iris_data.sepal_length, iris_data.sepal_width, iris_data.petal_length, iris_data.petal_width, variety ) ) {
        variety.erase(remove( variety.begin(), variety.end(), '\"' ), variety.end()); // remove quotes from string

        if( variety == "Setosa" ) {
            iris_data.variety = IRIS_VARIETY::SETOSA;
         }
         else if( variety == "Versicolor" ) {
             iris_data.variety = IRIS_VARIETY::VERSICOLOR;
         }
         else if( variety == "Virginica" ) {
             iris_data.variety = IRIS_VARIETY::VIRGINICA;
         }
         else{
             throw std::runtime_error("Unknown variety: " + variety);
        }

        all_iris_data.push_back( iris_data );
    }

    const int n_data = all_iris_data.size();
    BCLog::OutSummary("Input data: found " + std::to_string(n_data) + " entries." );

    // Pre-process input data
    Standardize( all_iris_data );
    for( int i = 0; i < n_data; i++ ) {
        std::cout   << all_iris_data.at(i).sepal_length << " "
                    << all_iris_data.at(i).sepal_width << " "
                    << all_iris_data.at(i).petal_length << " "
                    << all_iris_data.at(i).petal_width << std::endl;
    }

    BCLog::OutSummary("Input data: done.");

    // create new DeepBayesIris object
    DeepBayesIris m("DeepBayesIris");

    // set precision
    //m.SetPrecision(BCEngineMCMC::kMedium);
    m.SetPrecision(BCEngineMCMC::kHigh);
    m.SetNIterationsPreRunMax( 1000000 );

    m.SetIrisData( all_iris_data );


    BCLog::OutSummary("Test model created");

    //////////////////////////////
    // perform your analysis here

    // Normalize the posterior by integrating it over the full par. space
    //m.Normalize();

    // run MCMC, marginalizing posterior
    m.MarginalizeAll(BCIntegrate::kMargMetropolis);

    // run mode finding; by default using Minuit
    m.FindMode(m.GetBestFitParameters());

    // draw all marginalized distributions into a PDF file
    m.PrintAllMarginalized(m.GetSafeName() + "_plots.pdf");

    // print summary plots
    m.PrintParameterPlot(m.GetSafeName() + "_parameters.pdf");
    m.PrintCorrelationPlot(m.GetSafeName() + "_correlation.pdf");
    m.PrintCorrelationMatrix(m.GetSafeName() + "_correlationMatrix.pdf");
    m.PrintKnowledgeUpdatePlots(m.GetSafeName() + "_update.pdf");

    // print results of the analysis into a text file
    m.PrintSummary();

    // close log file
    BCLog::OutSummary("Exiting");
    BCLog::CloseLog();

    return 0;
}
