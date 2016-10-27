#include "header/Net.h"


//
//
// MAIN

void showVectorVals(std::string label, t_vals &v)
{
    std::cout << label << " ";
    for (unsigned i = 0; i < v.size(); ++i)
        std::cout << v[i] << " ";

    std::cout << std::endl;
}

int main()
{
    //TrainingData trainData("trainsample/fingerp");
    TrainingData trainData("../trainsample/out_xor.txt");
    // TrainingData trainData("trainsample/out_and.txt");
    // TrainingData trainData("trainsample/out_or.txt");
    // TrainingData trainData("trainsample/out_no.txt");
    
    // e.g., { 3, 2, 1 }
    std::vector<unsigned> topology;
    trainData.getTopology(topology);

    Net myNet(topology);
    
    t_vals inputVals, targetVals, resultVals;
    int trainingPass = 0;

    while (!trainData.isEof()) 
    {
        ++trainingPass;
        std::cout << std::endl << "Pass " << trainingPass << std::endl;

        // Get new input data and feed it forward:
        if (trainData.getNextInputs(inputVals) != topology[0])
            break;

        showVectorVals("Inputs:", inputVals);
        myNet.feedForward(inputVals);

        // Collect the net's actual output results:
        myNet.getResults(resultVals);
        showVectorVals("Outputs:", resultVals);

        // Train the net what the outputs should have been:
        trainData.getTargetOutputs(targetVals);
        showVectorVals("Targets:", targetVals);
        assert(targetVals.size() == topology.back());

        myNet.backProp(targetVals);

        // Report how well the training is working, average over recent samples:
        std::cout << "Net current error: " << myNet.getError() << std::endl;
        std::cout << "Net recent average error: " << myNet.getRecentAverageError() << std::endl;

        if (trainingPass > 100 && myNet.getRecentAverageError() < 0.05)
        {
            std::cout << std::endl << "average error acceptable -> break" << std::endl;
            break;
        }
    }

    std::cout << std::endl << "Done" << std::endl;

    if (topology[0] == 2)
    {
        std::cout << "TEST" << std::endl;
        std::cout << std::endl;

        unsigned dblarr_test[4][2] = { {0,0}, {0,1}, {1,0}, {1,1} };
        // unsigned dblarr_test[1][16] = {{300,300,109,65,105,50,15,97,48,10,21,10,5,89,58,5}};
        for (unsigned i = 0; i < 4; ++i)
        {
            inputVals.clear();
            inputVals.push_back(dblarr_test[i][0]);
            inputVals.push_back(dblarr_test[i][1]);
            // inputVals.push_back(dblarr_test[i][2]);
            // inputVals.push_back(dblarr_test[i][3]);
            // inputVals.push_back(dblarr_test[i][4]);
            // inputVals.push_back(dblarr_test[i][5]);
            // inputVals.push_back(dblarr_test[i][6]);
            // inputVals.push_back(dblarr_test[i][7]);
            // inputVals.push_back(dblarr_test[i][8]);
            // inputVals.push_back(dblarr_test[i][9]);
            // inputVals.push_back(dblarr_test[i][10]);
            // inputVals.push_back(dblarr_test[i][11]);
            // inputVals.push_back(dblarr_test[i][12]);
            // inputVals.push_back(dblarr_test[i][13]);
            // inputVals.push_back(dblarr_test[i][14]);
            // inputVals.push_back(dblarr_test[i][15]);


            myNet.feedForward(inputVals);
            myNet.getResults(resultVals);

            showVectorVals("Inputs:", inputVals);
            showVectorVals("Outputs:", resultVals);

            std::cout << std::endl;
        }

        std::cout << "/TEST" << std::endl;
    }
}

// MAIN
//
//



