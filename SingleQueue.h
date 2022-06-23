/**
 * 
 * ----- Github Repository -----
 * https://github.com/evans-work/single-queue-simulation.git
*/

/**
 * SingleQueue.h
 * Header file containing definitions required for the simulation
*/
#include <string>
#include <vector>

struct Time{
    int hours;
    int mins;
};

struct Interval{
    int low;
    int high;
};

struct PeriodTableRow //IAT or Service table 
{ 
    int period;
    double probability;
    double cumProbability;
    Interval RNInterval;
};


struct MainOutputTableRow
{
    int customer;
    int IATRandomValue;
    int IAT;
    Time arrivalTimeOnClock;
    int serviceRandomValue;
    int serviceTime;
    Time serviceBeginTimeOnClock;
    Time serviceEndTimeOnClock;
    int noOfCustomersInQueue;
    int serverIdleTime;
    int delay;
};

struct Cumulatives{
    int queueLength;
    int serverIdleTime;
    int delay;
};

struct Stats{
    double serverUtilizationRate;
    double averageDelay;
    double averageNoOfCustomersQueuingPerHour;
};

class SingleQueue
{

    private:
        //populating IAT and Service tables
        void populateIATTable();
        void populateServiceTable();

        //generate single value for 1 customer to be used in the final table
        void generateIAT(int i);
        void generateArrivalTimeOnClock(int i);
        void generateServiceTime(int i);
        void generateServiceBeginTimeOnClock(int i);
        void generateServiceEndTimeOnClock(int i);
        void calculateNumberOfCustomersInQueue(int i);
        void calculateServerIdleTime(int i);
        void calculateDelay(int i);

        //IAT and Service Time tables
        double calculateCumulativeProbability(int i, std::vector<double>& propabilities);
        Interval calculateRandomIntervals(int i, double cumPropability, double previousRowHighInterval);

        //final calculations
        void calculateServerUtilizationRate();
        void calculateAverageDelay();
        void calculateAverageNumberOfCustomersQueueingPerHour();


    public:
        std::string IATTableTitles;
        std::vector<PeriodTableRow> IATTable;

        std::string serviceTableTitles;
        std::vector<PeriodTableRow> serviceTable;

        std::vector<double> IATProbabilities;
        std::vector<double> serviceProbabilities;
        std::vector<double> IATRandomValues;
        std::vector<double> serviceRandomValues;

        std::string mainOutputTableTitles;
        std::vector<MainOutputTableRow> mainOutputTable;

        //stores totals for selective values
        Cumulatives cumulatives;

        //stores statistics inferred from the simulation
        Stats stats;
        
        int noOfCustomers;
        double serverUtilizationRate;
        double AverageDelay;
        double AverageNumberOfCustomersQueueingPerHour;

        //constructor
        SingleQueue(int noOfCustomers, std::vector<double> IATProbabilities, std::vector<double> serviceProbabilities, std::vector<double> IATRandomValues, std::vector<double> serviceRandomValues);

        //calculate values for final table save results in file
        void simulate();
        void printResults();
    
};