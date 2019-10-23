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

#include <TGraph.h>
#include <TFile.h>

#include <BAT/BCLog.h>
#include <BAT/BCAux.h>

#include "DeepBayesIris.h"

// taken from: https://github.com/ben-strasser/fast-cpp-csv-parser
#include "csv.h"

#define CSV_IO_NO_THREAD

#define STANDARDIZE(field)

void Standardize( std::vector<double> &data ) {
    const size_t n_data = data.size();
    double mean = 0.;
    double stdev = 0;

    // sepal_length
    mean = std::accumulate( data.begin(), data.end(), 0. ) / double(n_data);
    stdev = 0.;
    std::for_each( data.begin(), data.end(), [&](const double x) {
        stdev += ( x - mean ) * ( x - mean );
    });
    stdev = sqrt( stdev / double(n_data-1) );

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

int main()
{
    // set nicer style for drawing than the ROOT default
    BCAux::SetStyle();

    // open log file
    BCLog::OpenLog("log.txt", BCLog::detail, BCLog::detail);

    // Read Iris data from file

    io::CSVReader<5> ifile("iris.csv");
    ifile.read_header( io::ignore_extra_column, "sepal.length","sepal.width","petal.length","petal.width","variety" );

    IrisDataCollection all_iris_data;
    double sepal_length = 0.;
    double sepal_width = 0.;
    double petal_length = 0.;
    double petal_width = 0.;
    std::string s_variety;
    while(ifile.read_row( sepal_length, sepal_width, petal_length, petal_width, s_variety ) ) {
        s_variety.erase(remove( s_variety.begin(), s_variety.end(), '\"' ), s_variety.end()); // remove quotes from string

        IRIS_VARIETY variety;
        if( s_variety == "Setosa" ) {
            variety = IRIS_VARIETY::SETOSA;
         }
         else if( s_variety == "Versicolor" ) {
             variety = IRIS_VARIETY::VERSICOLOR;
         }
         else if( s_variety == "Virginica" ) {
             variety = IRIS_VARIETY::VIRGINICA;
         }
         else{
             throw std::runtime_error("Unknown variety: " + s_variety);
        }

        all_iris_data.sepal_length.push_back( sepal_length );
        all_iris_data.sepal_width.push_back( sepal_width );
        all_iris_data.petal_length.push_back( petal_length );
        all_iris_data.petal_width.push_back( petal_width );
        all_iris_data.variety.push_back( variety );
    }

    const int n_data = all_iris_data.sepal_length.size();
    BCLog::OutSummary("Input data: found " + std::to_string(n_data) + " entries." );

    // Pre-process input data
    Standardize( all_iris_data );
    for( int i = 0; i < n_data; i++ ) {
        std::cout   << all_iris_data.sepal_length.at(i) << " "
                    << all_iris_data.sepal_width.at(i) << " "
                    << all_iris_data.petal_length.at(i) << " "
                    << all_iris_data.petal_width.at(i) << std::endl;
    }

    BCLog::OutSummary("Input data: done.");

    // create new DeepBayesIris object
    DeepBayesIris m("DeepBayesIris");

    // set precision
    //m.SetPrecision(BCEngineMCMC::kLow);
    m.SetPrecision(BCEngineMCMC::kMedium);
    //m.SetPrecision(BCEngineMCMC::kHigh);
    //m.SetPrecision(BCEngineMCMC::kVeryHigh);
    //m.SetNIterationsPreRunMax( 5000000 );

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

    m.SaveWeights( "weights.txt" );

    std::vector<TGraph*> loss = m.GetLossGraphs();
    TFile * ofile = TFile::Open( "loss.root", "RECREATE" );
    for( int i = 0; i < loss.size() ; i++ ) {
        loss[i]->Write();
    }
    ofile->Close();

    // close log file
    BCLog::OutSummary("Exiting");
    BCLog::CloseLog();

    return 0;
}
