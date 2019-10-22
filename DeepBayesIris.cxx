// ***************************************************************
// This file was created using the bat-project script.
// bat-project is part of Bayesian Analysis Toolkit (BAT).
// BAT can be downloaded from http://mpp.mpg.de/bat
// ***************************************************************

#include "DeepBayesIris.h"

#include <stdio.h>
#include <iostream>
#include <cmath>

#include <BAT/BCMath.h>

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
    m_n_hidden = 2;
    m_n_outputs = 3;

    char pname[32];
    char desc[128];
    for( int i = 0 ; i < m_n_hidden ; i++ ) {
        for( int j = 0 ; j < m_n_inputs ; j++ ) {
            sprintf(pname, "Wh_%i_%i", i, j);
            sprintf(desc, "Wh_%i_%i", i, j);
            AddParameter( pname, -max_weight, max_weight, desc );
            GetParameters().Back().SetPriorConstant();
            std::cout << "Added param: " << desc << std::endl;
        }
        // bias
        sprintf(pname, "bh_%i", i );
        sprintf(desc, "bh_%i", i);
        AddParameter( pname, -max_weight, max_weight, desc);
        GetParameters().Back().SetPriorConstant();
        std::cout << "Added param: " << desc << std::endl;
    }

    for( int i = 0 ; i < m_n_outputs ; i++ ) {
        for( int j = 0 ; j < m_n_hidden ; j++ ) {
            sprintf(pname, "Wo_%i_%i", i, j);
            sprintf(desc, "Wo_%i_%i", i, j);
            AddParameter( pname, -max_weight, max_weight, desc );
            GetParameters().Back().SetPriorConstant();
            std::cout << "Added param: " << desc << std::endl;
        }
        // bias
        sprintf(pname, "bo_%i", i );
        sprintf(desc, "bo_%i", i);
        AddParameter( pname, -max_weight, max_weight, desc);
        GetParameters().Back().SetPriorConstant();
        std::cout << "Added param: " << desc << std::endl;
    }
    
    std::cout << "Number of parameters:" << GetNParameters() << std::endl;

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
}

void DeepBayesIris::ReLU( std::vector<double> & x )
{
    for( size_t i = 0 ; i < x.size() ; i++ ) {
        x[i] = ( x[i]>0.) ? x[i] : 0.;
    }
}

void DeepBayesIris::Sigmoid( std::vector<double> & x )
{
   for( size_t i = 0 ; i < x.size() ; i++ ) {
       double exp_value = exp((double) -x[i]);
       x[i] = 1. / (1. + exp_value);
   }
}

void DeepBayesIris::Tanh( std::vector<double> & x )
{
    for( size_t i = 0 ; i < x.size() ; i++ ) {
        x[i] = tanh(x[i]);
    }
}

void DeepBayesIris::Softmax( std::vector<double> & x )
{
    double N = 0.;
    for( size_t i = 0 ; i < x.size() ; i++ ) {
        N += std::exp( x[i] );
    }

    for( size_t i = 0 ; i < x.size() ; i++ ) {
        x[i] = std::exp(x[i]) / N;
    }
}

// ---------------------------------------------------------
void DeepBayesIris::FeedForward( std::vector<double> &inputs, 
                                std::vector<double> &Wh, std::vector<double> &Wy,
                                std::vector<double> &bh, std::vector<double> &by,
                                std::vector<double> & outputs )
{
    // h_i = sigma(Wh_ij * x_j + b_i )

    std::vector<double> hidden( m_n_hidden, 0. );

    //std::cout << inputs.size() << " " << weights.size() << " " << bias.size() << " " << outputs.size() <<  std::endl;

    // hidden layer
    int k = 0;
    for( int i = 0 ; i < m_n_hidden ; i++ ) {
        hidden[i] = bh.at(i);

        for( int j = 0 ; j < m_n_inputs ; j++ ) {
            hidden[i] += inputs.at(j) * Wh.at(k);
            k++;
        }
        
    }

    //ReLU( hidden );
    Tanh( hidden );

    // output layer
    k = 0;
    for( int i = 0 ; i < m_n_outputs ; i++ ) {
        outputs[i] = by[i];

        for( int j = 0 ; j < m_n_hidden ; j++ ) {
            outputs[i] += hidden[j] * Wy[k];
            k++;
        }
        
    }

    Softmax( outputs );
    //Sigmoid( outputs );
}

// ---------------------------------------------------------
void DeepBayesIris::GetWeights( std::vector<double> parameters, 
                                std::vector<double>& Wh, std::vector<double>& Wy, 
                                std::vector<double>& bh, std::vector<double>& by )
{
    int k = 0;
    // hidden layer
    for( int i = 0 ; i < m_n_hidden ; i++ ) {
        for( int j = 0 ; j < m_n_inputs ; j++ ) {
            double w = parameters[k];
            Wh.push_back(w);
            k++;
        }

        double b = parameters[k];
        bh.push_back( b );
        k++;
    }

    // output layer
    for( int i = 0 ; i < m_n_outputs ; i++ ) {
        for( int j = 0 ; j < m_n_hidden ; j++ ) {
            double w = parameters[k];
            Wy.push_back(w);
            k++;
        }

        double b = parameters[k];
        by.push_back( b );
        k++;
    }
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

    GetWeights( parameters, Wh, Wy, bh, by );
    //std::cout << "weights:" << weights.size() << " bias:" << bias.size() << std::endl;

    double L = 0;
    long N = m_iris_data->sepal_length.size();
    for( size_t i = 0 ; i < N ; i++ ) {

        inputs[0] = m_iris_data->sepal_length.at(i);
        inputs[1] = m_iris_data->sepal_width.at(i);
        inputs[2] = m_iris_data->petal_length.at(i);
        inputs[3] = m_iris_data->petal_width.at(i);

        //std::cout << "input: " << inputs[0] << " " << inputs[1] << " " << inputs[2] << " " << inputs[3] << std::endl;

        FeedForward( inputs, Wh, Wy, bh, by, outputs );

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
            L +=  ( outputs[i] - targets[i] )*( outputs[i] - targets[i] ) / float(N); // MSE
            //L -= targets[i] * log( outputs[i] ); // cross-entropy

            GetObservable(i).Value( outputs[i]);
        }

        //std::cout << "input " << i << ":" << inputs[0] << " " << inputs[1] << " " << inputs[2] << " " << inputs[3] << std::endl;
        //std::cout << "O=" << outputs[0] << " " << outputs[1] << " " << outputs[2] << std::endl;
        //std::cout << "T=" << targets[0] << " " << targets[1] << " " << targets[2] << std::endl;

    }
    
    return L;
}

// ---------------------------------------------------------
// double DeepBayesIris::LogAPrioriProbability(const std::vector<double> & parameters) {
// This returns the log of the prior probability for the parameters
// If you use built-in 1D priors, don't uncomment this function.
// }
