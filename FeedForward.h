#ifndef __FEEDFORWARD_H__
#define __FEEDFORWARD_H__

#include <vector>

class FeedForward
{
    public:
        FeedForward();
        FeedForward( int n_inputs, int n_hidden, int n_outputs );
        virtual ~FeedForward();
    
        void SetWeights( const std::vector<double>& parameters );
        void predict( std::vector<double> &inputs, std::vector<double> &outputs );

    protected:
        int m_n_inputs;
        int m_n_hidden;
        int m_n_outputs;

        std::vector<double> m_Wh;
        std::vector<double> m_bh;
        std::vector<double> m_Wy;
        std::vector<double> m_by;

        void ReLU( std::vector<double> & x );
        void Sigmoid( std::vector<double> & x );
        void Tanh( std::vector<double> & x );
        void Softmax( std::vector<double> & x );
};

#endif /* __FEEDFORWARD_H__ */


