/**
 * SingleQueue.cpp
*/

#include <iostream>
#include <string>
#include <fstream>
#include "SingleQueue.h"

SingleQueue::SingleQueue(int noOfCustomers, std::vector<double> IATProbabilities, std::vector<double> serviceProbabilities, std::vector<double> IATRandomValues, std::vector<double> serviceRandomValues)
{

    this->noOfCustomers = noOfCustomers;

    IATTableTitles= "IAT,Probability,Cumulative Propability,Random Intervals";
    serviceTableTitles = "Service Time,Probability,Cumulative Probability,Random Interval";
    mainOutputTableTitles = "Customer,RN IAT,IAT,Arrival Time on Clock,RN Service,Service Time,Begin Service Time on Clock,End Service  Time on Clock,Queue (No of Customers In Queue),Server Idle Time,Delay";
    
    this->IATProbabilities = IATProbabilities;
    this->serviceProbabilities = serviceProbabilities;
    this->IATRandomValues = IATRandomValues;
    this->serviceRandomValues = serviceRandomValues;
    
    populateIATTable();
    populateServiceTable();

}


void SingleQueue::simulate()
{
    
    int i;
    cumulatives.queueLength=0;
    cumulatives.serverIdleTime=0;
    cumulatives.delay=0;

    for(i=0; i<noOfCustomers; i++){

        //initialize a new row to avoid memory error (segmentation fault)
        MainOutputTableRow row;
        mainOutputTable.push_back(row);
        
        generateIAT(i);
        generateArrivalTimeOnClock(i);
        generateServiceTime(i);
        generateServiceBeginTimeOnClock(i);
        generateServiceEndTimeOnClock(i);
        calculateNumberOfCustomersInQueue(i);
        calculateServerIdleTime(i);
        calculateDelay(i);

        cumulatives.queueLength = cumulatives.queueLength + mainOutputTable[i].noOfCustomersInQueue;
        cumulatives.serverIdleTime = cumulatives.serverIdleTime + mainOutputTable[i].serverIdleTime;
        cumulatives.delay = cumulatives.delay + mainOutputTable[i].delay;
    }

    calculateServerUtilizationRate();
    calculateAverageDelay();
    calculateAverageNumberOfCustomersQueueingPerHour();
}

void SingleQueue::populateIATTable()
{

    for(int i = 0; i<IATProbabilities.size(); i++){

        double previousRowHighInterval = 0;

        if(i != 0){
            previousRowHighInterval = IATTable[i-1].RNInterval.high;
        }
        
        PeriodTableRow row;
        
        row.period = i + 1;
        
        row.probability = IATProbabilities[i];
        double cumProbability = calculateCumulativeProbability(i,IATProbabilities);
        row.cumProbability = cumProbability;
        
        row.RNInterval = calculateRandomIntervals(i, cumProbability, previousRowHighInterval);
        
        IATTable.push_back(row);
        
    }
}

void SingleQueue::populateServiceTable()
{
   for(int i = 0; i<serviceProbabilities.size(); i++){
       double previousRowHighInterval = 0;

       if(i != 0 ){
            previousRowHighInterval = serviceTable[i-1].RNInterval.high;
       }

       PeriodTableRow row;
       row.period = i+1;
       double cumProbability = calculateCumulativeProbability(i,serviceProbabilities);
       row.cumProbability = cumProbability;
       row.RNInterval = calculateRandomIntervals(i,cumProbability,previousRowHighInterval);

       serviceTable.push_back(row);
   }
}


void SingleQueue::printResults()
{
    std::ofstream outputFile("output.csv");
    outputFile<<mainOutputTableTitles<<"\n";

    /**
     * Print The table
    */
    int i;
    for(i=0;i<noOfCustomers;i++){
        outputFile<<mainOutputTable[i].customer<<","<< mainOutputTable[i].IATRandomValue<<","<< mainOutputTable[i].IAT<<","<< mainOutputTable[i].arrivalTimeOnClock.hours<<":"<<mainOutputTable[i].arrivalTimeOnClock.mins<<","<<mainOutputTable[i].serviceRandomValue<<","<<mainOutputTable[i].serviceTime<<","<< mainOutputTable[i].serviceBeginTimeOnClock.hours<<":"<<mainOutputTable[i].serviceBeginTimeOnClock.mins<<"," << mainOutputTable[i].serviceEndTimeOnClock.hours<<":"<< mainOutputTable[i].serviceEndTimeOnClock.mins<<","<< mainOutputTable[i].noOfCustomersInQueue<<","<< mainOutputTable[i].serverIdleTime<<","<< mainOutputTable[i].delay<<"\n";
    }

    /**
     * Print Totals/cumulatives
    */
    outputFile<< ",,,,,,,,"<< cumulatives.queueLength <<","<< cumulatives.serverIdleTime <<","<< cumulatives.delay<< std::endl;
   
    /**
     * Print statistics
    */
    outputFile<<std::endl; //create space
    outputFile<<"Stat,Value"<<std::endl;
    outputFile<<"Server Utilization Rate"<<","<<stats.serverUtilizationRate<<std::endl;
    outputFile<<"Average Delay"<<","<<stats.averageDelay<<std::endl;
    outputFile<<"average No of Customers Queueing Per Hour"<<","<<stats.averageNoOfCustomersQueuingPerHour<<std::endl;
    outputFile.close();
}

void SingleQueue::generateIAT(int i)
{
    mainOutputTable[i].customer = i+1;
    mainOutputTable[i].IATRandomValue = IATRandomValues[i];

    int j;
    for(j=0; j<IATTable.size(); j++){

        if(mainOutputTable[i].IATRandomValue >= IATTable[j].RNInterval.low && mainOutputTable[i].IATRandomValue <= IATTable[j].RNInterval.high){
            mainOutputTable[i].IAT = IATTable[j].period;
            return;
        }
    }

    mainOutputTable[i].IAT = 0;
     
}

void SingleQueue::generateArrivalTimeOnClock(int i)
{

    if(i == 0){
        mainOutputTable[i].arrivalTimeOnClock.hours = 1;
        mainOutputTable[i].arrivalTimeOnClock.mins = mainOutputTable[i].IAT;
    }else{

        mainOutputTable[i].arrivalTimeOnClock.hours = mainOutputTable[i-1].arrivalTimeOnClock.hours;

        if(mainOutputTable[i-1].arrivalTimeOnClock.mins + mainOutputTable[i].IAT > 60){
            mainOutputTable[i].arrivalTimeOnClock.mins =  (mainOutputTable[i-1].arrivalTimeOnClock.mins + mainOutputTable[i].IAT) - 60;
            mainOutputTable[i].arrivalTimeOnClock.hours = mainOutputTable[i-1].arrivalTimeOnClock.hours + 1;
        }else{
            mainOutputTable[i].arrivalTimeOnClock.mins = mainOutputTable[i-1].arrivalTimeOnClock.mins + mainOutputTable[i].IAT;
        }

    }
     
}

void SingleQueue::generateServiceTime(int i){
    mainOutputTable[i].serviceRandomValue = serviceRandomValues[i];

    int j;
    for(j=0; j<serviceTable.size(); j++){

        if(mainOutputTable[i].serviceRandomValue >= serviceTable[j].RNInterval.low && mainOutputTable[i].serviceRandomValue <= serviceTable[j].RNInterval.high){
            mainOutputTable[i].serviceTime = serviceTable[j].period;
            return;
        }
    }

    mainOutputTable[i].serviceTime = 0;
}

void SingleQueue::generateServiceBeginTimeOnClock(int i){
    if(i == 0){
        mainOutputTable[i].serviceBeginTimeOnClock.hours = mainOutputTable[i].arrivalTimeOnClock.hours ;
        mainOutputTable[i].serviceBeginTimeOnClock.mins = mainOutputTable[i].arrivalTimeOnClock.mins;
    }else{
        
        mainOutputTable[i].serviceBeginTimeOnClock.hours = mainOutputTable[i-1].serviceEndTimeOnClock.hours;
        
        
        mainOutputTable[i].serviceBeginTimeOnClock.mins = mainOutputTable[i-1].serviceEndTimeOnClock.mins;
        
    }
}

void SingleQueue::generateServiceEndTimeOnClock(int i)
{

    
    if(mainOutputTable[i].serviceBeginTimeOnClock.mins + mainOutputTable[i].serviceTime >= 60 )
    {
        mainOutputTable[i].serviceEndTimeOnClock.hours = mainOutputTable[i].serviceBeginTimeOnClock.hours + 1;
        mainOutputTable[i].serviceEndTimeOnClock.mins = (mainOutputTable[i].serviceBeginTimeOnClock.mins + mainOutputTable[i].serviceTime)-60;
    
    }else{
        
        mainOutputTable[i].serviceEndTimeOnClock.hours = mainOutputTable[i].serviceBeginTimeOnClock.hours;

        mainOutputTable[i].serviceEndTimeOnClock.mins = mainOutputTable[i].serviceBeginTimeOnClock.mins + mainOutputTable[i].serviceTime;
    }
}

void SingleQueue::calculateNumberOfCustomersInQueue(int i)
{
    
    int j,queueLength=0;
    
    int currentCustomerArrivalTime = (mainOutputTable[i].arrivalTimeOnClock.hours*60)+ mainOutputTable[i].arrivalTimeOnClock.mins;
    int currentCustomerBeginTime = (mainOutputTable[i].serviceBeginTimeOnClock.hours*60) + mainOutputTable[i].serviceBeginTimeOnClock.mins;
    

    if(i!=0){
        for(j=0; j<i; j++){

            int customerInLoopEndTime = (mainOutputTable[j].serviceEndTimeOnClock.hours*60) + mainOutputTable[j].serviceEndTimeOnClock.mins;
            
            if(customerInLoopEndTime > currentCustomerArrivalTime){
                queueLength = queueLength+1;
            }
        }
    }
    mainOutputTable[i].noOfCustomersInQueue = queueLength;
}

void SingleQueue::calculateServerIdleTime(int i)
{
    if(i == 0){
        mainOutputTable[i].serverIdleTime = mainOutputTable[i].IAT;
    }else{
        mainOutputTable[i].serverIdleTime = 0;
    }
}

void SingleQueue::calculateDelay(int i)
{

    int arrivalTimeInMins = (mainOutputTable[i].arrivalTimeOnClock.hours * 60) + mainOutputTable[i].arrivalTimeOnClock.mins;
    int beginTimeInMins = (mainOutputTable[i].serviceBeginTimeOnClock.hours * 60) + mainOutputTable[i].serviceBeginTimeOnClock.mins;
    
    mainOutputTable[i].delay = beginTimeInMins - arrivalTimeInMins;

}


double SingleQueue::calculateCumulativeProbability(int i, std::vector<double>& propabilities)
{
    
    if(i==0){
        return propabilities[0];
    }

    double cumProbability = 0;

    for(int j=0; j<=i; j++){
        cumProbability = cumProbability + propabilities[j];
    }

    return cumProbability;
    
}


Interval SingleQueue::calculateRandomIntervals(int i, double cumPropability, double previousRowHighInterval)
{
    
    Interval interval;

    interval.low = previousRowHighInterval + 1;
    interval.high = (cumPropability*100)-1;

    if(i == 0){
        interval.low = 0;  
    }

    
    return interval;
}

void SingleQueue::calculateServerUtilizationRate(){
    /**
     * Simulation Ends when last person enters service.
     * time server is busy = time last person enters service - idle time.
    */

   int simulationTime = mainOutputTable[noOfCustomers-1].serviceBeginTimeOnClock.mins;
  
   int serverBusyTime = simulationTime - cumulatives.serverIdleTime;
 
   stats.serverUtilizationRate = (double)serverBusyTime / simulationTime;

   

}

void SingleQueue::calculateAverageDelay(){
    /**
     * average delay = total delay / no of customers
    */

   stats.averageDelay = (double)cumulatives.delay / noOfCustomers;
   
}


void SingleQueue::calculateAverageNumberOfCustomersQueueingPerHour(){
    /**
     * average no of customers queuing per hour = cumulative no of customers in queue/ total simulation time
    */
    int simulationTime = mainOutputTable[noOfCustomers-1].serviceBeginTimeOnClock.mins;

    stats.averageNoOfCustomersQueuingPerHour = (double)cumulatives.queueLength / simulationTime;
}



