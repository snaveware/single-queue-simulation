/**
 * Main.cpp
 * Contains main function
*/
#include <iostream>
#include <string>
#include<sstream>
#include <fstream>
#include "SingleQueue.h"
#include<vector>


/**
 * Execution Instructions using g++ compiler
 * Install g++
 * Run: g++ main.cpp SingleQueue.cpp SingleQueue.h
*/

/**
 * Struct to store default values
*/
struct Requirements{
    int noOfCustomers;
    std::vector<double> IATProbabilities;
    std::vector<double> serviceProbabilities;
    std::vector<double> IATRandomValues;
    std::vector<double> serviceRandomValues;

}reqs = {
    15,
    {0.20,0.25,0.30,0.15,0.10},
    {0.10,0.15,0.35,0.15,0.15,0.10},
    {82,91,12,77,90,75,33,61,19,58,41,54,52,16,86},
    {93,59,76,62,40,41,57,91,93,38,92,22,9,7,65}
};


/**
 * Prints a Menu showing the user Options to run the simulation
*/
void printOptions();

/**
 * Checks if each probabilit is < 1
 * Checks if cumulative probability is = 1;
*/
bool probabilitiesValid(std::vector<double> probabilities);

int inputNoOfCustomers();

/**
 * Displays probabilities and random values in a vector that is passed by reference
*/
void displayValues(std::vector<double>& values);

/**
 * Gets probabilities and random values as user input when necessary
 * Returns a vector of doubles containing all the new values 
*/ 
std::vector<double> getInput(int minSize);


int main(){
    
    int choice;

    while(true){

        

        printOptions();
        std::cout<<"Choice: "; 
        std::cin>> choice;

        switch (choice)
        {
            case 1 :
            {
                std::cout<< "\n Running Simulation...\n";
                SingleQueue sq(
                    reqs.noOfCustomers,
                    reqs.IATProbabilities,
                    reqs.serviceProbabilities,
                    reqs.IATRandomValues,
                    reqs.serviceRandomValues
                );

                sq.simulate();
                std::cout<< "\n saving output to 'output.csv'...\n";
                sq.printResults();
                break;
            }

            case 2 : {
                
                std::cout<< "\n----- Changing Number of Customers -----\n";
                int noOfCustomers = reqs.noOfCustomers;

                while(true){
                   noOfCustomers = inputNoOfCustomers();

                    if(noOfCustomers> 0 && noOfCustomers <= reqs.noOfCustomers){
                        break;
                    }

                    std::cerr<< "No of Customers cannot be greater than " << reqs.noOfCustomers<< " or less than 1 please try again."<<std::endl;

                }


                std::cout<< "\n No of Customers = "<<noOfCustomers<<std::endl;

                std::cout<< "\nRunning Simulation...\n";
                SingleQueue sq(
                    noOfCustomers, // use the new noOfCustomers and not the one in reqs
                    reqs.IATProbabilities,
                    reqs.serviceProbabilities,
                    reqs.IATRandomValues,
                    reqs.serviceRandomValues
                );

                sq.simulate();

                std::cout<< "\n saving output to 'output.csv'...\n";
                sq.printResults();
                break;
            }

            case 3 :
            {
                Requirements *newReqs = new Requirements;

                std::cout<< "\n----- Changing Number of Customers -----\n";
                
                newReqs->noOfCustomers = inputNoOfCustomers();

                std::cout<< "\n No of Customers = "<<newReqs->noOfCustomers<<std::endl;


                std::cout<< "\n----- Changing Inter-Arrival Probabilities -----\n";
                while (true)
                {
                    newReqs->IATProbabilities = getInput(0);
                    if(probabilitiesValid(newReqs->IATProbabilities)){
                        break;
                    }
                }
                
                
                std::cout<< "\n----- Changing Service Probabilities -----\n";
                while(true){
                    
                    newReqs->serviceProbabilities = getInput(0);

                    if(probabilitiesValid(newReqs->serviceProbabilities)){
                        break;
                    }

                }
                

                std::cout<< "\n----- Changing Inter-Arrival Random Values -----\n";
                newReqs->IATRandomValues = getInput(newReqs->noOfCustomers);

                std::cout<< "\n----- Changing Service Random Values -----\n";
                newReqs->serviceRandomValues = getInput(newReqs->noOfCustomers);

                std::cout<< "\n Running Simulation...\n";
                SingleQueue sq(
                    newReqs->noOfCustomers,
                    newReqs->IATProbabilities,
                    newReqs->serviceProbabilities,
                    newReqs->IATRandomValues,
                    newReqs->serviceRandomValues
                );

                sq.simulate();

                std::cout<< "\n saving output to 'output.csv'...\n";
                sq.printResults();
                break;
            }

            case 4 : 
            exit(0);
            break;
                
            
            default:
            std::cout<< "\nInvalid Choice. Please try again"<<std::endl;
            break;
        }

        
    }
    


    
    return 0;
}


void printOptions(){
    std::cout<< "\n1. Simulate with default values.\n";
    std::cout<< "\n2. Change the Number of Customers only, then simulate\n";
    std::cout<< "\n3. Change all values, then simulate\n";
    std::cout<< "\n4. Quit \n";
}

int inputNoOfCustomers(){

    int noOfCustomers;

    while(true){
        std::cout<<"No of Customers: ";
        std::cin>>noOfCustomers;

        if(noOfCustomers != 0){
            break;
        }

        std::cerr<< "Number of Customers cannot be 0. Please try Again.";
    }

    return noOfCustomers;
}

bool probabilitiesValid(std::vector<double> probabilities){
    int i;
    double total = 0;
    for(i = 0; i<probabilities.size(); i++){
        if(probabilities[i] > 1){
            std::cerr<< "A probability value cannot be greater than 1. Plealse try again";
            return false;
        }
        total = total + probabilities[i];
    }

    if(!(int) total == 1){
        std::cerr<< "Total Cumulative Probability should be equal to 1. Please try again";
        return false;
    }

    return true;
}

std::vector<double> getInput(int minCount){
    
    std::vector<double> userValues;
    std::string input,sstorage;

    
    int hasFailed = false; //checks if input has failed validation so that the loop can repeat

    std::cout<< "\nEnter values separated by a comma.\n";

    do{

        hasFailed = false;
        std::cout<< "Values: ";

        std::cin.clear();
        std::cin.sync();
        std::getline(std::cin,input);

        std::stringstream sstream(input);

        while(std::getline(sstream,sstorage,',')){ //separate inputs by comma

            try
            {
                if(!sstorage.empty()){
                    userValues.push_back(std::stod(sstorage));
                }
                
            }
            catch(const std::invalid_argument& e)
            {

                std::cerr << "All values must be numbers. Please try again"<< '\n';
                hasFailed = true;
            }
            
        }

        if(minCount != 0 && userValues.size() < minCount){

            std::cerr<< "Values must be more than " << minCount<< ". Please try again \n";
        }

    }while (hasFailed);
    

    return userValues;
}


