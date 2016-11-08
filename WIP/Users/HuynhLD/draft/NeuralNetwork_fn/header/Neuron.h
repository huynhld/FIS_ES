#ifndef NEURON_H
#define NEURON_H
//
//
// NEURON
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>

class Neuron;

typedef std::vector<Neuron> Layer;

struct Connection
{
    double weight;
    double deltaWeight;
};

typedef std::vector<Connection> Connections;

class Neuron
{
public:
    Neuron(unsigned numOutputs, unsigned myIndex);

    inline void setOutputVal(double val) { m_outputVal = val; }
    inline double getOutputVal(void) const { return m_outputVal; }

    void feedForward(const Layer& prevLayer);
    void calcOutputGradients(double targetVal);
    void calcHiddenGradients(const Layer& nextLayer);
    void updateInputWeights(Layer& prevLayer);

private:
    static double eta;   // [0.0..1.0] overall net training rate
    static double alpha; // [0.0..n] multiplier of last weight change (momentum)
    static double transferFunction(double x);
    static double transferFunctionDerivative(double x);
    static double randomWeight(void) { return rand() / double(RAND_MAX); }
    double sumDOW(const Layer &nextLayer) const;
    double m_outputVal;
    Connections m_outputWeights;
    unsigned m_myIndex;
    double m_gradient; // used by the backpropagation
};

#endif