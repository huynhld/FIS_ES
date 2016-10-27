#ifndef TRAINNINGDATA_H
#define TRAINNINGDATA_H
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>

typedef std::vector<double> t_vals;
//
//
// TrainingData

// Silly class to read training data from a text file -- Replace This.
// Replace class TrainingData with whatever you need to get input data into the
// program, e.g., connect to a database, or take a stream of data from stdin, or
// from a file specified by a command line argument, etc.
class TrainingData
{
	private:
	    std::ifstream m_trainingDataFile;
	public:

	    TrainingData(const std::string filename);
	    bool isEof(void);
	    void getTopology(std::vector<unsigned> &topology);

	    // Returns the number of input values read from the file:
	    unsigned getNextInputs(t_vals &inputVals);
	    unsigned getTargetOutputs(t_vals &targetOutputVals);

	
};

#endif