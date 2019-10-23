// ***************************************************************
// This file was created using the bat-project script.
// bat-project is part of Bayesian Analysis Toolkit (BAT).
// BAT can be downloaded from http://mpp.mpg.de/bat
// ***************************************************************

#include "DeepBayesIris.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cmath>

#include <BAT/BCMath.h>
#include <TH2D.h>

// ---------------------------------------------------------
DeepBayesIris::DeepBayesIris(const std::string& name)
    : BCModel(name)
{
    // Define parameters here in the constructor. For example:
    // AddParameter("mu",-2,1,"#mu");
    // And set priors, if using built-in priors. For example:
    // GetParamater("mu").SetPrior(new BCPriorGaus(-1, 0.25));

    // Structure of the neural network: 4(in) -> 5(hid) -> 3(out)
    // each hidden neuron has 4+1(bias) inputs
    const float max_weight = 100.;

    m_n_inputs = 4;
    m_n_hidden = 2; //5;
    m_n_outputs = 3;

    m_mlp = new FeedForward( m_n_inputs, m_n_hidden, m_n_outputs );

    int n_chains = GetNChains();
    char cname[32];
    for( int i = 0 ; i < n_chains; i++ ) {
        m_loss.push_back( new TGraph() );

        sprintf( cname, "chain_%i_loss", i);
        m_loss.at(i)->SetName(cname);
    }

    char pname[32];
    char desc[128];
    for( int i = 0 ; i < m_n_hidden ; i++ ) {
        for( int j = 0 ; j < m_n_inputs ; j++ ) {
            sprintf(pname, "Wh_%i_%i", i, j);
            sprintf(desc, "Wh_%i_%i", i, j);
            AddParameter( pname, -max_weight, max_weight, desc );
            //GetParameters().Back().SetPriorConstant();
            std::cout << "Added param: " << desc << std::endl;
        }
        // bias
        sprintf(pname, "bh_%i", i );
        sprintf(desc, "bh_%i", i);
        AddParameter( pname, -max_weight, max_weight, desc);
        //GetParameters().Back().SetPriorConstant();
        std::cout << "Added param: " << desc << std::endl;
    }

    for( int i = 0 ; i < m_n_outputs ; i++ ) {
        for( int j = 0 ; j < m_n_hidden ; j++ ) {
            sprintf(pname, "Wo_%i_%i", i, j);
            sprintf(desc, "Wo_%i_%i", i, j);
            AddParameter( pname, -max_weight, max_weight, desc );
            //GetParameters().Back().SetPriorConstant();
            std::cout << "Added param: " << desc << std::endl;
        }
        // bias
        sprintf(pname, "bo_%i", i );
        sprintf(desc, "bo_%i", i);
        AddParameter( pname, -max_weight, max_weight, desc);
        //GetParameters().Back().SetPriorConstant();
        std::cout << "Added param: " << desc << std::endl;
    }
    
    std::cout << "Number of parameters:" << GetNParameters() << std::endl;

    for( int i = 0 ; i < GetNParameters() ; i++ ) {
        SetPriorGauss( i, 0., max_weight/4. );
    }
    //SetFlagFillHistograms(false);

    for( int i = 0 ; i < m_n_outputs ; i++ ) {
        sprintf(pname, "y_%i", i );
        sprintf(desc, "y_%i", i);
        AddObservable( pname, 0., 1., desc );
    }
    SetFillHistogramObsObs( 0, 1 );
    SetFillHistogramObsObs( 0, 2 );
    SetFillHistogramObsObs( 1, 3 );
}

// ---------------------------------------------------------
DeepBayesIris::~DeepBayesIris()
{
    // destructor

    if( m_mlp ) delete m_mlp;

    /*
    int n_chains = GetNChains();
    for( int i = 0 ; i < n_chains; i++ ) {
        delete m_loss[i];
    }
    */
}

// ---------------------------------------------------------
double DeepBayesIris::LogLikelihood(const std::vector<double>& parameters)
{
    // This returns the log of the conditional probability p(data|pars)
    // This is where you define your model.
    // BCMath contains many functions you will find helpful

    std::vector<double> inputs( m_n_inputs, 0. );
    std::vector<double> Wh; 
    std::vector<double> Wy;
    std::vector<double> bh; 
    std::vector<double> by;
    std::vector<double> outputs( m_n_outputs, 0. );
    std::vector<double> targets( m_n_outputs, 0. );

    m_mlp->SetWeights( parameters );

    double L = 0;
    long N = m_iris_data->sepal_length.size();
    for( size_t i = 0 ; i < N ; i++ ) {

        inputs[0] = m_iris_data->sepal_length.at(i);
        inputs[1] = m_iris_data->sepal_width.at(i);
        inputs[2] = m_iris_data->petal_length.at(i);
        inputs[3] = m_iris_data->petal_width.at(i);

        //std::cout << "input: " << inputs[0] << " " << inputs[1] << " " << inputs[2] << " " << inputs[3] << std::endl;

        m_mlp->predict( inputs, outputs );

        if( m_iris_data->variety.at(i) == SETOSA ) {
            targets = { 1., 0., 0. };
        }
        else if( m_iris_data->variety.at(i) == VERSICOLOR ) {
            targets = { 0., 1., 0. };
        }
        else if ( m_iris_data->variety.at(i) == VIRGINICA ) {
            targets = { 0., 0., 1. };
        }

        for( size_t i = 0 ; i < m_n_outputs ; i++ ) {
            //L +=  ( outputs[i] - targets[i] )*( outputs[i] - targets[i] ) / float(N); // MSE
            L += targets[i] * log( outputs[i] ); // cross-entropy

            GetObservable(i).Value( outputs[i]);
        }

    }
    
    return L;
}

// ---------------------------------------------------------
// double DeepBayesIris::LogAPrioriProbability(const std::vector<double> & parameters) {
// This returns the log of the prior probability for the parameters
// If you use built-in 1D priors, don't uncomment this function.
// }

// ---------------------------------------------------------
void DeepBayesIris::MCMCUserIterationInterface()
{
    return;
    /*
    int nchains = GetNChains();
    for( int ichain = 0 ; ichain < nchains; ichain++ ) {
        double logP = GetLogProbx(ichain);

        
        long n = m_loss[ichain]->GetN();
        m_loss.at(ichain)->SetPoint( n, double(n), logP );
    }
    */
}

// ---------------------------------------------------------
void DeepBayesIris::SaveWeights( const std::string& fname )
{   
    std::ofstream ofile;
    ofile.open( fname );

    const int n_params = GetNParameters();

    ofile << "<network arch=\'mlp\'>\n";
    ofile << m_n_inputs << "," << m_n_hidden << "," << m_n_outputs << "\n";
    ofile << "</network>\n";

    ofile << "<weights>\n";
    for (unsigned i = 0; i < n_params; ++i) {
        double w = GetBestFitParameters()[i];
        double dw = GetBestFitParameterErrors()[i];

        const std::string& pname = GetParameter(i).GetName();

        ofile << pname << "," << w << "," << dw << "\n";
    }
    ofile << "</weights>\n";

    double corr[n_params][n_params];
    for (unsigned i = 0; i < n_params; ++i) {
        for (unsigned j = i; j < n_params; ++j) {
            if( i != j ) {
                TH2D * h = (TH2D*)GetMarginalizedHistogram(i,j);
                corr[i][j] = h->GetCorrelationFactor();
                corr[j][i] = corr[i][j];
            }
            else {
                corr[i][j] = 1.;
            }
        }
    }
    // now print out to file
    ofile << "<correlations>\n";
    for (unsigned i = 0; i < n_params; ++i) {
        for (unsigned j = 0; j < n_params; ++j) {
            ofile << corr[i][j];
            if( j < (n_params-1) ) ofile << ",";
        }
        ofile << "\n";
    }           
    ofile << "</correlations>\n";

    ofile.close();
}