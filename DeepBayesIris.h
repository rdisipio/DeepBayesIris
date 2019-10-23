// ***************************************************************
// This file was created using the bat-project script.
// bat-project is part of Bayesian Analysis Toolkit (BAT).
// BAT can be downloaded from http://mpp.mpg.de/bat
// ***************************************************************

#ifndef __BAT__DEEPBAYESIRIS__H
#define __BAT__DEEPBAYESIRIS__H

#include <BAT/BCModel.h>

#include <TGraph.h>

#include <string>
#include <vector>

#include "data_types.h"
#include "FeedForward.h"

// This is a DeepBayesIris header file.
// Model source code is located in file DeepBayesIris/DeepBayesIris.cxx

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

    std::vector<TGraph*>& GetLossGraphs() { return m_loss; };

    void SaveWeights( const std::string& fname = "weights.txt" );

protected:
    void MCMCUserIterationInterface();

    FeedForward * m_mlp;

private:
    IrisDataCollection * m_iris_data;

    std::vector<TGraph*> m_loss; 

    int m_n_inputs;
    int m_n_hidden;
    int m_n_outputs;
};
// ---------------------------------------------------------

#endif
