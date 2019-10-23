#include "FeedForward.h"

#include <cmath>

FeedForward::FeedForward()
{
    m_n_inputs = 0;
    m_n_hidden = 0;
    m_n_outputs = 0;
}

FeedForward::FeedForward( int n_inputs, int n_hidden, int n_outputs ) : 
    m_n_inputs(n_inputs), m_n_hidden(n_hidden), m_n_outputs(n_outputs)
{
 
}

FeedForward::~FeedForward()
{

}

// ---------------------------------------------------------
void FeedForward::ReLU( std::vector<double> & x )
{
    for( size_t i = 0 ; i < x.size() ; i++ ) {
        x[i] = ( x[i]>0.) ? x[i] : 0.;
    }
}

void FeedForward::Sigmoid( std::vector<double> & x )
{
   for( size_t i = 0 ; i < x.size() ; i++ ) {
       double exp_value = exp((double) -x[i]);
       x[i] = 1. / (1. + exp_value);
   }
}

void FeedForward::Tanh( std::vector<double> & x )
{
    for( size_t i = 0 ; i < x.size() ; i++ ) {
        x[i] = tanh(x[i]);
    }
}

void FeedForward::Softmax( std::vector<double> & x )
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
void FeedForward::SetWeights( const std::vector<double>& parameters )
{
    m_Wh.clear();
    m_bh.clear();
    m_Wy.clear();
    m_by.clear();
    
    int k = 0;
    // hidden layer
    for( int i = 0 ; i < m_n_hidden ; i++ ) {
        for( int j = 0 ; j < m_n_inputs ; j++ ) {
            double w = parameters[k];
            m_Wh.push_back(w);
            k++;
        }

        double b = parameters[k];
        m_bh.push_back( b );
        k++;
    }

    // output layer
    for( int i = 0 ; i < m_n_outputs ; i++ ) {
        for( int j = 0 ; j < m_n_hidden ; j++ ) {
            double w = parameters[k];
            m_Wy.push_back(w);
            k++;
        }

        double b = parameters[k];
        m_by.push_back( b );
        k++;
    }
}

// ---------------------------------------------------------
void FeedForward::predict( const std::vector<double> &inputs, std::vector<double> &outputs )
{
    // h_i = sigma(Wh_ij * x_j + b_i )

    std::vector<double> hidden( m_n_hidden, 0. );

    //std::cout << inputs.size() << " " << weights.size() << " " << bias.size() << " " << outputs.size() <<  std::endl;

    // hidden layer
    int k = 0;
    for( int i = 0 ; i < m_n_hidden ; i++ ) {
        hidden[i] = m_bh.at(i);

        for( int j = 0 ; j < m_n_inputs ; j++ ) {
            hidden[i] += inputs.at(j) * m_Wh.at(k);
            k++;
        }
        
    }

    //ReLU( hidden );
    Tanh( hidden );

    // output layer
    k = 0;
    for( int i = 0 ; i < m_n_outputs ; i++ ) {
        outputs[i] = m_by[i];

        for( int j = 0 ; j < m_n_hidden ; j++ ) {
            outputs[i] += hidden[j] * m_Wy[k];
            k++;
        }
        
    }

    Softmax( outputs );
    //Sigmoid( outputs );
}