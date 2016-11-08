// neural-net-tutorial.cpp
// David Miller, http://millermattson.com/dave
// See the associated video for instructions: http://vimeo.com/19569529


#include "../header/Net.h"


// using namespace std;

double Net::k_recentAvgSmoothingFactor = 100.0; // Number of training samples to average over

Net::Net(const std::vector<unsigned>& topology)
    :   m_error(0.0),
        m_recentAvgError(0.0)
{
    std::cout<<"log1"<<std::endl;
    
    assert( !topology.empty() ); // no empty topology
    std::cout<<"log2"<<std::endl;
    
    for (unsigned i = 0; i < topology.size(); ++i)
    {
        unsigned num_neuron = topology[i];

        assert( num_neuron > 0 ); // no empty layer

        m_layers.push_back(Layer());

        Layer& new_layer = m_layers.back();

        bool is_last_layer = (i == (topology.size() - 1));

        // 0 output if on the last layer
        unsigned numOutputs = ((is_last_layer) ? (0) : (topology[i + 1]));

        // We have a new layer, now fill it with neurons, and
        // add a bias neuron in each layer.
        for (unsigned j = 0; j < (num_neuron + 1); ++j) // add a bias neuron
            new_layer.push_back( Neuron(numOutputs, j) );

        // Force the bias node's output to 1.0 (it was the last neuron pushed in this layer):
        Neuron& bias_neuron = new_layer.back();
        bias_neuron.setOutputVal(1.0);
    }
}

void Net::feedForward(const t_vals &inputVals)
{
    assert( inputVals.size() == (m_layers[0].size() - 1) ); // exclude bias neuron

    // Assign (latch) the input values into the input neurons
    for (unsigned i = 0; i < inputVals.size(); ++i)
        m_layers[0][i].setOutputVal(inputVals[i]);

    // forward propagate
    for (unsigned i = 1; i < m_layers.size(); ++i) // exclude input layer
    {
        Layer& prevLayer = m_layers[i - 1];
        Layer& currLayer = m_layers[i];

        unsigned num_neuron = (currLayer.size() - 1); // exclude bias neuron
        for (unsigned n = 0; n < num_neuron; ++n)
            currLayer[n].feedForward(prevLayer);
    }
}

void Net::backProp(const t_vals &targetVals)
{
    //
    // error

    // Calculate overall net error (RMS of output neuron errors)

    Layer &outputLayer = m_layers.back();
    m_error = 0.0;

    for (unsigned n = 0; n < outputLayer.size() - 1; ++n)
    {
        double delta = targetVals[n] - outputLayer[n].getOutputVal();
        m_error += delta * delta;
    }
    m_error /= (outputLayer.size() - 1); // get average error squared
    m_error = sqrt(m_error); // RMS

    // Implement a recent average measurement

    m_recentAvgError =
            (m_recentAvgError * k_recentAvgSmoothingFactor + m_error)
            / (k_recentAvgSmoothingFactor + 1.0);

    // error
    //


    //
    // Gradients

    // Calculate output layer gradients

    for (unsigned n = 0; n < (outputLayer.size() - 1); ++n)
        outputLayer[n].calcOutputGradients(targetVals[n]);

    // Calculate hidden layer gradients

    for (unsigned i = (m_layers.size() - 2); i > 0; --i)
    {
        Layer &hiddenLayer = m_layers[i];
        Layer &nextLayer = m_layers[i + 1];

        for (unsigned n = 0; n < hiddenLayer.size(); ++n)
            hiddenLayer[n].calcHiddenGradients(nextLayer);
    }

    // Gradients
    //


    // For all layers from outputs to first hidden layer,
    // update connection weights

    for (unsigned i = (m_layers.size() - 1); i > 0; --i)
    {
        Layer &currLayer = m_layers[i];
        Layer &prevLayer = m_layers[i - 1];

        for (unsigned n = 0; n < (currLayer.size() - 1); ++n) // exclude bias
            currLayer[n].updateInputWeights(prevLayer);
    }
}

void Net::getResults(t_vals &resultVals) const
{
    resultVals.clear();

    const Layer& outputLayer = m_layers.back();

    // exclude last neuron (bias neuron)
    unsigned total_neuron = (outputLayer.size() - 1);

    for (unsigned n = 0; n < total_neuron; ++n)
        resultVals.push_back(outputLayer[n].getOutputVal());
}