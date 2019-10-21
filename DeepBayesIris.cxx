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
    const float max_weight = 10.;

    m_n_inputs = 4;
    m_n_hidden = 5;
    m_n_outputs = 3;

    char pname[32];
    char desc[128];
    for( int i = 0 ; i < m_n_hidden ; i++ ) {
        for( int j = 0 ; j < m_n_inputs ; j++ ) {
            sprintf(pname, "h_w%i%i", i, j);
            sprintf(desc, "Hidden %i, weight %i", i, j);
            AddParameter( pname, -max_weight, max_weight, desc );
            std::cout << "Added param: " << desc << std::endl;
        }
        // bias
        sprintf(pname, "h_b%i", i );
        sprintf(desc, "Hidden %i, bias", i);
        AddParameter( pname, -max_weight, max_weight, desc);
        std::cout << "Added param: " << desc << std::endl;
    }

    for( int i = 0 ; i < m_n_outputs ; i++ ) {
        for( int j = 0 ; j < m_n_hidden ; j++ ) {
            sprintf(pname, "o_w%i%i", i, j);
            sprintf(desc, "Output %i, weight %i", i, j);
            AddParameter( pname, -max_weight, max_weight, desc );
            std::cout << "Added param: " << desc << std::endl;
        }
        // bias
        sprintf(pname, "o_b%i", i );
        sprintf(desc, "Output %i, bias", i);
        AddParameter( pname, -max_weight, max_weight, desc);
        std::cout << "Added param: " << desc << std::endl;
    }
    
}

// ---------------------------------------------------------
DeepBayesIris::~DeepBayesIris()
{
    // destructor
}

void DeepBayesIris::Activation( std::vector<double> & x )
{
    for( size_t i = 0 ; i < x.size() ; i++ ) {
        x[i] = ( x[i]>0.) ? x[i] : 0.;
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
void DeepBayesIris::FeedForward( std::vector<double> inputs, std::vector<double> weights, std::vector<double> bias, std::vector<double> & outputs )
{
    std::vector<double> hidden( m_n_hidden, 0. );

    // hidden layer
    int k = 0;
    for( int i = 0 ; i < m_n_hidden ; i++ ) {
        for( int j = 0 ; j < m_n_inputs ; j++ ) {
            hidden[i] += inputs[j] * weights[k];
            k++;
        }
        hidden[i] += bias[i];
    }
    Activation( hidden );

    // output layer
    std::fill( outputs.begin(), outputs.end(), 0. );
    for( int i = 0 ; i < m_n_outputs ; i++ ) {
        for( int j = 0 ; j < m_n_hidden ; j++ ) {
            outputs[i] += hidden[j] * weights[k];
        }
        outputs[i] += bias[i];
        //outputs[i] = activation( outputs[i] );
    }
    Softmax( outputs );

}

// ---------------------------------------------------------
void DeepBayesIris::GetWeights( std::vector<double> parameters, std::vector<double>& weights, std::vector<double>& bias )
{
    int k = 0;
    // hidden layer
    for( int i = 0 ; i < m_n_hidden ; i++ ) {
        for( int j = 0 ; j < m_n_inputs ; j++ ) {
            double w = parameters[k];
            weights.push_back(w);
            k++;
        }

        double b = parameters[k];
        bias.push_back( b );
        k++;
    }
    // output layer
    for( int i = 0 ; i < m_n_outputs ; i++ ) {
        for( int j = 0 ; j < m_n_hidden ; j++ ) {
            double w = parameters[k];
            weights.push_back(w);
            k++;
        }

        double b = parameters[k];
        bias.push_back( b );
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
    std::vector<double> weights; 
    std::vector<double> bias; 
    std::vector<double> outputs( m_n_outputs, 0. );
    std::vector<double> targets( m_n_outputs, 0. );

    GetWeights( parameters, weights, bias );

    double L = 0;
    long N = m_iris_data->size();
    for( size_t i = 0 ; i < N ; i++ ) {
        inputs.clear();
        inputs[0] = m_iris_data->at(i).sepal_length;
        inputs[1] = m_iris_data->at(i).sepal_width;
        inputs[2] = m_iris_data->at(i).petal_length;
        inputs[3] = m_iris_data->at(i).petal_width;

        FeedForward( inputs, weights, bias, outputs );

        if( m_iris_data->at(i).variety == SETOSA ) {
            targets = { 1., 0., 0. };
        }
        else if( m_iris_data->at(i).variety == VERSICOLOR ) {
            targets = { 0., 1., 0. };
        }
        else if ( m_iris_data->at(i).variety == VIRGINICA ) {
            targets = { 0., 0., 1. };
        }

        for( size_t i = 0 ; i < m_n_outputs ; i++ ) {
            L +=  ( outputs[i] - targets[i] )*( outputs[i] - targets[i] ) / float(N);
        }

    }
    
    return L;
}

// ---------------------------------------------------------
// double DeepBayesIris::LogAPrioriProbability(const std::vector<double> & parameters) {
// This returns the log of the prior probability for the parameters
// If you use built-in 1D priors, don't uncomment this function.
// }
