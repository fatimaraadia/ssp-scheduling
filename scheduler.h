#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include "taskgen.h"
#include "security_relations.h"

class Scheduler {
public:
    // constructor
    explicit Scheduler(std::vector<Task>& taskSet, const SecurityRelations& secRel);

    void initializeBlockingParameters();
    void printBlockingParameters() const;

    double computeDBF(double L) const;
    bool isSchedulable();

    double computeDmax() const;
    double computeFeasibilityBound() const;

    std::vector<double> generateTestingSet1() const;
    std::vector<double> generateTestingSet2() const;

private:
    std::vector<Task>& taskSet;

    const SecurityRelations& securityRelations;
    
    bool adjustBlockingParameters(double L, double slack);
    int calculateCnt(const Task& task);
    int cnt;

    //helper functions
    double dbf(const Task& task, double L) const;
    double totalDBF(double L) const;
    double computeBlocking(double L) const;
    void adjustBlocking(double L);
    

};

#endif