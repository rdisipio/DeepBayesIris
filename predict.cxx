#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string> 

using namespace std; 

#include "preprocessing.h"
#include "data_types.h"
#include "FeedForward.h"

#include <TFile.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TMatrixDSym.h>
#include <TMatrixD.h>
#include <TDecompChol.h>
#include <TRandom3.h>

// taken from: https://github.com/ben-strasser/fast-cpp-csv-parser
#include "csv.h"
#define CSV_IO_NO_THREAD

void tokenize( std::vector<std::string>& tokens, std::string& text, const char delimiter = ',' )
{
    std::stringstream buffer(text);
    std::string tmp;
    while( getline( buffer, tmp, delimiter ) ) {
        tokens.push_back( tmp );
    }
}

int main( int argc, char *argv[] )
{
    FeedForward * mlp = NULL;
    int n_inputs = 0;
    int n_hidden = 0;
    int n_outputs = 0;
    int idata = 0;
    int n_runs = 1000;

    if( argc > 0 ) idata = atoi( argv[1] );
    if( argc > 1 ) n_runs = atoi( argv[2] );

    // read data from file
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
    std::cout << "Input data: found " << n_data << " entries." << std::endl;

    Standardize( all_iris_data );

    std::cout << "Selected entry id = " << idata << std::endl;
    std::cout   << all_iris_data.sepal_length.at(idata) << " "
                << all_iris_data.sepal_width.at(idata) << " "
                << all_iris_data.petal_length.at(idata) << " "
                << all_iris_data.petal_width.at(idata) << std::endl;
    std::cout << "Real category: " << all_iris_data.variety.at(idata) << std::endl;
    
    const std::vector<double> inputs = {
            all_iris_data.sepal_length.at(idata),
            all_iris_data.sepal_width.at(idata),
            all_iris_data.petal_length.at(idata),
            all_iris_data.petal_width.at(idata),
        };
    // read weights

    std::vector<double> weights;
    std::vector<double> errors;

    std::ifstream wfile( "weights.txt" );
    std::string line;
    getline(wfile, line); // skip first line <network>
    getline(wfile, line); // this contains network structure

    std::vector<std::string> arch;
    tokenize( arch, line );
    n_inputs = stoi( arch.at(0).c_str() );
    n_hidden = stoi( arch.at(1).c_str() );
    n_outputs = stoi( arch.at(2).c_str() );
    std::cout << "Network architecture: inputs:" << n_inputs << " hidden:" << n_hidden << " outputs:" << n_outputs << endl;

    getline(wfile, line); // <network>
    getline(wfile, line); // <weights>
    // number of weights is:
    const int n_weights = n_inputs*n_hidden + n_hidden + n_outputs*n_hidden + n_outputs;

    TFile * rootfile = TFile::Open( "predict.root", "RECREATE" );
    TH2D * h_corr = new TH2D( "corr", "Correlation Matrix", n_weights, -0.5, n_weights-0.5, n_weights, -0.5, n_weights-0.5 );

    std::vector<std::string> w;
    for( int i = 0 ; i < n_weights ; i++ ) {
        getline(wfile, line);
        w.clear();
        tokenize( w, line );

        weights.push_back( stod(w.at(1)) );
        errors.push_back( stod(w.at(2)) );
    }
    getline(wfile, line); // </weights>

    std::cout << "Reading covariance matrix" << std::endl;
    getline(wfile, line); // <correlations>
    TMatrixD corr(n_weights, n_weights);
    TMatrixD sigma(n_weights, n_weights);
    for( int i = 0 ; i < n_weights ; i++ ) {
        sigma[i][i] = errors.at(i);

        getline(wfile, line);
        std::vector<string> tmp;
        tokenize(tmp, line);
        for( int j = 0 ; j < n_weights ; j++ ) {
            corr[i][j] = stod( tmp.at(j) ); 
            h_corr->SetBinContent( i+1, j+1, corr[i][j] );
        }
    }
    TMatrixD cov = sigma * corr * sigma;

    mlp = new FeedForward( n_inputs, n_hidden, n_outputs );

    // setup RNG
    std::cout << "Setting up RNG" << std::endl;
    TRandom3 * rng = new TRandom3();
    rng->SetSeed(12345);

    std::cout << "Calculating the nominal output" << std::endl;
    mlp->SetWeights( weights );
    std::vector<double> outputs_0(n_outputs, 0.);
    mlp->predict( inputs, outputs_0 );
    for( int i = 0; i < n_outputs ; ++i ) { 
        std::cout << "Category " << i << ": " << outputs_0.at(i) << std::endl;
    }

    std::cout << "Executing " << n_runs << " repeated executions..." << std::endl;
    std::vector< std::vector<double> > category(n_outputs);

    TH2D * h_output = new TH2D( "output", "Classifier output", n_outputs, -0.5, n_outputs-0.5, 100, 0., 1. );

    for( int irun = 0 ; irun < n_runs ; ++irun ) {

        // You can turn a standard normal random variable z~N(0,1) into a
        // normal random variable with mean μ and variance σ 
        // by first scaling by the square root of the variance (i.e., the standard deviation) 
        // and then moving the result to have the correct mean (adding μ)
        // x = μ + √σ * N(0,1)
        // The same idea holds in the multivariate case, but instead of having a scalar value for the variance 
        // we have a covariance matrix
        // What is the square root of a matrix? C = C^1/2 * (C^1/2)^T
        // The most common method is given by the Cholesky decompostion C = U^T U where U = upper triangular
        // Hence: X = μ + U^T*Z

        TDecompChol chol( cov );
        chol.Decompose();
        TMatrixD L( chol.GetU( ));
        L.T(); // lower triangular

        TMatrixD Z(1,n_weights);
        TMatrixD mu(n_weights,1);
        for( int i = 0 ; i < n_weights ; i++ ) {
            Z[0][i] = rng->Gaus(0.,1.); // standard normal distribution
            mu[i][0] = weights.at(i);
        }
        TMatrixD X = mu + L*Z.T();

        std::vector<double> parameters;
        for( int i = 0 ; i < n_weights ; i++ ) {
            parameters.push_back( X[i][0] );
        }
        mlp->SetWeights( parameters );

        std::vector<double> outputs(n_outputs, 0.);
        mlp->predict( inputs, outputs );
        //std::cout << outputs[0] << "," << outputs[1] << "," << outputs[2] << std::endl;

        for( int i = 0; i < n_outputs ; ++i ) { 
            double y = outputs[i];
            category[i].push_back( y );

            h_output->Fill( i, y );
        }

    }
    std::cout << "...done." << endl;

    std::vector<double> y( n_outputs, 0. );
    std::vector<double> dy( n_outputs, 0. );

    for( int i = 0; i < n_outputs ; ++i ) {
        double mean;
        double stdev;
        CalcMeanStdev( category[i], mean, stdev );
        y[i] = mean;
        dy[i] = stdev;

        std::cout << "Category " << i << ": " << mean << " +- " << stdev << std::endl;
    }

    if( mlp ) delete mlp;
    if( rng ) delete rng;

    rootfile->Write();
    rootfile->Close();

    return 0;
}