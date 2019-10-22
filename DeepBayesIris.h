// ***************************************************************
// This file was created using the bat-project script.
// bat-project is part of Bayesian Analysis Toolkit (BAT).
// BAT can be downloaded from http://mpp.mpg.de/bat
// ***************************************************************

#ifndef __BAT__DEEPBAYESIRIS__H
#define __BAT__DEEPBAYESIRIS__H

#include <BAT/BCModel.h>

#include <string>
#include <vector>
#include <string>

// This is a DeepBayesIris header file.
// Model source code is located in file DeepBayesIris/DeepBayesIris.cxx

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
//typedef std::vector<iris_data_t> IrisDataCollection_t;

// ---------------------------------------------------------
class DeepBayesIris : public BCModel
{

public:

    // Constructor
    DeepBayesIris(const std::string& name);

    // Destructor
    ~DeepBayesIris();

    // Overload LogLikelihood to implement model
    double LogLikelihood(const std::vector<double>& parameters);

    // Overload LogAprioriProbability if not using built-in 1D priors
    // double LogAPrioriProbability(const std::vector<double> & parameters);

    void SetIrisData( IrisDataCollection& data ) { m_iris_data = &data; };
    IrisDataCollection * GetIrisData() const { return m_iris_data; };

protected:
    void GetWeights( std::vector<double> parameters, std::vector<double>& weights, std::vector<double>& bias );
    void FeedForward( std::vector<double> &inputs, std::vector<double> &weights, std::vector<double> &bias, std::vector<double> & outputs );

    void ReLU( std::vector<double> & x );
    void Sigmoid( std::vector<double> & x );
    void Tanh( std::vector<double> & x );
    void Softmax( std::vector<double> & x );

private:
    IrisDataCollection * m_iris_data;

    int m_n_inputs;
    int m_n_hidden;
    int m_n_outputs;
};
// ---------------------------------------------------------

#endif
